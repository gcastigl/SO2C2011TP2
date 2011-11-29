#include <driver/port.h>
#include <util/logger.h>

PRIVATE int _serial_received();
PRIVATE int _serial_is_transmit_empty();
PRIVATE int _serial_received() {
    return _port_in(SERIAL_PORT_COM1 + 5) & 1;
}
PRIVATE int _serial_is_transmit_empty() {
    return _port_in(SERIAL_PORT_COM1 + 5) & 0x20;
}

PUBLIC void port_serial_init() {
    _port_out(SERIAL_PORT_COM1 + 1, 0x00); // Disable all interrupts
    _port_out(SERIAL_PORT_COM1 + 3, 0x80); // Enable DLAB (set baud rate divisor)
    _port_out(SERIAL_PORT_COM1 + 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud
    _port_out(SERIAL_PORT_COM1 + 1, 0x00); //                  (hi byte)
    _port_out(SERIAL_PORT_COM1 + 3, 0x03); // 8 bits, no parity, one stop bit
    _port_out(SERIAL_PORT_COM1 + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
    _port_out(SERIAL_PORT_COM1 + 4, 0x0B); // IRQs enabled, RTS/DSR set
    log(L_INFO, "port serial initialized");
}

PUBLIC char port_serial_read() {
    while (_serial_received() == 0);
    return _port_in(SERIAL_PORT_COM1);
}

PUBLIC void port_serial_write(char c) {
    while (_serial_is_transmit_empty() == 0);
    _port_out(SERIAL_PORT_COM1, c);
}

PUBLIC void port_parallel_write(char c) {
    _port_out(0x37a, 0x04|0x08);
    _port_out(0x378, (unsigned char)c);
    _port_out(0x37a, 0x01);
}
