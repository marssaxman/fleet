#ifndef _IRQ_H
#define _IRQ_H

#include <workqueue.h>

void _irq_init();
void irq_listen(unsigned irq, struct work_item*);

#endif //_IRQ_H
