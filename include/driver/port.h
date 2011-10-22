#ifndef PORT_H_
#define PORT_H_

#include <defs.h>
#include <asm/libasm.h>

#define SERIAL_PORT_COM1 0x3f8   /* SERIAL COM1 */

PUBLIC void port_serial_init();
PUBLIC char port_serial_read();
PUBLIC void port_serial_write(char c);

PUBLIC void port_parallel_write(char c);
#endif /* PORT_H_ */
