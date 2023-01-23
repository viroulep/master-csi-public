#include "AbstractFilterFactory.h"

namespace llvm {
  class Value;
  class Instruction;
}

llvm::Value* GetLiveValue(llvm::Instruction &I, AbstractFilterFactory::FilterTy Filter);
