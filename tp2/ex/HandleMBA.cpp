#include "LiveValues.h"

#include "MBAHandlers.h"

#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instruction.h"

#include <random>

using namespace llvm;

void HandleMBA(Instruction &I) {
  std::mt19937 RandomEngine(0);

  unsigned constant_op_idx = isa<ConstantInt>(I.getOperand(0)) ? 0 : 1;
  Value* C = I.getOperand(constant_op_idx);
  Value* X = GetLiveValue(I, AbstractFilterFactory::get().createFilter(I));

  IRBuilder<> B(&I);

  // TODO if necessary, cast everything to a big integer type
  // see IRBuilder<>::CreateIntCast
  // TODO do  (-X+(C-C'))+(X+C') where X is a live variable, C is the original operand, C' is a new random operand. Use RandomEngine() to get a new random int
  // TODO obfuscate the three different adds using MBA
  // TODO replace the old operand by the new one using User::setOperand

}

