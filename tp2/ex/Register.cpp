#include "Passes.h"

#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

namespace {
  bool addPassToFPM(StringRef Name, FunctionPassManager &FPM,
                    ArrayRef<PassBuilder::PipelineElement>) {
    if (Name == "opaque-constants") {
      FPM.addPass(OpaqueConstants());
      return true;
    }
    return false;
  }
}

PassPluginLibraryInfo getOCPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "OpaqueConstants", LLVM_VERSION_STRING,
    [](PassBuilder &PB) {
      PB.registerPipelineParsingCallback(addPassToFPM);
    }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getOCPluginInfo();
}
