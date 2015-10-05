#ifndef _IRQ_H
#define _IRQ_H

#include <signal.h>

void _irq_init();
void irq_listen(unsigned irq, struct signal_action*);

#endif //_IRQ_H
