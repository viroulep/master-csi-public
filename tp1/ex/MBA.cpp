#include "Passes.h"

#include "MBAHandlers.h"

#include "llvm/IR/Function.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

#include <random>

using namespace llvm;

static cl::opt<double> Ratio("mba-ratio",
    cl::init(1.0), cl::desc("Ratio to control the MBA pass"));

llvm::PreservedAnalyses MBA::run(Function &F, FunctionAnalysisManager &) {
  DenseMap<unsigned, HandlerTy> Handlers;
  Handlers.insert(std::make_pair<unsigned, HandlerTy>(Instruction::Add, HandleAdd));
  // TODO: Add handlers for A-B and A^B

  SmallVector<Instruction*, 8> Candidates;

  for(BasicBlock &B : F) {
    for(Instruction &I : B) {
      if(Handlers.count(I.getOpcode()))
          Candidates.push_back(&I);
    }
  }

  // Keep only Candidates.size()*ratio candidates
  std::mt19937 RandomEngine(0);
  std::shuffle(Candidates.begin(), Candidates.end(), RandomEngine);
  size_t N = std::max<size_t>(std::min<size_t>(Ratio * Candidates.size(), Candidates.size()), 0);
  Candidates.erase(Candidates.begin()+N, Candidates.end());

  for(Instruction *I : Candidates) {
    IRBuilder<> B(I);
    Value *NewI = Handlers[I->getOpcode()](*I);
    // TODO: replace old instruction with new, see Value::replaceAllUsesWith
  }

  for(Instruction *I : Candidates) {
    // TODO: erase old instructions, see Instruction::eraseFromParent 
  }

  return Candidates.empty() ? PreservedAnalyses::all() : PreservedAnalyses::none();
}
