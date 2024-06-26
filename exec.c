#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"
#include "elf.h"

int
exec(char *path, char **argv, char **envp)
{
  char *s, *last;
  int i, off;
  uint argc, envc, sz, sp, ustack[3 + MAXARG + MAXENV + 2];
  struct elfhdr elf;
  struct inode *ip;
  struct proghdr ph;
  pde_t *pgdir, *oldpgdir;
  struct proc *curproc = myproc();

  begin_op();

  if((ip = namei(path)) == 0){
    end_op();
    return -1;
  }
  ilock(ip);
  pgdir = 0;

  // Check ELF header
  if(readi(ip, (char*)&elf, 0, sizeof(elf)) != sizeof(elf))
    goto bad;
  if(elf.magic != ELF_MAGIC)
    goto bad;

  if((pgdir = setupkvm()) == 0)
    goto bad;

  // Load program into memory.
  sz = 0;
  for(i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)){
    if(readi(ip, (char*)&ph, off, sizeof(ph)) != sizeof(ph))
      goto bad;
    if(ph.type != ELF_PROG_LOAD)
      continue;
    if(ph.memsz < ph.filesz)
      goto bad;
    if(ph.vaddr + ph.memsz < ph.vaddr)
      goto bad;
    if((sz = allocuvm(pgdir, sz, ph.vaddr + ph.memsz)) == 0)
      goto bad;
    if(ph.vaddr % PGSIZE != 0)
      goto bad;
    if(loaduvm(pgdir, (char*)ph.vaddr, ip, ph.off, ph.filesz) < 0)
      goto bad;
  }
  iunlockput(ip);
  end_op();
  ip = 0;

  // Allocate two pages at the next page boundary.
  // Make the first inaccessible.  Use the second as the user stack.
  sz = PGROUNDUP(sz);
  if((sz = allocuvm(pgdir, sz, sz + 2*PGSIZE)) == 0)
    goto bad;
  clearpteu(pgdir, (char*)(sz - 2*PGSIZE));
  sp = sz;

  /*
   * We need to push the arguments and environment variables onto the stack in
   * the following order:
   *
   * sp     :    argc
   * argv   :    argv[0]
   *             argv[1]
   *             ...
   *             NULL
   * envp   :    envp[0]
   *             envp[1]
   *             ...
   *             NULL
   */

  // Push argument strings.
  for(argc = 0; argv[argc]; argc++) {
    if(argc >= MAXARG)
      goto bad;
    sp = (sp - (strlen(argv[argc]) + 1)) & ~3;
    if(copyout(pgdir, sp, argv[argc], strlen(argv[argc]) + 1) < 0)
      goto bad;
    ustack[1 + argc] = sp;
  }
  ustack[1 + argc] = 0;  // Null-terminate the argv array

//  ustack[0] = 0xffffffff;  // No more needed due to updated call convention
  // Push argc.
  ustack[0] = argc;

  // Push environment strings.
  for (envc = 0; envp[envc]; envc++) {
    if (envc >= MAXENV)
      goto bad;
    sp = (sp - (strlen(envp[envc]) + 1)) & ~3;
    if(copyout(pgdir, sp, envp[envc], strlen(envp[envc]) + 1) < 0)
      goto bad;
    ustack[argc + 2 + envc] = sp;  // Offset by argc + 1 for argv
  }
  ustack[argc + 2 + envc] = 0;  // Null-terminate the envp array

  sp -= (3 + argc + envc) * 4;
  if (copyout(pgdir, sp, ustack, (3 + argc + envc) * 4) < 0)
    goto bad;

  // Save program name for debugging.
  for(last=s=path; *s; s++)
    if(*s == '/')
      last = s+1;
  safestrcpy(curproc->name, last, sizeof(curproc->name));

  // Commit to the user image.
  oldpgdir = curproc->pgdir;
  curproc->pgdir = pgdir;
  curproc->sz = sz;
  curproc->tf->eip = elf.entry;  // main
  curproc->tf->esp = sp;
  switchuvm(curproc);
  freevm(oldpgdir);
  return 0;

 bad:
  if(pgdir)
    freevm(pgdir);
  if(ip){
    iunlockput(ip);
    end_op();
  }
  return -1;
}
