#include "MBAHandlers.h"

#include "llvm/IR/Instruction.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Type.h"

using namespace llvm;

Value* HandleAdd(Instruction &I) {
  assert(I.getOpcode() == Instruction::Add);
  IRBuilder<> B(&I);

  // TODO: Recover the operands X,Y. See User::getOperand 
  // TODO: For the shift, carefully pick the appropriate function from IRBuilder. You may or may not have to create the constant "one" for it. See ConstantInt::get and Value::getType if needed.
  // TODO: Do ((X&Y)<<1 + (X^Y)). see IRBuilder<>

  Value* X = I.getOperand(0);
  Value* Y = I.getOperand(1);
  return B.CreateAdd(B.CreateShl(B.CreateAnd(X, Y), 1), B.CreateXor(X,Y));
}
