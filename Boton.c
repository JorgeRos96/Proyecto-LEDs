/**
  ******************************************************************************
  * @file    Templates/Src/Boton.c
  * @author  MCD Application Team
  * @brief   Fichero de inicialización del boton de la tarjeta del				 
	*					 microcontrolador. Pin del boton-> PC13
	*					
  *
  * @note    modified by ARM
  *          The modifications allow to use this file as User Code Template
  *          within the Device Family Pack.
  ******************************************************************************
  * 
  ******************************************************************************
  */


#include "stm32f4xx_hal.h"


/**
  * @brief Funcion de inicializacion del boton y activacion de la interrupcion
  * @param None
  * @retval None
  */
void GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);


  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

