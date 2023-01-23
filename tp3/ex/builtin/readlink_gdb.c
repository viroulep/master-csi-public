#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

int detect_readlink() {
  pid_t ppid = getppid();

  char dbg_path[128];
  char path[32];

  sprintf(path, "/proc/%d/exe", ppid);
  readlink(path, dbg_path, sizeof(dbg_path));

  if(strstr(dbg_path, "gdb") ||strstr(dbg_path, "lldb")) {
    return 1;
  } 
  return 0;
}


