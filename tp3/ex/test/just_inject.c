// RUN: %clang %s -S -emit-llvm -o %t.ll
// RUN: %opt %t.ll -S -load %lib_antidebug -load-pass-plugin=%lib_antidebug -passes=anti-debug -startup=true -inject=true -o %t.inject.ll
// RUN: %filecheck %s < %t.inject.ll
//
// CHECK: detect_readlink
// CHECK: detect_ptrace
