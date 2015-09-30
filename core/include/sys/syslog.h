#ifndef _SYS_SYSLOG_H
#define _SYS_SYSLOG_H

void syslog_init();
void syslog_putch(char ch);
void syslog_print(const char *str);
void syslog_puts(const char *str);
void syslog_printf(const char *format,...);

#endif //_SYS_SYSLOG_H
