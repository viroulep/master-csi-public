#include "Passes.h"

#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

namespace {
  bool addPassToFPM(StringRef Name, FunctionPassManager &FPM,
                    ArrayRef<PassBuilder::PipelineElement>) {
    if (Name == "ir2z3") {
      FPM.addPass(IR2Z3());
      return true;
    }
    return false;
  }
}

PassPluginLibraryInfo getZ3PluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "Z3Plugin", LLVM_VERSION_STRING,
    [](PassBuilder &PB) {
      PB.registerPipelineParsingCallback(addPassToFPM);
    }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getZ3PluginInfo();
}
