#include "Passes.h"

#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

namespace {
  bool addPassToFPM(StringRef Name, ModulePassManager &MPM,
                    ArrayRef<PassBuilder::PipelineElement>) {
    if (Name == "anti-debug") {
      MPM.addPass(AntiDebug());
      return true;
    }
    return false;
  }
}

PassPluginLibraryInfo getADPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "AntiDebug", LLVM_VERSION_STRING,
    [](PassBuilder &PB) {
      PB.registerPipelineParsingCallback(addPassToFPM);
    }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getADPluginInfo();
}
