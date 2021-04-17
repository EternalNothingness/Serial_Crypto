 /*
 * Titel: Krypto-Prozessor
 * Beschreibung: Entschluesselt die mithilfe der UART0-Schnittstelle empfangenen Daten und sendet diese zurueck
 * Link zum GitHub-Repository: https://github.com/EternalNothingness/Serial_Crypto
 * Datum der letzten Bearbeitung: 17.04.2021
 * Autor: Patrick Wintner
 */

#include <stdlib.h>
#include <device.h>
#include <zephyr.h>
#include <sys/printk.h>
#include <drivers/uart.h>
#include <crypto/cipher.h>
#include <crypto/cipher_structs.h>
#include <string.h>
#include <stdbool.h>

// -- defines --
// defines for threads
#define THREAD_STACK_SIZE 1024
#define THREAD_PRIORITY 0

// defines for uart
#define UART_DEVICE DT_NODELABEL(uart0)

// defines for crypto
#ifdef CONFIG_CRYPTO_TINYCRYPT_SHIM
#define CRYPTO_DRV_NAME CONFIG_CRYPTO_TINYCRYPT_SHIM_DRV_NAME
#elif CONFIG_CRYPTO_MBEDTLS_SHIM
#define CRYPTO_DRV_NAME CONFIG_CRYPTO_MBEDTLS_SHIM_DRV_NAME
#elif CONFIG_CRYPTO_STM32
#define CRYPTO_DRV_NAME DT_LABEL(DT_INST(0, st_stm32_cryp))
#elif CONFIG_CRYPTO_NRF_ECB
#define CRYPTO_DRV_NAME DT_LABEL(DT_INST(0, nordic_nrf_ecb))
#else
#error "You need to enable one crypto device"
#endif

// defines for message queuues
#define DATA_SIZE_MAX 255
/*
Die Laenge von zu entschluesselnden Ciphertexten wird im ersten empfangenen Byte angegeben ->
dementsprechend kann eine Nachricht nie laenger 2^8-1=255 Bits sein
*/

// ## global ##
// -- peripherals --
// uart
const struct device *uart_dev;
struct uart_config uartconf;

// crypto
const struct device *crypto_dev;
uint32_t cap_flags;
int validate_hw_compatibility(void);
void cbc_mode(uint8_t *key, uint8_t *iv, uint8_t *ciphertext, uint8_t **plaintext);
char *decrypt_data(void);

// -- state machine --
// states
enum {st_init, st_avail, st_decrypt, st_data, st_op_decrypt};
int state = st_init;

// Operands
enum{op_decrypt, op_key, op_iv};
int op;

// -- threads --
void uart_in(void *, void *, void *);
void uart_out(void*, void*, void*);
void processing(void*, void*, void*);

K_THREAD_DEFINE(uart_in_tid, THREAD_STACK_SIZE,
		uart_in, NULL, NULL, NULL,
		THREAD_PRIORITY, 0, 0);
K_THREAD_DEFINE(uart_out_tid, THREAD_STACK_SIZE,
		uart_out, NULL, NULL, NULL,
		THREAD_PRIORITY, 0, 0);
K_THREAD_DEFINE(processing_tid, THREAD_STACK_SIZE,
		processing, NULL, NULL, NULL,
		THREAD_PRIORITY, 0, 0);

// -- message queues --
K_MSGQ_DEFINE(uart_msgq, DATA_SIZE_MAX*sizeof(uint8_t), 10, 1);
K_MSGQ_DEFINE(processing_msgq, DATA_SIZE_MAX*sizeof(uint8_t), 1, 1);

// ## main ##
void main(void){
	// ## setup area ##
	// -- Peripherie --
	// UART
	uart_dev = device_get_binding(DT_LABEL(UART_DEVICE));
	if(!uart_dev){
		printk("UART not found\n");
		return;
	}
	printk("UART found\n");

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

	// Crypto
	crypto_dev = device_get_binding(CRYPTO_DRV_NAME);
	if(validate_hw_compatibility()!=0){
		printk("Crypto device not compatible\n");
		return;
	}
	printk("Crypto device is compatible\n");

	// ## main loop ##
	for(;;){
		printk("\nmain is waiting for death\n"); // "main is alive" ist mir irgendwie zu langweilig...
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

	uint8_t input;
	uint8_t *data = malloc(DATA_SIZE_MAX*sizeof(uint8_t));
	uint8_t len = 0;

	// ## main loop ##
	for(;;){
		// state machine
		switch(state){
			case st_init:
				// Warten auf Daten
				if(!uart_poll_in(uart_dev, &input)){
					printk("received command: %c\n", (char)input);
					switch(input){
						case '.':
							// Punkt zuruecksenden
							printk("Sending '.' back\n\n");
							k_msgq_put(&uart_msgq, ".\n", K_FOREVER);
							break;
						case 'P':
							// Processing available
							printk("Changing state to st_avail\n\n");
							state = st_avail;
							break;
						case 'D':
							// Decrypt
							printk("Changing state to st_decrypt\n\n");
							state = st_decrypt;
							break;
						default: break;
					}
				}
				break;
			case st_avail: break; // do nothing
			case st_decrypt:
				// set op to op_decrypt, receive len of incoming message and
				// change state to st_data
				if(!uart_poll_in(uart_dev, &input)){
					op = op_decrypt;
					len = input;
					printk("Length of incoming ciphertext is %i\n", len);
					printk("Changing state to st_data\n\n");
					state = st_data;
				}
				break;
			case st_data:
				// receive data
				for(uint8_t i=0; len!=0; len--){
					if(!uart_poll_in(uart_dev, &input)){
						if(len==-1) break;
						printk("received data: <0x%x>\n", input);
						*(data+i) = input;
						if(i>DATA_SIZE_MAX){
							printk("Error: received message too large!\n");
							printk("Changing state to st_init\n\n");
							state = st_init;
						}
						i++;
					}
					else{
						len++;
						// Keine Daten angekommen ->
						// auto-decrement muss kompensiert werden
					}
					k_usleep(1); //  Prozessor abgeben
				}
				for(;;){
					if(!uart_poll_in(uart_dev, &input)){
					// neccessary to remove the additional received byte
						switch(op){
							// next state depends on operand
							case op_decrypt:
								printk("Sending data <%s> to processing thread\n", data);
								k_msgq_put(&processing_msgq, data, K_FOREVER);
								printk("Changing state to st_op_decrypt\n\n");
								state = st_op_decrypt;
								break;
							default:
								printk("Error: unknwown operand!\n");
								printk("Changing state to st_init\n\n");
								state = st_init;
								break;
						}
						break;
					}
					k_usleep(1); // Prozessor abgeben
				}
				break;
			case st_op_decrypt: break; // do nothing
			default: break;
		}
		k_usleep(1); // Prozessor abgeben
	}

	return; // sollte nie geschehen
}

void uart_out(void *ptr1, void *ptr2, void *ptr3){
	// ## setup area ##
	ARG_UNUSED(ptr1);
	ARG_UNUSED(ptr2);
	ARG_UNUSED(ptr3);

	char *output=malloc(DATA_SIZE_MAX*sizeof(char));

	// ## main loop ##
	for(;;){
		memset(output, 0, strlen(output)); // Inhalt resetten
		if(k_msgq_get(&uart_msgq, output, K_NO_WAIT)==0){ // Abfrage der uart message queue
			printk("Going to send data to the uart: <%s>\n", output);
			for(int i=0; i<strlen(output); i++){ // Send data byte by byte
				uart_poll_out(uart_dev, *(output+i));
				printk("Sent byte: <0x%x>\n", *(output+i));
				k_usleep(1); // Abgeben des Prozessors
			}
			printk("Finished transmission\n\n");
		}
		k_usleep(1); // Abgeben des Prozessors
	}
	return;
}

void processing(void *ptr1, void *ptr2, void *ptr3){
	// ## setup area ##
	ARG_UNUSED(ptr1);
	ARG_UNUSED(ptr2);
	ARG_UNUSED(ptr3);

	uint8_t *plaintext=malloc(DATA_SIZE_MAX*sizeof(char));
	uint8_t *ciphertext=malloc(DATA_SIZE_MAX*sizeof(char));
	uint8_t *key = "BBBBBBBBBBBBBBBB";
	uint8_t *iv = "BBBBBBBBBBBBBBBB";

	// ## main loop ##
	for(;;){
		// state machine
		switch(state){
			case st_init: break;
			case st_avail:
				// Senden von "PROCESSING AVAILABLE" an den Benutzer
				k_msgq_put(&uart_msgq, "PROCESSING AVAILABLE\n", K_FOREVER);
				printk("Changing state to st_init\n\n");
				state = st_init; // Nach erfolgreichen Senden Rueckkehr in init state
				break;
			case st_decrypt: break; // do nothing
			case st_data: break; // do nothing
			case st_op_decrypt:
				// decrypt the received ciphertext,
				// send the plaintext to uart_out and
				// return to st_init
				if(k_msgq_get(&processing_msgq, (void*)ciphertext, K_NO_WAIT)==0){
					printk("Going to decrypt ciphertext: <%s>\n", ciphertext);
					//plaintext = decrypt_data();
					cbc_mode(key, iv, ciphertext, &plaintext);
					printk("Sending plaintext <%s> to uart_out\n", plaintext);
					k_msgq_put(&uart_msgq, plaintext, K_FOREVER);
					printk("Changing state to st_init\n\n");
					state = st_init;
				}
				break;
			default: break;
		}
		k_usleep(1); // Abgeben des Prozessors
	}
	return;
}

char *decrypt_data(void){
	// ## setup area ##
	char *plaintext=malloc(DATA_SIZE_MAX);
	plaintext = "XERROR\n";

	// ## main loop ##
	return plaintext;
}

int validate_hw_compatibility(void)
// ubernommen aus Krypto-Sample von Zephyr
{
        uint32_t flags = 0U;

        flags = cipher_query_hwcaps(crypto_dev);
        if ((flags & CAP_RAW_KEY) == 0U) {
                return -1;
        }

        if ((flags & CAP_SYNC_OPS) == 0U) {
                return -1;
        }

        if ((flags & CAP_SEPARATE_IO_BUFS) == 0U) {
                return -1;
        }

        cap_flags = CAP_RAW_KEY | CAP_SYNC_OPS | CAP_SEPARATE_IO_BUFS;

        return 0;
}

void cbc_mode(uint8_t *key, uint8_t *iv, uint8_t *ciphertext, uint8_t **plaintext)
// uebernommen aus Krypto-Sample von Zephyr und von Mitterhuber
{
        struct cipher_ctx ini = {
                .keylen = sizeof(key),
                .key.bit_stream = key,
                .flags = cap_flags,
        };

        struct cipher_pkt decrypt = {
                .in_buf = ciphertext,
                .in_len = sizeof(ciphertext),
                .out_buf = *plaintext,
                .out_buf_max = sizeof(*plaintext),
        };

        if (cipher_begin_session(crypto_dev, &ini, CRYPTO_CIPHER_ALGO_AES,
                                 CRYPTO_CIPHER_MODE_CBC,
                                 CRYPTO_CIPHER_OP_DECRYPT)){
		*plaintext = "XERROR\n";
                return;
        }

        if (cipher_cbc_op(&ini, &decrypt, ciphertext)) {
                printk("CBC mode DECRYPT - Failed\n");
		*plaintext = "XERROR\n";
                goto out;
        }

       printk("Output length (decryption): %d\n", decrypt.out_len);
out:
        cipher_free_session(crypto_dev, &ini);
}
