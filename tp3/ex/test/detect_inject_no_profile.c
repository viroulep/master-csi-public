// RUN: %clang %s -S -emit-llvm -o %t.ll
// RUN: %opt %t.ll -S -load %lib_antidebug -load-pass-plugin=%lib_antidebug -passes=anti-debug -startup=false -inject=true -inject-threshold=1 -o %t.inject.ll
// RUN: %filecheck %s < %t.inject.ll
//
// CHECK-NOT: global_ctors 
// CHECK-NOT: call{{.*}}detect_readlink 

#include <stdio.h>

int main() {
  printf("on main!\n");
  return 0;
}
