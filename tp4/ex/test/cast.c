// RUN: %clang -O2 -S -emit-llvm %s -o %t.ll
// RUN: %opt -load-pass-plugin=%lib_mba -passes=mba -S %t.ll -o %t.obf.ll
// RUN: %opt -load-pass-plugin=%lib_z3 -passes=ir2z3 %t.obf.ll 1>/dev/null 2>%t.out
// RUN: %filecheck %s < %t.out
//
// CHECK: model 
// CHECK: define-fun
// FIXME: It's actually hard to come up with a non flacky way to test the output!
// Ideally we would run this program by passing the value found by Z3
// (converted to decimal) as arguments.

#include <stdio.h>
#include <stdint.h>

int  add(int a, int b) {
  return a+b;
}

int __attribute__((section("solve.eq.1"))) main(int argc, char** argv) {
  uint32_t x = atoi(argv[1]);
  uint32_t y = atoi(argv[2]);
  return (uint8_t)((x+y)>>31); 
}
