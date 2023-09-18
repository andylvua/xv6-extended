.text
.globl _start

_start:
    xorl %ebp, %ebp

	popl %esi
	movl %esp, %ecx

	// We don't need to align the stack to 16 bytes,
	// since SSE is disabled by -mgeneral-regs-only.
	pushl %ecx
	pushl %esi

	pushl $main

	call __libc_start_main
	// exit() doesn't accept a return value, drop it.
    call exit
