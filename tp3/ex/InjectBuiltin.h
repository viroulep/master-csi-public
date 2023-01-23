#ifndef INJECT_BUILTIN
#define INJECT_BUILTIN

#include <llvm/ADT/StringRef.h>

namespace llvm {
  class Module;
  class Function;
};

llvm::Function* InjectBuiltin(llvm::StringRef const CCode, llvm::Module &M, llvm::StringRef const F);

#endif
