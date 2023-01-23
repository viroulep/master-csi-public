#ifndef INJECTORS_H
#define INJECTORS_H

namespace llvm {
  class Function;
}

void InsertInjectChecks(llvm::Function* DetectReadlink);
void InsertStartupCheck(llvm::Function *DetectPTrace);

#endif
