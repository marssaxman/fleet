#ifndef _STARTC_CPUSTATE_H
#define _STARTC_CPUSTATE_H

struct _cpu_state
{
	unsigned edi, esi, ebp, esp;
	unsigned ebx, edx, ecx, eax;
	unsigned error, eip, cs, eflags;
};

#endif //_STARTC_CPUSTATE_H

