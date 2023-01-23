// RUN: %clang -O2 -S -emit-llvm %s -o %t.ll
// RUN: %opt -load-pass-plugin=%lib_mba -passes=mba -S %t.ll -o %t.obf.ll
// RUN: %opt -load-pass-plugin=%lib_z3 -passes=ir2z3 %t.obf.ll 1>/dev/null 2>%t.out
// RUN: %filecheck %s < %t.out
//
// CHECK: model 
// CHECK: param_0
// CHECK: #x00000002

#include <stdio.h>

int  add(int a, int b) {
  return a+b;
}

int __attribute__((section("solve.eq.0"))) main(int argc, char** argv) {
  if(argc < 2) {
    return -1;
  }
  int n = atoi(argv[1]);
  if(n & 2) {
    printf("das is gut");
    return 0;
  } else {
    printf("bad!");
    return 7;
  }
}
