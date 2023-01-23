#ifndef OCHANDLERS_H
#define OCHANDLERS_H

namespace llvm {
  class Instruction;
}

void HandleMBA(llvm::Instruction &I);
void HandleSoft(llvm::Instruction &I);
void HandleHard(llvm::Instruction &I);

#endif
