// RUN: %clang %s -O2 -fprofile-instr-generate -o %t.instr
// RUN: rm -fr %t.profile.dir 
//
// fuzz !
// RUN: LLVM_PROFILE_FILE="%t.profile.dir/code-%%p.profraw" %t.instr
// RUN: LLVM_PROFILE_FILE="%t.profile.dir/code-%%p.profraw" %t.instr
// RUN: LLVM_PROFILE_FILE="%t.profile.dir/code-%%p.profraw" %t.instr
//
// RUN: %llvm_tools_dir/llvm-profdata merge -output=%t.profdata %t.profile.dir/code-*.profraw
//
// RUN: %clang %s -fprofile-instr-use=%t.profdata -S -emit-llvm -o %t.ll
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

int unused_function() {
  return 0;
}

int main() {
  printf("on main!\n");
  return 0;
}
