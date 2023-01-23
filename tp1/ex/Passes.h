#ifndef PASSES_H
#define PASSES_H

#include "llvm/IR/PassManager.h"
#include "llvm/IR/Function.h"

// INLINE FROM HERE #PASS_MBA#
struct MBA : public llvm::PassInfoMixin<MBA> {
  llvm::PreservedAnalyses run(llvm::Function &F,
                              llvm::FunctionAnalysisManager &AM);
  // Make sure it runs on optnone.
  static bool isRequired() { return true; }
};
// TO HERE #PASS_MBA#

#endif
