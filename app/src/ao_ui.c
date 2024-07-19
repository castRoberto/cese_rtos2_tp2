/*
 * Copyright (c) 2024 Grupo 2.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @author : Grupo 2
 */

/********************** inclusions *******************************************/


#include "ao_ui.h"
#include "active_object.h"
#include "ao_led.h"

/********************** macros and definitions *******************************/

/********************** internal data declaration ****************************/

ao_t ao_ui = (ao_t) {

	.event_queue_h = NULL,
	.event_queue_len = QUEUE_LEN,
	.event_size = QUEUE_SIZE_EVEN,
	.queue_name = "UI_queue",

		// Thread
	.task_name = "UI_task",
	.thread_h = NULL,
	.priority = TASK_PRIORITY,
	.stack_size = configMINIMAL_STACK_SIZE,

		/* Process */
	.handler = NULL,

};


/********************** internal functions declaration ***********************/
void ao_ui_send_msg(void *argument) {
	op_result_e result = SEND_ERR;

	while (1) {
		ao_led_even_t msg;
		if (pdPASS == xQueueReceive(ao_ui.event_queue_h, &msg, portMAX_DELAY)) {;

			strncpy(ao_led.task_name, msg.task_name, configMAX_TASK_NAME_LEN - 1);
			ao_led.task_name[configMAX_TASK_NAME_LEN - 1] = '\0';
			strncpy(ao_led.queue_name, msg.queue_name, configMAX_TASK_NAME_LEN - 1);
			ao_led.queue_name[configMAX_TASK_NAME_LEN - 1] = '\0';
			ao_led_init(&ao_led, task_led_handler);
			result = (pdPASS == xQueueSend(ao_led.event_queue_h, &msg, 0));

			if (result != SEND_OK) {
				LOGGER_INFO("Failed to send message to LED queue");
			}

		} else {
			LOGGER_INFO("Failed to receive message from queue");
		}
	}
}

/********************** internal data definition *****************************/

/********************** external data definition *****************************/

/********************** internal functions definition ************************/

/********************** external functions definition ************************/




/*
op_result_e ao_ui_send_msg (ao_t* ao, void* msg) {

	op_result_e result = SEND_ERR;

	if (NULL != ao  && NULL != msg) {

		result = (pdPASS == xQueueSend (ao->event_queue_h, msg, 0));

	}

	return result;

}
*/



/********************** end of file ******************************************/
