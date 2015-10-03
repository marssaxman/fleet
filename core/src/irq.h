#ifndef _IRQ_H
#define _IRQ_H

#include "task.h"

void irq_init();
void irq_listen(unsigned irq, struct _task*);

#endif //_IRQ_H
