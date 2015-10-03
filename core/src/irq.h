#ifndef _IRQ_H
#define _IRQ_H

struct irq_handler {
	struct irq_handler *next;
	void (*proc)(struct irq_handler*);
};

void irq_init();
void irq_attach(unsigned irq, struct irq_handler*);

#endif //_IRQ_H
