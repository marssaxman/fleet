#ifndef IRQ_H
#define IRQ_H

void _irq_init();
void _irq_listen(unsigned irq, void *ref, void(*action)(void*));
void _irq_ignore(unsigned irq);

#endif // IRQ_H
