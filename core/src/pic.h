#ifndef _PIC_H
#define _PIC_H

extern void _pic_init();
extern void _pic_disable();
extern void _pic_enable();
extern void _pic_eoi_master();
extern void _pic_eoi_slave();

#endif //_PIC_H
