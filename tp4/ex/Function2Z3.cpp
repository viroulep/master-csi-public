#include "Passes.h"

#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

#include <z3++.h>
#include <random>
#include <map>

using namespace llvm;

namespace {

z3::expr getPathCondition(z3::context &Z3, BasicBlock &From, BasicBlock &To,
                          std::map<Value*, z3::expr> &Inst2Cond,
                          std::map<BasicBlock*, z3::expr> &BB2Cond);

// takes a value V and returns its z3 representation
z3::expr getValueFor(z3::context &Z3, Value* V, std::map<Value*, z3::expr> &Inst2Cond) {

  auto Inst = Inst2Cond.find(V);
  if(Inst != Inst2Cond.end()) {
    return Inst->second;
  }

  if(ConstantInt* CI = dyn_cast<ConstantInt>(V)) {
    uint64_t Val = CI->getZExtValue();
    return Z3.bv_val(Val, CI->getType()->getPrimitiveSizeInBits());
  }

  if(Argument * A = dyn_cast<Argument>(V)) {
    Type* T = A->getType();
    // Since LLVM 10 argument names are discarded in the IR, so name the arg
    // based on its position.
    llvm::Twine VarName = "param_" + std::to_string(A->getArgNo());
    if(T->isIntegerTy()) {
      z3::expr FreeVar = Z3.bv_const(VarName.str().c_str(), T->getPrimitiveSizeInBits());
      return FreeVar;
    }
  }

  if(IntegerType* T = dyn_cast<IntegerType>(V->getType())) {
    std::string buf; { llvm::raw_string_ostream sbuf(buf); sbuf << *V; }
    z3::expr FreeVar = Z3.bv_const(buf.c_str(), T->getPrimitiveSizeInBits());
    return FreeVar;
  }

  errs() << "unkown value: " << *V << "\n";
  return Z3.bv_val(0U, std::max<unsigned>(V->getType()->getPrimitiveSizeInBits(), 1));
}

// this function should be only called for i1
z3::expr getBoolValueFor(z3::context &Z3, Value* V, std::map<Value*, z3::expr> &Inst2Cond) {
  assert(V->getType()->isIntegerTy() && V->getType()->getScalarSizeInBits() == 1);
  z3::expr VExpr = getValueFor(Z3, V, Inst2Cond);
  return VExpr == Z3.bv_val(1, 1);
}

// maps an instruction to a z3 formula
void Inst2Z3(z3::context &Z3, Instruction &I, std::map<Value*, z3::expr> &Inst2Cond, std::map<BasicBlock*, z3::expr> &BB2Cond) {
  Type* T = I.getType();

  std::map<unsigned, std::function<z3::expr(z3::expr, z3::expr)>> Bin2Z3;
  Bin2Z3.insert({Instruction::Add, [](z3::expr L, z3::expr R) { return L+R; } });
  Bin2Z3.insert({Instruction::Sub, [](z3::expr L, z3::expr R) { return L-R; } });
  Bin2Z3.insert({Instruction::Mul, [](z3::expr L, z3::expr R) { return L*R; } });
  Bin2Z3.insert({Instruction::Xor, [](z3::expr L, z3::expr R) { return L^R; } });
  Bin2Z3.insert({Instruction::And, [](z3::expr L, z3::expr R) { return L&R; } });
  Bin2Z3.insert({Instruction::Or, [](z3::expr L, z3::expr R) { return L|R; } });
  Bin2Z3.insert({Instruction::Shl, [](z3::expr L, z3::expr R) { return z3::shl(L, R); } });
  Bin2Z3.insert({Instruction::LShr, [](z3::expr L, z3::expr R) { return z3::lshr(L, R); } });
  Bin2Z3.insert({Instruction::AShr, [](z3::expr L, z3::expr R) { return z3::ashr(L, R); } });
  Bin2Z3.insert({Instruction::URem, [](z3::expr L, z3::expr R) { return z3::urem(L, R); } });
  Bin2Z3.insert({Instruction::SRem, [](z3::expr L, z3::expr R) { return z3::srem(L, R); } });
  Bin2Z3.insert({Instruction::UDiv, [](z3::expr L, z3::expr R) { return z3::udiv(L, R); } });
  Bin2Z3.insert({Instruction::SDiv, [](z3::expr L, z3::expr R) { return L/R; } });

  if(Bin2Z3.count(I.getOpcode()) && T->isIntegerTy()) {
    z3::expr L = getValueFor(Z3, I.getOperand(0), Inst2Cond);
    z3::expr R = getValueFor(Z3, I.getOperand(1), Inst2Cond);
    z3::expr IExpr = Bin2Z3[I.getOpcode()](L, R);
    Inst2Cond.insert({&I, IExpr});
    return;
  }

  if(ICmpInst* Cmp = dyn_cast<ICmpInst>(&I)) {
    std::map<llvm::CmpInst::Predicate, std::function<z3::expr(z3::expr,z3::expr)>> CmpHandlers;
    CmpHandlers.insert({CmpInst::ICMP_EQ, [](z3::expr a, z3::expr b) { return a == b; } });
    CmpHandlers.insert({CmpInst::ICMP_NE, [](z3::expr a, z3::expr b) { return a != b; } });
    CmpHandlers.insert({CmpInst::ICMP_ULT, [](z3::expr a, z3::expr b) { return z3::ult(a, b); } });
    CmpHandlers.insert({CmpInst::ICMP_ULE, [](z3::expr a, z3::expr b) { return z3::ule(a, b); } });
    CmpHandlers.insert({CmpInst::ICMP_UGT, [](z3::expr a, z3::expr b) { return z3::ugt(a,b); } });
    CmpHandlers.insert({CmpInst::ICMP_UGE, [](z3::expr a, z3::expr b) { return z3::uge(a,b); } });
    CmpHandlers.insert({CmpInst::ICMP_SLT, [](z3::expr a, z3::expr b) { return a < b; } });
    CmpHandlers.insert({CmpInst::ICMP_SLE, [](z3::expr a, z3::expr b) { return a <= b; } });
    CmpHandlers.insert({CmpInst::ICMP_SGT, [](z3::expr a, z3::expr b) { return a > b; } });
    CmpHandlers.insert({CmpInst::ICMP_SGE, [](z3::expr a, z3::expr b) { return a >= b; } });

    auto CmpExpr = CmpHandlers[Cmp->getPredicate()](getValueFor(Z3, I.getOperand(0), Inst2Cond), getValueFor(Z3, I.getOperand(1), Inst2Cond));

    // is a bool, transform to bitvec
    CmpExpr = z3::ite(CmpExpr, Z3.bv_val(1, 1), Z3.bv_val(0, 1));

    Inst2Cond.insert({Cmp, CmpExpr});
    return;
  }

  if(CastInst* Cast = dyn_cast<CastInst>(&I)) {

    Value* X = Cast->getOperand(0);

    if(T->isIntegerTy() && X->getType()->isIntegerTy()) {
      z3::expr CastExpr = Z3.bv_val(0, T->getPrimitiveSizeInBits());
      z3::expr X_Expr = getValueFor(Z3, X, Inst2Cond);

      IntegerType* IT = cast<IntegerType>(X->getType());
      IntegerType* OT = cast<IntegerType>(T);
      if(OT->getPrimitiveSizeInBits() >= IT->getPrimitiveSizeInBits()) { // extend
        if(Cast->getOpcode() == Instruction::SExt) {
          CastExpr = z3::sext(X_Expr, OT->getPrimitiveSizeInBits() - IT->getPrimitiveSizeInBits());
        } else {
          CastExpr = z3::zext(X_Expr, OT->getPrimitiveSizeInBits() - IT->getPrimitiveSizeInBits());
        }
      } else { // trunc
        CastExpr = X_Expr.extract(OT->getPrimitiveSizeInBits()-1, 0);
      }

      Inst2Cond.insert({Cast, CastExpr});
      return;
    }
  }

  if(SelectInst* S = dyn_cast<SelectInst>(&I)) {
    // TODO: Create the expression for the select instruction. Use the z3::ite function.
    // Use z3::ite (IfThenElse). It takes a boolean condition, a true-case formula and a false-case formula
  }

  if(PHINode* PHI = dyn_cast<PHINode>(&I)) {
    // TODO Create the expression for the phi-nodes. The condition of the incoming block must be taken into account.
    // Again, use the z3::ite
  }

  if(!T->isVoidTy()) {
    Inst2Cond.insert({&I, getValueFor(Z3, &I, Inst2Cond)});
  }
}

z3::expr getPathCondition(z3::context &Z3, BasicBlock &From, BasicBlock &To,
                          std::map<Value*, z3::expr> &Inst2Cond,
                          std::map<BasicBlock*, z3::expr> &BB2Cond) {
  z3::expr BrCond = Z3.bool_val(false);

  Instruction* Term = From.getTerminator();
  if(BranchInst* Br = dyn_cast<BranchInst>(Term)) {
    if(Br->isUnconditional()) {
      // TODO If it is an unconditional branch, always taken
      // See BranchInst::getCondition()
      // See BranchInst::getSuccessor()
    } else {
      Value* Cond = Br->getCondition();
      // TODO If it is a conditional branch, depends if B is the true branch or the false branch (or both !)
    }
  }
  else if(SwitchInst* Sw = dyn_cast<SwitchInst>(Term)) {
    // TODO If it is a switch, it may be the default (SwitchInst::getDefaultDest) or some of the cases
    // see SwitchInst::getDefaultDest() SwitchInst::getCondition() and SwitchInst::cases()
    // see also SwitchInst::CaseIt::getCaseValue() and SwitchInst::CaseIt::getCaseSuccessor()
  }

  return BrCond && BB2Cond.find(&From)->second; // the condition of from->to is the condition to take the edge plus the condition to reach from
}

std::pair<std::map<Value*, z3::expr>, std::map<BasicBlock*, z3::expr>> Function2Z3(z3::context &Z3, Function &F) {

  std::map<BasicBlock*, z3::expr> BB2Cond;
  std::map<Value*, z3::expr> Inst2Cond;
  std::map<BasicBlock*, size_t> Predecessors;

  for(BasicBlock &B : F) {
    Predecessors[&B] = std::distance(pred_begin(&B), pred_end(&B));
  }

  BasicBlock* Entry = &F.getEntryBlock();

  SmallVector<BasicBlock*, 8> Q;
  for(BasicBlock &B : F) // push entry and unreachable blocks
    if(pred_begin(&B) == pred_end(&B))
      Q.push_back(&B);

  // visit the basic blocks in topological order. notice that if there is a loop the algorithm will stop early !
  while(!Q.empty()) {
    BasicBlock* B = Q.pop_back_val();

    // get the condition to reach B:
    // * B is the entry,
    // * or is reachable by a predecessor P. For every P, consider the path condition from P->B (te getPathCondition function)
    z3::expr BCond = Z3.bool_val(B == Entry);
    for(BasicBlock *P : predecessors(B)) {
      BCond = BCond || getPathCondition(Z3, *P, *B, Inst2Cond, BB2Cond);
    }
    BB2Cond.insert({B, BCond});

    // get the condition for each instruction
    for(Instruction &I : *B) {
      Inst2Z3(Z3, I, Inst2Cond, BB2Cond);
    }

    // add the successors to the queue
    for(BasicBlock *S : successors(B)) {
      if((--Predecessors[S]) == 0)
        Q.push_back(S);
    }
  }

  return {Inst2Cond, BB2Cond};
}

void Solve(Function &F, llvm::CmpInst::Predicate P, uint64_t Y) {
  z3::context Z3;
  auto F2Z3Expr = Function2Z3(Z3, F);
  auto YExpr = Z3.bv_val(Y, F.getFunctionType()->getReturnType()->getPrimitiveSizeInBits());

  // construct the return condition
  z3::expr FReturnCond = Z3.bool_val(false);

  for(BasicBlock &B : F) {
    ReturnInst* R = dyn_cast<ReturnInst>(B.getTerminator());
    if(!R)
      continue;

    auto BlockExprIt = F2Z3Expr.second.find(&B);
    if(BlockExprIt == F2Z3Expr.second.end())
      continue;

    auto BlockExpr = BlockExprIt->second; // the condition to reach the block
    Value* ReturnValue = R->getReturnValue();

    z3::expr ReturnExpr= Z3.bool_val(false);
    if(F2Z3Expr.first.count(ReturnValue)) {
      ReturnExpr = F2Z3Expr.first.find(ReturnValue)->second;
    } else {
      ReturnExpr = getValueFor(Z3, ReturnValue, F2Z3Expr.first);
    }

    z3::expr SolverCond = Z3.bool_val(false);

    switch(P) {
      case CmpInst::ICMP_EQ:
        { SolverCond = ReturnExpr == YExpr; break; }
      case CmpInst::ICMP_NE:
        { SolverCond = ReturnExpr != YExpr; break; }
      case CmpInst::ICMP_ULT:
        { SolverCond = ReturnExpr < YExpr; break; }
      case CmpInst::ICMP_UGT:
        { SolverCond = ReturnExpr > YExpr; break; }
      default:
        assert(false);
    }
    FReturnCond = FReturnCond || (SolverCond && BlockExpr);
  }

  z3::solver S(Z3);
  S.add(FReturnCond.simplify());
  if(S.check() == z3::unsat) {
    errs() << "no solution\n";
    return;
  } else if (S.check() == z3::unknown) {
    errs() << "unknown solution\n";
    return;
  }

  z3::model M = S.get_model();
  std::stringstream M_serialized;
  M_serialized << M ;
  M_serialized.flush();

  errs() << "model:\n";
  errs() << M_serialized.str() << "\n";
}
}

PreservedAnalyses IR2Z3::run(Function &F, FunctionAnalysisManager &) {
  if(F.isDeclaration() || F.getFunctionType()->getReturnType()->isVoidTy())
    return PreservedAnalyses::all();

  if(!F.hasSection())
    return PreservedAnalyses::all();

  StringRef SectionName = F.getSection();
  if(!SectionName.startswith("solve."))
    return PreservedAnalyses::all();

  std::map<std::string, llvm::CmpInst::Predicate> Comparisons;
  Comparisons.insert(std::make_pair("eq", CmpInst::ICMP_EQ));
  Comparisons.insert(std::make_pair("ne", CmpInst::ICMP_NE));
  Comparisons.insert(std::make_pair("ul", CmpInst::ICMP_ULT)); // unsigned lower than
  Comparisons.insert(std::make_pair("ug", CmpInst::ICMP_UGT)); // unsigned greater than

  auto ComparisonBegin = 6;
  auto ComparisonEnd = SectionName.rfind('.');
  std::string Comparison = SectionName.substr(ComparisonBegin, ComparisonEnd - ComparisonBegin).str();

  if(ComparisonEnd == std::string::npos || !Comparisons.count(Comparison)) {
    errs() << "wrong format!\n";
    return PreservedAnalyses::all();
  }

  std::string CompareWithStr = std::string(SectionName).substr(ComparisonEnd+1);
  if(CompareWithStr.empty() ||
     !std::all_of(CompareWithStr.begin(), CompareWithStr.end(),
       [](char c) { return isdigit(c); })) {
    errs() << "wrong format!\n";
    return PreservedAnalyses::all();
  }

  uint64_t CompareWith = std::stoul(CompareWithStr);

  Solve(F, Comparisons[Comparison], CompareWith);

  return PreservedAnalyses::all();
}
