.code
	CallSyscall proc
			mov r10, rcx
			mov eax, 55h
			syscall
			ret
	CallSyscall endp
end