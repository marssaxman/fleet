#ifndef _STARTC_STARTC_H
#define _STARTC_STARTC_H

struct multiboot_info;

// The application should define this entrypoint function.
extern void _startc(unsigned magic, struct multiboot_info *info);

#endif //_STARTC_STARTC_H
