#ifndef PASSES_H
#define PASSES_H

#include "llvm/IR/PassManager.h"

struct IR2Z3 : public llvm::PassInfoMixin<IR2Z3> {
  llvm::PreservedAnalyses run(llvm::Function &F,
                              llvm::FunctionAnalysisManager &AM);
};

#endif
