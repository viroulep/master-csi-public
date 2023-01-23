// RUN: %clang -O2 -S -emit-llvm %s -o %t.ll
// RUN: %opt -load-pass-plugin=%lib_mba -passes=mba -S %t.ll -o %t.obf.ll
// RUN: %opt -load-pass-plugin=%lib_z3 -passes=ir2z3 %t.obf.ll 1>/dev/null 2>%t.out
// RUN: %filecheck %s < %t.out
//
// CHECK: model 
// CHECK: param_0
// CHECK: #x0002
#include <stdint.h>

uint64_t __attribute__((section("solve.eq.0"))) polynome(uint16_t x) {
  return 2*x*x*x*x + 13*x*x*x - 7*x*x - 72*x + 36;
}
