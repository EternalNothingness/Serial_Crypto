/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <sys/printk.h>
#include "/home/eternaldarkness/zephyrproject/serial_crypto/build/zephyr/include/generated/syscalls/uart.h"
#define MY_STACK_SIZE 1024
#define MY_PRIORITY 5
#define MY_SERIAL DI_NODELABEL(uart0);

const struct device *uart_dev = device_get_binding(DT_LABEL(MY_SERIAL);

void my_entry_point(void *, void *, void *);
K_THREAD_STACK_DEFINE(my_stack_area, MY_STACK_SIZE);
struct k_thread my_thread_data;
struct data_item_type{
	uint8_t field1;
};

char __aligned(4) my_msgq_buffer[10*sizeof(struct data_item_type)];
struct k_msgq my_msgq;

void main(void){
	// message queue initalisieren
	k_msgq_init(&my_msgq, my_msgq_buffer, sizeof(struct data_item_type), 10);
	// Threads starten
	k_tid_t my_tid = k_thread_create(&my_thread_data, my_stack_area,
		K_THREAD_STACK_SIZEOF(my_stack_area),
		my_entry_point,
		NULL, NULL, NULL,
 		MY_PRIORITY, 0, K_NO_WAIT);
	//k_thread_name_set(NULL, "main");
	printk("main1: Priority of main thread is: %i\n",k_thread_priority_get(k_current_get()));
	k_thread_priority_set(k_current_get(), -2);
	printk("main2: Priority of main thread is: %i\n",k_thread_priority_get(k_current_get()));
	printk("main3: Priority of secondary thread is: %i\n",k_thread_priority_get(my_tid));
	k_thread_priority_set(my_tid, -3);
	//k_yield();
	printk("main4: Back in main\n");
	
	struct data_item_type data;
	data.field1 = 0x00;
	for(;;){
		printk("Currently in main thread\n");
		k_msgq_put(&my_msgq, &data, K_NO_WAIT);
		printk("Send %x\n",data.field1);
		data.field1++;
		k_msleep(100);
	}
}

void my_entry_point(void *ptr1, void *ptr2, void *ptr3){
	ARG_UNUSED(ptr1);
	ARG_UNUSED(ptr2);
	ARG_UNUSED(ptr3);
	printk("sec1: Priority of secondary thread is : %i\n", k_thread_priority_get(k_current_get()));
	k_thread_priority_set(k_current_get(), -1);
	//printk("sec2: Priority of secondary thread is : %i\n", k_thread_priority_get(k_current_get()));
	//k_yield();
	struct data_item_type data;
	data.field1 = 0x00;
	for(;;){
		printk("Currently in secondary thread\n");
		k_msgq_get(&my_msgq, &data, K_FOREVER);
		printk("Received %x\n",data.field1);
		k_msleep(100);
	}
}
