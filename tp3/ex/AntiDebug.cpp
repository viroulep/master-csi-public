#include "Passes.h"
#include "Injectors.h"
#include "InjectBuiltin.h"
#include "builtins.h"

#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

#include <random>

using namespace llvm;

static cl::opt<bool> Startup("startup", cl::init(true), cl::desc("Inject startup checks."));

static cl::opt<bool> Inject("inject", cl::init(true), cl::desc("Inject checks in the code."));


PreservedAnalyses AntiDebug::run(Module &M, ModuleAnalysisManager &) {
  if(Inject) {
    // TODO Inject the detect_readlink anti debug
    Function *DetectReadlink;
    if(!DetectReadlink)
      llvm::report_fatal_error("Could not load builtin", false);
    DetectReadlink->setLinkage(Function::PrivateLinkage);

    InsertInjectChecks(DetectReadlink);
  }

  if(Startup) {
    // TODO Inject the detect_ptrace anti debug
    Function *DetectPTrace;
    if(!DetectPTrace)
      llvm::report_fatal_error("Could not load builtin", false);
    DetectPTrace->setLinkage(Function::PrivateLinkage);

    InsertStartupCheck(DetectPTrace);
  }
  return (Startup || Inject) ? PreservedAnalyses::none() : PreservedAnalyses::all();
}
