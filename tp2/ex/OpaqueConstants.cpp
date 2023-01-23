#include "Passes.h"

#include "AbstractFilterFactory.h"
#include "MBAHandlers.h"
#include "OCHandlers.h"

#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

#include <random>

using namespace llvm;

static cl::opt<double> Ratio("opaque-constants-ratio",
    cl::init(1.0), cl::desc("Ratio to control the OpaqueConstants pass"));

enum OCAlgo { MBA=0, Light, Heavy };

static cl::opt<unsigned> UseAlgo("opaque-constants-algo",
    cl::init(OCAlgo::MBA), cl::desc("Choose between algorithm types (values between 0-2)."));

bool IsBinopWithConstantOperand(Instruction &I) {
  // TODO check if it is a BinaryOperator with a ConstantInt operand
  // see isa<> dyn_cast<>, BinaryOperator

  return false;
}

PreservedAnalyses OpaqueConstants::run(Function &F, FunctionAnalysisManager &) {
  SmallVector<Instruction*, 8> Candidates;

  for(BasicBlock &B : F) {
    for(Instruction &I : B) {
      if(IsBinopWithConstantOperand(I))
          Candidates.push_back(&I);
    }
  }

  // Keep only Candidates.size()*ratio candidates
  std::mt19937 RandomEngine(0);
  std::shuffle(Candidates.begin(), Candidates.end(), RandomEngine);
  size_t N = std::max<size_t>(std::min<size_t>(Ratio * Candidates.size(), Candidates.size()), 0);
  Candidates.erase(Candidates.begin()+N, Candidates.end());

  auto Handle = (UseAlgo == 2) ? HandleHard : ((UseAlgo == 1) ? HandleSoft : HandleMBA);
  for(Instruction *I : Candidates) {
    Handle(*I);
  }

  return Candidates.empty() ? PreservedAnalyses::all() : PreservedAnalyses::none();
}
