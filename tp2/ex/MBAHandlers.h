#ifndef MBAHANDLERS_H
#define MBAHANDLERS_H

#include "llvm/IR/IRBuilder.h"

#include <functional>

using HandlerTy = std::function<llvm::Value*(llvm::Instruction &I)>;

llvm::Value* HandleAdd(llvm::Instruction &I);

#endif
