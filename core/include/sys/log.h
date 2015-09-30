#ifndef _SYS_LOG_H
#define _SYS_LOG_H

void _log_init();
void _log_putch(char ch);
void _log_print(const char *str);
void _log_puts(const char *str);
void _log_printf(const char *format,...);

#endif //_SYS_LOG_H
