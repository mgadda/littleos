#ifndef _IO_H_
#define _IO_H_

/**
 * outb:
 * Sends the given data ot the given I/O port. Defined in io.s
 *
 * @param port The I/O port to send the data to
 * @param data The data to send to the I/O port
 */
void outb(unsigned short port, unsigned char data);

/**
 * inb:
 * Read a bute from an I/O port
 */

unsigned char inb(unsigned short port);

#endif /* _IO_H_ */
