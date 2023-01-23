// RUN: %clang %s -S -emit-llvm -o %t.ll
// RUN: %opt %t.ll -S -load %lib_antidebug -load-pass-plugin=%lib_antidebug -passes=anti-debug -startup=false -inject=true -o %t.inject.ll
// RUN: %filecheck %s < %t.inject.ll
//
// RUN: %clang -O2 %t.inject.ll -o %t
// RUN: %t
// RUN: ! gdb --return-child-result --eval-command="run" --eval-command="q" %t
//
// CHECK-NOT: global_ctors 
// CHECK: detect_readlink 

#include <stdio.h>

int main() {
  printf("on main!\n");
  return 0;
}
