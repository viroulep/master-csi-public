// Compile the reference version
// RUN: %clang -O2 %s -o %t.ref
//
// Compile the obfuscated version by generating the ll, then running mba, then
// generating the binary.
// RUN: %clang -O2 -S -emit-llvm %s -o %t.ll
// RUN: %opt -load-pass-plugin=%lib_mba -passes=mba -S %t.ll -o %t.obf.ll
// RUN: %clang %t.obf.ll -o %t 
//
// RUN: %filecheck %s < %t.obf.ll
//
// Compare the executions
// RUN: %t.ref > %t.ref.out
// RUN: %t > %t.out
// RUN: diff %t.out %t.ref.out
//
//
// two compilation yield the exact same code !
// RUN: %opt -load-pass-plugin=%lib_mba -passes=mba -S %t.ll -o %t.obf2.ll
// RUN: diff %t.obf.ll %t.obf2.ll

#include <stdio.h>

// CHECK: @add
// CHECK: = {{and|xor|or}}
// CHECK: ret
__attribute__((noinline))
int add(int a, int b) {
  return a+b;
}

// CHECK: @main
int main(int argc, char** argv) {
  for(int i = 0; i != 256; ++i) {
    for(int j = 0; j != 256; ++j) {
      printf("%d + %d = %d\n", i, j, add(i, j));
    }
  }
  return 0;
}
