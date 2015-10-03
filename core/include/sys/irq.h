#ifndef _SYS_IRQ_H
#define _SYS_IRQ_H

struct irq_handler {
	struct irq_handler **previous;
	struct irq_handler *next;
	void (*proc)(struct irq_handler*);
};

void irq_attach(unsigned irq, struct irq_handler*);
void irq_detach(struct irq_handler*);

extern const unsigned irq_count;

#endif //_SYS_IRQ_H
