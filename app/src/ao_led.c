/*
 * Copyright (c) 2023 Sebastian Bedin <sebabedin@gmail.com>.
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
 * @author : Sebastian Bedin <sebabedin@gmail.com>
 */

/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "cmsis_os.h"
#include "board.h"
#include "logger.h"
#include "dwt.h"
#include "ao_led.h"

/********************** macros and definitions *******************************/

#define TASK_PERIOD_MS_           (1000u)

#define QUEUE_LEN				  (5u)
#define QUEUE_SIZE_EVEN			  (sizeof (ao_led_even_t))
#define TASK_PRIORITY			  (1u)
#define MS_DELAY				  (1000u)

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

typedef enum
{
  LED_COLOR_NONE,
  LED_COLOR_RED,
  LED_COLOR_GREEN,
  LED_COLOR_BLUE,
  LED_COLOR_WHITE,
  LED_COLOR__N,
} led_color_t;

/********************** external data definition *****************************/

ao_t ao_led = (ao_t) {

	.event_queue_h = NULL,
	.event_queue_len = QUEUE_LEN,
	.event_size = QUEUE_SIZE_EVEN,
	.queue_name = "",

		// Thread
	.task_name = "",
	.thread_h = NULL,
	.priority = TASK_PRIORITY,
	.stack_size = configMINIMAL_STACK_SIZE,

		/* Process */
	.handler = NULL,

};

/********************** internal functions definition ************************/

/********************** external functions definition ************************/

static void _task (void *parameters) {

	ao_t* ao = (ao_t*) parameters;
	ao_led_even_t *msg = (ao_led_even_t *)pvPortMalloc(ao->event_size);

	while (1) {
		if(NULL != msg)
		{
			LOGGER_INFO("Allocated Memory: %d", sizeof(ao_led_even_t));
			if (pdPASS == xQueueReceive(ao->event_queue_h, msg, portMAX_DELAY)) {
				ao->handler ((void*)msg);
				LOGGER_INFO(ao->task_name);
				ao_ui_destroy(ao);
			}
		}
		else
		{
			LOGGER_INFO("Insufficient memory ");
		}
	}
}
void ao_led_init(ao_t* ao, handlerFunc_t handler) {

    if (NULL != ao && NULL != handler) {

        ao->event_queue_h = xQueueCreate(ao->event_queue_len, ao->event_size);
        configASSERT(NULL != ao->event_queue_h);
        vQueueAddToRegistry(ao->event_queue_h, ao->queue_name);

        BaseType_t status = xTaskCreate(_task,
        		ao->task_name,
				ao->stack_size,
				(void*)ao,
				ao->priority,
				&ao->thread_h);

        configASSERT(pdPASS == status);

        ao->handler = handler;

    }
}

void ao_ui_destroy (ao_t* ao) {

	if (NULL != ao) {

		LOGGER_INFO("TASK DELETED");
		vQueueDelete (ao->event_queue_h);

		vTaskDelete (ao->thread_h);

		ao->handler = NULL;


	}

	//return (NULL == ao->event_queue_h && NULL == ao->thread_h);

}




void task_led_handler (void* msg) {

	TickType_t delay = pdMS_TO_TICKS(MS_DELAY);

	ao_led_even_t* event = (ao_led_even_t*) msg;

	HAL_GPIO_WritePin(event->led_port, event->led_pin, event->led_state);
	vTaskDelay(delay);
	HAL_GPIO_WritePin(event->led_port, event->led_pin, !event->led_state);
	vPortFree((void*)msg);
	LOGGER_INFO("MEMORY FREE");


}

/********************** end of file ******************************************/
