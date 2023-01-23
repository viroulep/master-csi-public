#ifndef PASSES_H
#define PASSES_H

#include "llvm/IR/PassManager.h"
#include "llvm/IR/Module.h"

struct AntiDebug : public llvm::PassInfoMixin<AntiDebug> {
  llvm::PreservedAnalyses run(llvm::Module &M,
                              llvm::ModuleAnalysisManager &);
  // Make sure it runs on optnone.
  static bool isRequired() { return true; }
};

#endif
