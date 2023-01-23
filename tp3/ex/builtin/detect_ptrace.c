#include <sys/ptrace.h>
#include <stdio.h>

int detect_ptrace() {
  if(ptrace(PTRACE_TRACEME, 0, 0, 0) == -1) {
    return 1;
  }
  // after this we cannot call ptrace no more since the process is ptraceing itself
  return 0;
}

