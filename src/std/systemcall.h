#ifndef SYSTEMCALL_H
#define	SYSTEMCALL_H

void __read(int fd, void* buffer, int count);
void __write(int fd, const void* buffer, int count);
void __cpuspeed(void * ips);

#endif	/* SYSTEMCALL_H */

