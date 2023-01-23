#include "Passes.h"

#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

namespace {

  bool addPassToFPM(StringRef Name, FunctionPassManager &FPM,
                    ArrayRef<PassBuilder::PipelineElement>) {
    if (Name == "mba") {
      FPM.addPass(MBA());
      return true;
    }
    return false;
  }
}

PassPluginLibraryInfo getMBAPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "MBA", LLVM_VERSION_STRING,
    [](PassBuilder &PB) {
      PB.registerPipelineParsingCallback(addPassToFPM);
    }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getMBAPluginInfo();
}
