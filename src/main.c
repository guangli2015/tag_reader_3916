/* main.c - Application main entry point */

/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/types.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <st25r3911b_nfca.h>
#include <nfc/ndef/msg_parser.h>
#include <nfc/ndef/le_oob_rec_parser.h>
#include <nfc/t2t/parser.h>
#include <nfc/t4t/ndef_file.h>
#include <nfc/t4t/isodep.h>
#include <nfc/t4t/hl_procedure.h>
#include <nfc/ndef/ch_rec_parser.h>
#include <zephyr/sys/byteorder.h>
#include <st25r3916_nfca.h>
#include "st25r3916_irq.h"

#define MY_STACK_SIZE 1024
#define MY_PRIORITY -2
static K_SEM_DEFINE(irq_sem, 0, 1);
int is=0;
void my_entry_point(int unused1, int unused2, int unused3)
{
	int err;
	while(1){
		err = k_sem_take(&irq_sem, K_FOREVER);
		if (err) {
			return;
		}
		st25r3916Isr();
	}

}

K_THREAD_STACK_DEFINE(my_stack_area, MY_STACK_SIZE);

void main(void)
{
	int err;

	printk("Starting NFC TAG Reader example\n");


	st25r3916InitInterrupts(&irq_sem);
	struct k_thread my_thread_data;
	
	k_tid_t my_tid = k_thread_create(&my_thread_data, my_stack_area,
									 K_THREAD_STACK_SIZEOF(my_stack_area),
									 my_entry_point,
									 NULL, NULL, NULL,
									 MY_PRIORITY, 0, K_NO_WAIT);

	err = st25r3916_nfca_init();
	if (err) {
		return err;
	}
}
