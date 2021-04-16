 /*
 * Titel: Krypto-Prozessor
 * Beschreibung: Entschluesselt die mithilfe der UART0-Schnittstelle empfangenen Daten und sendet diese zurueck
 * Link zum GitHub-Repository: https://github.com/EternalNothingness/Serial_Crypto
 * Datum der letzten Bearbeitung: 13.04.2021
 * Autor: Patrick Wintner
 */

#include <stdlib.h>
#include <device.h>
#include <zephyr.h>
#include <sys/printk.h>
#include <drivers/uart.h>
#include <string.h>

// defines for threads
#define THREAD_STACK_SIZE 1024
#define THREAD_PRIORITY 0

// defines for uart
#define UART_DEVICE DT_NODELABEL(uart0)

// uart
const struct device *uart_dev;
struct uart_config uartconf;

// state machine
enum {init, avail};
int state = init;

void uart_in(void *, void *, void *);
void uart_out(void*, void*, void*);
void processing(void*, void*, void*);

// threads
K_THREAD_DEFINE(uart_in_tid, THREAD_STACK_SIZE,
		uart_in, NULL, NULL, NULL,
		THREAD_PRIORITY, 0, 0);
K_THREAD_DEFINE(uart_out_tid, THREAD_STACK_SIZE,
		uart_out, NULL, NULL, NULL,
		THREAD_PRIORITY, 0, 0);
K_THREAD_DEFINE(processing_tid, THREAD_STACK_SIZE,
		processing, NULL, NULL, NULL,
		THREAD_PRIORITY, 0, 0);

// message queues
K_MSGQ_DEFINE(uart_msgq, 100*sizeof(char), 10, 1);
K_MSGQ_DEFINE(processing_msgq, 100*sizeof(char), 10, 1);

void main(void){
	// ## setup area ##
	// -- Peripherie --
	uart_dev = device_get_binding(DT_LABEL(UART_DEVICE));
	if(!uart_dev){
		printk("UART not found\n");
		return;
	}
	printk("UART found\n");

	// -- UART konfigurieren --
	uartconf.baudrate = 9600;
	uartconf.parity = UART_CFG_PARITY_NONE;
	uartconf.stop_bits = UART_CFG_STOP_BITS_1;
	uartconf.data_bits = UART_CFG_DATA_BITS_8;
	uartconf.flow_ctrl = UART_CFG_FLOW_CTRL_NONE;
	if(!uart_configure(uart_dev, &uartconf)){
		printk("Configuration of UART failed\n");
		return;
	}
	printk("UART configured\n");

	// ## main loop ##
	for(;;){
		printk("\nmain is waiting for death\n"); // "main is still alive" ist mir irgendwie zu langweilig...
		k_msleep(10*1000); // 10s schlafen
	}
}

// ## functions ##

void uart_in(void *ptr1, void *ptr2, void *ptr3){
	// ## setup area ##
	// Uebergabeparameter werden nicht benoetigt
	ARG_UNUSED(ptr1);
	ARG_UNUSED(ptr2);
	ARG_UNUSED(ptr3);

	unsigned char input;

	// ## main loop ##
	for(;;){
		// state machine
		switch(state){
			case init:
				// Warten auf Daten
				if(!uart_poll_in(uart_dev, &input)){
					printk("received data: %c\n", input);
					switch(input){
						case '.':
							// Punkt zuruecksenden
							k_msgq_put(&uart_msgq, ".\n", K_FOREVER);
							break;
						case 'P':
							printk("Changing state to avail\n");
							state = avail;
							break;
						default: break;
					}
				}
			case avail: break;
			default: break;
		}
		k_msleep(1); // Prozessor abgeben
	}

	return; // sollte nie geschehen
}

void uart_out(void *ptr1, void *ptr2, void *ptr3){
	// ## setup area ##
	ARG_UNUSED(ptr1);
	ARG_UNUSED(ptr2);
	ARG_UNUSED(ptr3);

	unsigned char *output=malloc(100*sizeof(char));

	// ## main loop ##
	for(;;){
		memset(output, 0, strlen(output)); // Inhalt resetten
		if(k_msgq_get(&uart_msgq, output, K_NO_WAIT)==0){ // Abfrage der uart message queue
			printk("Going to send data: <%s>\n", output);
			for(int i=0; i<strlen(output); i++){
				uart_poll_out(uart_dev, *(output+i));
				printk("Sent data: <%x>\n", *(output+i));
			}
		}
		k_msleep(1); // Abgeben des Prozessors
	}
	return;
}

void processing(void *ptr1, void *ptr2, void *ptr3){
	// ## setup area ##
	ARG_UNUSED(ptr1);
	ARG_UNUSED(ptr2);
	ARG_UNUSED(ptr3);

	// ## main loop ##
	for(;;){
		// state machine
		switch(state){
			case init:
				break;
			case avail:
				// Senden von "PROCESSING AVAILABLE" an den Benutzer
				k_msgq_put(&uart_msgq, "PROCESSING AVAILABLE\n", K_FOREVER);
				printk("Changing state to init\n");
				state = init; // Nach erfolgreichen Senden Rueckkehr in init state
				break;
			default: break;
		}
		k_msleep(1); // Abgeben des Prozessors
	}
	return;
}
