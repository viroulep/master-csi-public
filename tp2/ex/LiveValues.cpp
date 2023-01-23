#include "LiveValues.h"

#include "llvm/ADT/SetVector.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"

#include <random>

using namespace llvm;

Value* GetLiveValue(Instruction &I, AbstractFilterFactory::FilterTy Filter) {
  BasicBlock &B = *I.getParent();
  Function &F = *B.getParent();

  SetVector<Value *> Values;

  auto AddValue = [&](Value &V) {
    if(!isa<Constant>(V) && V.getType()->isIntegerTy() && Filter(V))
      Values.insert(&V);
  };

  std::for_each(F.arg_begin(), F.arg_end(), AddValue);
  std::for_each(B.begin(), I.getIterator(), AddValue);
  std::for_each(B.getFirstNonPHI()->getIterator(), B.end(), [&](Instruction &II) {
    for(Value* OII : II.operand_values()) {
      Instruction *OIII = dyn_cast<Instruction>(OII);
      if(!OIII || OIII->getParent() != &B)
        AddValue(*OII);
    }
  });

  if(Values.empty())
    return ConstantInt::get(Type::getInt64Ty(I.getContext()), 0);

  // keep the random engine initialized with a fixed seed
  static std::mt19937 engine(0);
  std::uniform_int_distribution<size_t> random_dist{0, Values.size()-1};

  size_t idx = random_dist(engine);
  return Values[idx];
}

