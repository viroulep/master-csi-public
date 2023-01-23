#include "AbstractFilterFactory.h"

using namespace llvm;

AbstractFilterFactory& __attribute__((weak)) AbstractFilterFactory::get() {
  static NoFilter _No;
  return _No;
}
