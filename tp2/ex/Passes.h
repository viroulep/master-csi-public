#ifndef PASSES_H
#define PASSES_H

#include "llvm/IR/PassManager.h"
#include "llvm/IR/Function.h"

struct OpaqueConstants : public llvm::PassInfoMixin<OpaqueConstants> {
  llvm::PreservedAnalyses run(llvm::Function &F, llvm::FunctionAnalysisManager &);
  // Make sure it runs on optnone.
  static bool isRequired() { return true; }
};

#endif
