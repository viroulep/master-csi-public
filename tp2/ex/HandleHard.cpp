#include "LiveValues.h"

#include "llvm/IR/Constants.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Transforms/Utils/Cloning.h"

using namespace llvm;

namespace {

bool InlineFunction(CallBase &CB) {
  InlineFunctionInfo IFI;
  return llvm::InlineFunction(CB, IFI, nullptr, true).isSuccess();
}


Function* GetFAbs(Module &M) {
  Type* Double = Type::getDoubleTy(M.getContext());
  return Intrinsic::getDeclaration(&M, Intrinsic::fabs, {Double});
}

Function* GetLeibiniz(Module &M) {
  if(Function *F = M.getFunction("internal.leibiniz")) {
    return F;
  }

  LLVMContext &C = M.getContext();

  Type* Double = Type::getDoubleTy(C);
  Type* I8 = Type::getInt8Ty(C);
  FunctionType* FTy = FunctionType::get(Double, I8, false);
  Function* F = Function::Create(FTy, GlobalValue::PrivateLinkage, "internal.leibiniz", &M);

  BasicBlock* Entry = BasicBlock::Create(C, "entry", F);
  BasicBlock* LoopHead = BasicBlock::Create(C, "loop_head", F);
  BasicBlock* LoopBody = BasicBlock::Create(C, "loop_body", F);
  BasicBlock* Exit = BasicBlock::Create(C, "exit", F);

  IRBuilder<> B(C);

  Value* N_Arg = F->arg_begin();
  Type* I64 = Type::getInt64Ty(C);
  Type* Bool = Type::getInt1Ty(C);

  Constant* Zero= ConstantInt::get(I64, 0);
  Constant* One = ConstantInt::get(I64, 1);
  Constant* Two = ConstantInt::get(I64, 2);
  Constant* FPZero = ConstantFP::get(Double, 0.0);
  Constant* FPOne = ConstantFP::get(Double, 1.0);
  Constant* FPmOne = ConstantFP::get(Double, -1.0);
  Constant* FPTwo = ConstantFP::get(Double, 2.0);
  Constant* FPFour = ConstantFP::get(Double, 4.0);

  // entry:
  //   sum = 0.0
  //   sign = (n%2 == 0) ? -1.0 : 1.0
  //   i = 2*n-1
  B.SetInsertPoint(Entry);
  Value* Sum_Init = FPZero;
  Value* N_Init = B.CreateIntCast(N_Arg, I64, false, "n.init");
  Value* I_Init = B.CreateSub(B.CreateShl(N_Init, 1), One, "i.init");
  Value* Sign_InitCond = B.CreateICmpEQ(B.CreateURem(N_Init, Two), Zero, "sign.cond");
  Value* Sign_Init = B.CreateSelect(Sign_InitCond, FPmOne, FPOne, "sign.init");
  B.CreateBr(LoopHead);

  // loop_head:
  //   phis
  //   n-- > 0
  B.SetInsertPoint(LoopHead);
  PHINode* N = B.CreatePHI(I64, 2, "n");
  PHINode* Sum = B.CreatePHI(Double, 2, "sum");
  PHINode* I = B.CreatePHI(I64, 2, "i");
  PHINode* Sign = B.CreatePHI(Double, 2, "sign");

  Value* N_Update = B.CreateSub(N, One, "n.update");
  Value* LoopCond = B.CreateICmpNE(N, Zero, "loop_cond");
  B.CreateCondBr(LoopCond, LoopBody, Exit);

  // loop_body:
  //   sum += sign/i;
  //   sign = -sign;
  //   i -= 2;
  B.SetInsertPoint(LoopBody);
  Value* Sum_Update = B.CreateFAdd(Sum, B.CreateFDiv(Sign, B.CreateUIToFP(I, Double)), "sum.update");
  Value* Sign_Update = B.CreateFMul(Sign, FPmOne, "sign.update");
  Value* I_Update = B.CreateSub(I, Two, "i.update");
  B.CreateBr(LoopHead);

  // exit:
  //   return 4*sum
  B.SetInsertPoint(Exit);
  B.CreateRet(B.CreateFMul(FPFour, Sum));

  // set the phi nodes
  N->addIncoming(N_Init, Entry);
  Sum->addIncoming(Sum_Init, Entry);
  I->addIncoming(I_Init, Entry);
  Sign->addIncoming(Sign_Init, Entry);

  N->addIncoming(N_Update, LoopBody);
  Sum->addIncoming(Sum_Update, LoopBody);
  I->addIncoming(I_Update, LoopBody);
  Sign->addIncoming(Sign_Update, LoopBody);

  return F;
}
}


void HandleHard(Instruction &I) {
  IRBuilder<> B(&I);

  Module &M = *I.getModule();
  Type *T = I.getType();

  unsigned constant_op_idx = isa<ConstantInt>(I.getOperand(0)) ? 0 : 1;
  Value* C = I.getOperand(constant_op_idx);
  Value* X = GetLiveValue(I, AbstractFilterFactory::get().createFilter(I));

  Function* FAbs = GetFAbs(M);
  Function* Leibiniz = GetLeibiniz(M);

  // TODO Use the leibiniz series to approximate Pi.
  // The leibiniz series, after 10 iterations gives an error smaller than 0.2
  // Bound the number of iteratons using some mask !
  // TODO Compare the approximation given by the leibiniz series to pi
  // TODO Inline the leibiniz function

}
