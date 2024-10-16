/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "lwip/api.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
osThreadId_t tcp_task_handle;
const osThreadAttr_t tcp_echo_attributes = {
  .name = "TCP_ECHO_TASK",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void tcp_echo_task (void *argument);

extern void MX_LWIP_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void)
{

}

__weak unsigned long getRunTimeCounterValue(void)
{
return 0;
}
/* USER CODE END 1 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* init code for LWIP */
  MX_LWIP_Init();
  /* USER CODE BEGIN StartDefaultTask */

  osThreadNew(tcp_echo_task, NULL, &tcp_echo_attributes);
  /* Infinite loop */
  for(;;)
  {
	printf("Running...\r\n");
    osDelay(5000);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void tcp_echo_task (void *argument)
{
	struct netconn *conn, *newconn;
	err_t err, accept_err;
	struct netbuf *buf;
	void *data;
	uint16_t len;
	//err_t recv_err;

	LWIP_UNUSED_ARG(argument);

	conn = netconn_new(NETCONN_TCP);
	if (conn != NULL) {
		printf("netconn created...\r\n");
		err = netconn_bind(conn, NULL, 7);
		if (err == ERR_OK) {
			printf("netconn is bound to port 7...\r\n");
			netconn_listen(conn);

			// Indefinite waiting
			while (1) {
				printf("Listening for connections...\r\n");

				accept_err = netconn_accept(conn, &newconn);

				if (accept_err == ERR_OK) {

					printf("Connection accepted...\r\n");
					while (netconn_recv(newconn, &buf) == ERR_OK) {

						do {
							netbuf_data(buf, &data, &len);
							netconn_write(newconn, data, len, NETCONN_COPY);
						} while (netbuf_next(buf) >= 0);

						netbuf_delete(buf);
					}

					/* this will be hit once connection is closed, prev err is ERR_CLSD */
					netconn_close(newconn);
					netconn_delete(newconn);
					printf("Client disconnected.\r\n");
				}
			}
		} else {
			netconn_delete(newconn);
			printf(" can not bind TCP netconn\r\n");
		}
	} else {
		printf("can not create TCP netconn.\r\n");
	}
}
/* USER CODE END Application */

