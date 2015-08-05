#include <linux/unistd.h>
#include <sys/types.h>

void mysend(pid_t pid, unsigned int n, void* buf);

unsigned int myreceive(pid_t pid, unsigned int n, void* buf);
