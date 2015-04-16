#include <stdio.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <linux/serial.h>
#include <asm-generic/ioctls.h> /* TIOCGRS485 + TIOCSRS485 ioctl definitions */
#include <sys/ioctl.h>
#include <unistd.h>

int main(void) {
         struct serial_rs485 rs485conf;
 
         int fd = open ("/dev/ttyS0", O_RDWR);
         if (fd < 0) {
                 printf("Error: Can't open: /dev/ttyS0\n");
         }
 
 
         /* Don't forget to read first the current state of the RS-485 options with ioctl.
            If You don't do this, You will destroy the rs485conf.delay_rts_last_char_tx
            parameter which is automatically calculated by the driver when You opens the
            port device. */
         if (ioctl (fd, TIOCGRS485, &rs485conf) < 0) {
                 printf("Error: TIOCGRS485 ioctl not supported.\n");
         }
 
         /* Enable RS-485 mode: */
         rs485conf.flags |= SER_RS485_ENABLED;
 
         /* Set rts/txen delay before send, if needed: (in microseconds) */
         rs485conf.delay_rts_before_send = 0;
 
         /* Set rts/txen delay after send, if needed: (in microseconds) */
         rs485conf.delay_rts_after_send = 0;
 
         if (ioctl (fd, TIOCSRS485, &rs485conf) < 0) {
                 printf("Error: TIOCSRS485 ioctl not supported.\n");
         }
 
         fcntl(fd, F_SETFL, 0);
	 static const int size=256;
	 unsigned char buf[size];

	 while(true){
	   	int nBytesRed = read(fd, buf, size);
		printf(".");
		if (nBytesRed < 0) {
		      /* Error handling */
		}else{
		  if(nBytesRed>0){
			printf("<ReadBuffer %i bytes: ", nBytesRed);
			for(int i=0; i < nBytesRed; i++)
			{
				//printf("|%04X:%02X", tick, buf[i]);
				printf("%02X", buf[i]);
			}
		  }
		}
	 }
	 

 
         if (close (fd) < 0) {
                 printf("Error: Can't close: /dev/ttyS0\n");
         }
}