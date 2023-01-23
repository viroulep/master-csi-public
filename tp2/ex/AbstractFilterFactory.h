#ifndef ABSTRACT_FILTER_H
#define ABSTRACT_FILTER_H

#include <functional>

namespace llvm {
  class Value;
};

struct AbstractFilterFactory {
  static AbstractFilterFactory& get();

  using FilterTy = std::function<bool(llvm::Value&)>;
  virtual FilterTy createFilter(llvm::Value& V) = 0;
};

struct NoFilter : public AbstractFilterFactory {
  FilterTy createFilter(llvm::Value& V) override {
    return [](llvm::Value&) { return true; }; // accept all
  }
};

#endif
