#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <assert.h>
#include <time.h>
#include "syscall.h"

// helper macros
#define _concat(x, y) x ## y
#define concat(x, y) _concat(x, y)
#define _args(n, list) concat(_arg, n) list
#define _arg0(a0, ...) a0
#define _arg1(a0, a1, ...) a1
#define _arg2(a0, a1, a2, ...) a2
#define _arg3(a0, a1, a2, a3, ...) a3
#define _arg4(a0, a1, a2, a3, a4, ...) a4
#define _arg5(a0, a1, a2, a3, a4, a5, ...) a5

// extract an argument from the macro array
#define SYSCALL  _args(0, ARGS_ARRAY)
#define GPR1 _args(1, ARGS_ARRAY)
#define GPR2 _args(2, ARGS_ARRAY)
#define GPR3 _args(3, ARGS_ARRAY)
#define GPR4 _args(4, ARGS_ARRAY)
#define GPRx _args(5, ARGS_ARRAY)

// ISA-depedent definitions
#if defined(__ISA_X86__)
# define ARGS_ARRAY ("int $0x80", "eax", "ebx", "ecx", "edx", "eax")
#elif defined(__ISA_MIPS32__)
# define ARGS_ARRAY ("syscall", "v0", "a0", "a1", "a2", "v0")
#elif defined(__riscv)
#ifdef __riscv_e
# define ARGS_ARRAY ("ecall", "a5", "a0", "a1", "a2", "a0")
#else
# define ARGS_ARRAY ("ecall", "a7", "a0", "a1", "a2", "a0")
#endif
#elif defined(__ISA_AM_NATIVE__)
# define ARGS_ARRAY ("call *0x100000", "rdi", "rsi", "rdx", "rcx", "rax")
#elif defined(__ISA_X86_64__)
# define ARGS_ARRAY ("int $0x80", "rdi", "rsi", "rdx", "rcx", "rax")
#elif defined(__ISA_LOONGARCH32R__)
# define ARGS_ARRAY ("syscall 0", "a7", "a0", "a1", "a2", "a0")
#else
#error _syscall_ is not implemented
#endif

/*
riscv32 下面展开的代码是
intptr_t _syscall_(intptr_t type, intptr_t a0, intptr_t a1, intptr_t a2) {
  register intptr_t _gpr1 asm ("a7") = type;
  register intptr_t _gpr2 asm ("a0") = a0;
  register intptr_t _gpr3 asm ("a1") = a1;
  register intptr_t _gpr4 asm ("a2") = a2;
  register intptr_t ret asm ("a0");
  asm volatile ("ecall" : "=r" (ret) : "r"(_gpr1), "r"(_gpr2), "r"(_gpr3), "r"(_gpr4));
  return ret;
}
*/
intptr_t _syscall_(intptr_t type, intptr_t a0, intptr_t a1, intptr_t a2) {
  register intptr_t _gpr1 asm (GPR1) = type;
  register intptr_t _gpr2 asm (GPR2) = a0;
  register intptr_t _gpr3 asm (GPR3) = a1;
  register intptr_t _gpr4 asm (GPR4) = a2;
  register intptr_t ret asm (GPRx);
  asm volatile (SYSCALL : "=r" (ret) : "r"(_gpr1), "r"(_gpr2), "r"(_gpr3), "r"(_gpr4));
  return ret;
}

void _exit(int status) {
  _syscall_(SYS_exit, status, 0, 0);
  while (1);
}

int _open(const char *path, int flags, mode_t mode) {
  _syscall_(SYS_open, path, flags, mode);
}

/*
man 2 write
DESCRIPTION
       write() writes up to count bytes from the buffer starting at buf to the file referred to by the file descriptor fd.
RETURN VALUE
       On success, the number of bytes written is returned.  On error, -1 is returned, and errno is set to indicate the error.
*/
int _write(int fd, void *buf, size_t count) {
  return _syscall_(SYS_write, fd, buf, count);
}

/*
brk(2)    System Calls Manual   brk(2)

NAME
       brk, sbrk - change data segment size

DESCRIPTION
       brk()  and sbrk() change the location of the program break, which defines the end of the process's data segment (i.e., the
       program break is the first location after the end of the uninitialized data segment).  Increasing the  program  break  has
       the effect of allocating memory to the process; decreasing the break deallocates memory.

       brk()  sets  the  end  of  the  data segment to the value specified by addr, when that value is reasonable, the system has
       enough memory, and the process does not exceed its maximum data size (see setrlimit(2)).
       sbrk() increments the program's data space by increment bytes.  Calling sbrk() with an increment of 0 can be used to  find
       the current location of the program break.

RETURN VALUE
       On success, brk() returns zero.  On error, -1 is returned, and errno is set to ENOMEM.

       On  success,  sbrk() returns the previous program break.  (If the break was increased, then this value is a pointer to the
       start of the newly allocated memory).  On error, (void *) -1 is returned, and errno is set to ENOMEM.
*/

/*
_sbrk()通过记录的方式来对用户程序的program break位置进行管理, 其工作方式如下:

1. program break一开始的位置位于_end
2. 被调用时, 根据记录的program break位置和参数increment, 计算出新program break
3. 通过SYS_brk系统调用来让操作系统设置新program break
4. 若SYS_brk系统调用成功, 该系统调用会返回0, 此时更新之前记录的program break的位置, 并将旧program break的位置作为_sbrk()的返回值返回
5. 若该系统调用失败, _sbrk()会返回-1
 */


/*
DESCRIPTION
       The addresses of these symbols indicate the end of various program segments:

       + etext  This is the first address past the end of the text segment (the program code).
       + edata  This is the first address past the end of the initialized data segment.
       + end    This is the first address past the end of the uninitialized data segment (also known as the BSS segment).
 */
extern char etext, edata, end; /* The symbols must have some type,
                                          or "gcc -Wall" complains */

void *_sbrk(intptr_t increment) {
  static intptr_t program_break = (intptr_t)(&end);
  intptr_t new_program_break = program_break + increment;
  intptr_t ret = _syscall_(SYS_brk, new_program_break, 0, 0);
  if (ret == 0) {
      program_break = new_program_break;
      return program_break;
  }
  else {
    return (void*)-1;
  }
}

int _read(int fd, void *buf, size_t count) {
  return _syscall_(SYS_read, fd, buf, count);
}

int _close(int fd) {
  return _syscall_(SYS_close, fd, 0, 0);
}

off_t _lseek(int fd, off_t offset, int whence) {
  return _syscall_(SYS_lseek, fd, offset, whence);
}

int _gettimeofday(struct timeval *tv, struct timezone *tz) {
  return _syscall_(SYS_gettimeofday, tv, tz, 0);
}

int _execve(const char *fname, char * const argv[], char *const envp[]) {
  _exit(SYS_execve);
  return 0;
}

// Syscalls below are not used in Nanos-lite.
// But to pass linking, they are defined as dummy functions.

int _fstat(int fd, struct stat *buf) {
  return -1;
}

int _stat(const char *fname, struct stat *buf) {
  assert(0);
  return -1;
}

int _kill(int pid, int sig) {
  _exit(-SYS_kill);
  return -1;
}

pid_t _getpid() {
  _exit(-SYS_getpid);
  return 1;
}

pid_t _fork() {
  assert(0);
  return -1;
}

pid_t vfork() {
  assert(0);
  return -1;
}

int _link(const char *d, const char *n) {
  assert(0);
  return -1;
}

int _unlink(const char *n) {
  assert(0);
  return -1;
}

pid_t _wait(int *status) {
  assert(0);
  return -1;
}

clock_t _times(void *buf) {
  assert(0);
  return 0;
}

int pipe(int pipefd[2]) {
  assert(0);
  return -1;
}

int dup(int oldfd) {
  assert(0);
  return -1;
}

int dup2(int oldfd, int newfd) {
  return -1;
}

unsigned int sleep(unsigned int seconds) {
  assert(0);
  return -1;
}

ssize_t readlink(const char *pathname, char *buf, size_t bufsiz) {
  assert(0);
  return -1;
}

int symlink(const char *target, const char *linkpath) {
  assert(0);
  return -1;
}

int ioctl(int fd, unsigned long request, ...) {
  return -1;
}
