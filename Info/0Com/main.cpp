/*
 * main.c 
 *
 *  Created on: 24 May 2012
 *      Author: Xuewu Daniel Dai
 *       Email: xuewu.dai@eng.ox.ac.uk
 *         URL: http://users.ox.ac.uk/~engs1058/
 * %                _
 * %    \/\ /\ /   /  \ _  '
 * %   _/\ \/\/ __/__.'(_|_|_
 */

// Modified from main.c by Jan Axelson
// Original files (main.c, led_control.h, and led_control.c) are available at
// www.Lvr.com/beagleboard.htm

//
// Demonstrates communications on a RS232 serial console port of Beagleboard.
// Tested with a USB/serial-port adapter AirLink that contains an FT232B USB UART.

// The console on ttyO2 serial port of the Beagleboard should be disabled by
// remove or rename the /dev/ttyO2.conf and reboot the beagleboard.

// To turn on the usr0 LED on the BeagleBoard, type led1on and press <Enter>.
// To turn off the usr0 LED on the BeagleBoard, type led1off and press <Enter>.
// Ctrl+C or a "closeapp" command terminates the program.

// To compile the application, copy led_control.h and led_control.c
// to the same folder as main.c.

// To gain access to the LEDs, run the application with administrative privileges
// (From the file's directory, use "sudo ./<executablefilename>

// C:\CodeSourcery\lib\gcc\arm-none-linux-gnueabi\4.6.1\include
// C:\CodeSourcery\lib\gcc\arm-none-linux-gnueabi\4.6.1\include-fixed
// C:\CodeSourcery\arm-none-linux-gnueabi\include
// C:\CodeSourcery\arm-none-linux-gnueabi\libc\usr\include

#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

void respond_to_command(char *read_buffer, int chars_read);
void serial_port_close();
int serial_port_open(void);
int serial_port_read(char *read_buffer, size_t max_chars_to_read);
void serial_port_write(char *write_buffer);
void sigint_handler(int sig);

#define MAX_COMMAND_LENGTH 10

static const char *PORT_NAME = "/dev/ttyO2";  // for default RS232 console at BB

int serial_port;
struct termios options_original;

// Opens the USB serial port and continuously attempts to read from the port.
// On receiving data, looks for a defined command.

int main(void){
	int chars_read;
	char read_buffer[MAX_COMMAND_LENGTH + 1] = {0};

	printf("Start main()...\n");
	serial_port_open();
	return 0;
	printf("    serial_port_open() done\n");

	// The main program loop:

	for (;;){
		if (serial_port != -1){
			// Read received data into the buffer
			// up to but not including the buffer's terminating null.

			chars_read = serial_port_read(read_buffer, MAX_COMMAND_LENGTH);
			printf(">>%d\n",chars_read);
		}
		// The application can perform other tasks here.
	}
	return 0;
}

// Looks for defined commands and takes appropriate action.
// read_buffer - the data to examine.

void respond_to_command(char *read_buffer, int chars_read)
{
	printf("Command %s\n", read_buffer);

}

// Resets the terminal and closes the serial port.

void serial_port_close()
{
	tcsetattr(serial_port,TCSANOW,&options_original);
	close(serial_port);
}

// Opens a USB virtual serial port at ttyUSB0.
//
// returns - the port's file descriptor or -1 on error.

// changing of , using 'canonical' input mode will change received byte '0D' to '0A'
// change your Carriage-Returns to Linefeeds.

int serial_port_open(void){
  struct termios options;

  serial_port = open(PORT_NAME, O_RDWR | O_NONBLOCK);

  if (serial_port != -1){
	  printf("Serial Port open\n");
	  tcgetattr(serial_port,&options_original);
 	  tcgetattr(serial_port, &options);
	  cfsetispeed(&options, B57600);
	  cfsetospeed(&options, B57600);
	  options.c_cflag |= (CLOCAL | CREAD);
	  options.c_lflag |= ICANON;
	  if (tcsetattr(serial_port, TCSANOW, &options)!=0){
		  printf("error %d from tcsetattr", errno);
		  return (-1);
	  }
  }else{
	  printf("Unable to open %s",PORT_NAME);
  	  printf("Error %d opening %s: %s",errno, PORT_NAME, strerror(errno));
  }
  return (serial_port);
}

// Attempts to read up to 10 bytes from the serial port.
// If data was read, calls a routine to examine the received data
// and take action.
// *read_buffer - the buffer that will contain the data read.
// max_chars_to_read - the maximum number of characters to read.
// returns - 0 if data was received, -1 if no data received.

int serial_port_read(char *read_buffer, size_t max_chars_to_read){
	int chars_read = read(serial_port, read_buffer, max_chars_to_read);

	return chars_read;
}

// Writes data to the port.
// Parameter:  write_buffer - the data to write to the port.
// *write_buffer - the buffer that contains the data to write.

void serial_port_write(char *write_buffer){
	int bytes_written;
	size_t len = 0;

	len = strlen(write_buffer);
	bytes_written = write(serial_port, write_buffer, len);
	if (bytes_written < len){
		printf("Write failed \n");
	}
}

// Executes when the user presses Ctrl+C.
// Closes the port, resets the terminal, and exits the program.

void  sigint_handler(int sig)
{
	serial_port_close();
	exit (sig);
}



