/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    iwdg.c
  * @brief   Independent Watchdog (IWDG) configuration
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
#include "iwdg.h"

/* USER CODE BEGIN 0 */

/*
 * Flag globale: indica che l'IWDG è stato inizializzato correttamente
 * e può essere ricaricato dai task FreeRTOS.
 */
volatile uint8_t g_iwdg_started = 0;

/* USER CODE END 0 */

IWDG_HandleTypeDef hiwdg;

/* IWDG init function */
void MX_IWDG_Init(void)
{
  /* USER CODE BEGIN IWDG_Init 0 */
  /* Flag lasciata a 0 finché l'init non è completata */
  g_iwdg_started = 0;
  /* USER CODE END IWDG_Init 0 */

  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_128;
  hiwdg.Init.Reload    = 16383;

  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    /* Init fallita: il watchdog NON è affidabile */
    Error_Handler();
  }

  /* USER CODE BEGIN IWDG_Init 2 */
  /* Da QUI in poi il watchdog è realmente attivo */
  g_iwdg_started = 1;
  /* USER CODE END IWDG_Init 2 */
}

/* USER CODE BEGIN 1 */
/* Nessun codice aggiuntivo */
 /* USER CODE END 1 */
