#include "Injectors.h"

#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;

static cl::opt<unsigned> InjectThreashold("inject-threshold", cl::init(0), cl::desc("Inject checks in executed functions under a threashold. 0 means do not use profile."));

namespace {
bool IsInjectCandidateFun(Function &F) {
  if(InjectThreashold == 0)
    return true;

  auto EntryCount = F.getEntryCount();

  // only insert in executed functions under a threshold
  if(!EntryCount)
    return false;
  return EntryCount->getCount() < InjectThreashold;
}

bool IsInjectCandidateInst(Instruction &I) {
  // TODO retur true if it is a BinaryOperator, ReturnInst or a CallInst with an integer operand
  return false;
}

void DoInject(Instruction &I, Function* Detect) {
  auto operand = I.op_begin();
  while(!operand->get()->getType()->isIntegerTy())
    operand++;

  Use* O = operand;
  IRBuilder<> B(&I);

  // TODO Call the check and corrupt one of the operands if the check is positive
  // TODO a way to easily corrupt an operand can be to add the result of the check:
  // if it succeeds it should be 0, if not adding the return code will change the
  // semantic.
}

}

void InsertStartupCheck(Function *DetectPTrace) {

  // TODO create a new function with type void(), if the test fails, call exit(1).
  // To create an if/else structure as a function body, you want to create 3 BasicBlocks (aka BB):
  //   - one for the condition
  //   - one for the success branch
  //   - one for the failed branch
  //   to create a StoreInst with 0x0, you can use the helpers in the builder to get
  //   a constant int, remember that on your machine the pointer type size is
  //   probably 64 bits :)
  //   Don't forget to create a return inst in both the success and failure basicblocks!
  // /!\ Everytime you want to add instructions to a BB, you need to create a builder for that BB, or change the insert point with SetInsertPoint!
  // TODO add the newly created function to the startup functions using llvm::appendToGlobalCtors with a priority of 500
}

void InsertInjectChecks(Function* DetectReadlink) {
  // TODO Find an inject candidate and do the injection
  for(Function &F : *DetectReadlink->getParent()) {
    if(&F == DetectReadlink)
      continue;

    if(!IsInjectCandidateFun(F))
      continue;

  }
}
