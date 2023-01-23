// RUN: %clang -O2 -S -emit-llvm %s -o %t.ll
// RUN: %opt -load-pass-plugin=%lib_mba -passes=mba -S %t.ll -o %t.obf.ll
// RUN: %opt -load-pass-plugin=%lib_z3 -passes=ir2z3 %t.obf.ll 1>/dev/null 2>%t.out
// RUN: %filecheck %s < %t.out
//
// CHECK: model
// CHECK: param_1
// CHECK: 09
// CHECK: param_0
// CHECK: 01

#include <stdio.h>

int __attribute__((section("solve.eq.10"))) add(int a, int b) {
  return a+b;
}

int main() {
  for(int i = 0; i != 256; ++i) {
    for(int j = 0; j != 256; ++j) {
      printf("%d + %d = %d\n", i, j, add(i, j));
    }
  }
  return 0;
}
