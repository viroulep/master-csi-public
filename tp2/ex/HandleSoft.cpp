#include "LiveValues.h"

#include "llvm/IR/Constants.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"

using namespace llvm;

namespace {
Function* GetFAbs(Module &M) {
  Type* Double = Type::getDoubleTy(M.getContext());
  return Intrinsic::getDeclaration(&M, Intrinsic::fabs, {Double});
}

Function* GetLog2(Module &M) {
  Type* Double = Type::getDoubleTy(M.getContext());
  return Intrinsic::getDeclaration(&M, Intrinsic::log2, {Double});
}

Function* GetLog10(Module &M) {
  Type* Double = Type::getDoubleTy(M.getContext());
  return Intrinsic::getDeclaration(&M, Intrinsic::log10, {Double});
}

}

void HandleSoft(Instruction &I) {
  Module &M = *I.getModule();
  Function* Log2 = GetLog2(M);
  Function* Log10 = GetLog10(M);
  Function* FAbs = GetFAbs(M);

  Type *T = I.getType();

  unsigned constant_op_idx = isa<ConstantInt>(I.getOperand(0)) ? 0 : 1;
  Value* C = I.getOperand(constant_op_idx);
  Value* X = GetLiveValue(I, AbstractFilterFactory::get().createFilter(I));

  IRBuilder<> B(&I);

  // TODO P(x) = (l2(x) == log10(x) / log10(2)) . consider using a certain error instead of equality
  // casts from int to float must be used: IRBuilder<>::CreateUIToFP
  // TODO the new operand C' is equal to C^0x1. So C'^P(x) == C^0x1^P(x) and since P(x) returns always 1, C'^P(x) == C
  // BE CAREFUL, x should not be 0 since log(0)==NAN. Take x'=x|1
  Type* Floating = Log2->getFunctionType()->getReturnType();

}

