/**
  ************************************************************************************
  * @file    Templates/Src/main.c 
  * @author  MCD Application Team
  * @brief   Proyecto que realiza el manejo del boton y los LEDs del microcontrolador 
	*					 STM32F429ZI. Las pulsaciones del boton permiten cambiar la frecuencia en
	*					 la que se van alternando el encendido/apagado de los tres LEDs.
	*
	*					 Para manejar la frecuencia de los LEDs se utiliza el Timer 3, con una 
	*					 frecuencia inicial de 1 Hz. Se habilita las interrupciones del Timer
	*					 para realizar el manejo de los LEDs en la función de callback. 
	*					 El botón utilizado se encuentra en el pin PC13 y se habilita la 
	*					 interrupción por pulsación. En la función del manejo de la interrupción
	*					 del botón se realiza la modificación de la frecuencia de los LEDs.
	*					 Los pines de los LEDs se encuentran distribuidos de la siguiente manera:
	*							
	*							- LED rojo->  PB14	(User LD3)
	*					  	- LED azul->  PB7	  (User LD2)
	*					  	- LED verde-> PB0		(User LD1)
	*				
	*					Mediante la USART 3 se van a enviar al terminal del PC los mensajes 
	*					indicando que LED se enciende y si se pulsa el botón.
	*					
  *
  * @note    modified by ARM
  *          The modifications allow to use this file as User Code Template
  *          within the Device Family Pack.
  ************************************************************************************
  * 
  ************************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "USART.h"
#include "LED.h"
#include "Boton.h"


#ifdef _RTE_
#include "RTE_Components.h"             // Component selection
#endif

/* Private variables ---------------------------------------------------------*/
char buf[100];
int size = 0;
int mode = 0;
TIM_HandleTypeDef htim3;
int pulsacion = 1;
int LED = 0;
int boton = 0;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Error_Handler(int fallo);
static void MX_TIM3_Init(void);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{

  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, Flash preread and Buffer caches
       - Systick timer is configured by default as source of time base, but user 
             can eventually implement his proper time base source (a general purpose 
             timer for example or other time source), keeping in mind that Time base 
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
             handled in milliseconds basis.
       - Low Level Initialization
     */
  if (HAL_Init() != HAL_OK)
		Error_Handler(0);


  /* Inicialización y configuración del reloj a 180 MHz */
  SystemClock_Config();
	SystemCoreClockUpdate();

  /* Inicialización del boton*/
  GPIO_Init();
	/*Inicialización del Timer 3*/
  MX_TIM3_Init();
	/*Inicialización de los 3 LEDs*/
	LED_Init();
	/* Habilitación del Timer 3 con generación de interrupción*/
	HAL_TIM_Base_Start_IT(&htim3);

	/* Inicialización de la USART a traves de la función init_USART de la libreria USART
	*	 y habilitación de la transmisión
	*							- Baudrate = 9600 baud
	*							- Word length = 8 bits
	*							- Un bit de stop
	*							- Sin bit de paridad
	*							- Sin control de flujo
	*/
	if (init_USART() != 0)
		Error_Handler(2);
	


  /* Infinite loop */
  while (1)
  {
		/*Pulsación del boton*/
		if (boton == 1){
			boton = 0;
			/* Envío por USART del texto indicando que sLED se ha encendido*/
			size = sprintf(buf,"\rPulsacion del boton\n");
			if (tx_USART(buf, size) != 0)
				Error_Handler(3);
		}
		/* Cambio del estado de los LEDs*/
		if (LED == 1){
			if (mode == 0){
				/* Envío por USART del texto indicando que sLED se ha encendido*/
				size = sprintf(buf,"\rSe ha encencido el LED verde \n");
				if (tx_USART(buf, size) != 0)
					Error_Handler(3);
			}
		
			else if (mode == 1){
				/* Envío por USART del texto indicando que sLED se ha encendido*/
				size = sprintf(buf,"\rSe ha encencido el LED azul \n");
				if (tx_USART(buf, size) != 0)
					Error_Handler(3);
		
			}
			else {
				/* Envío por USART del texto indicando que sLED se ha encendido*/
				size = sprintf(buf,"\rSe ha encencido el LED rojo \n");
				if (tx_USART(buf, size) != 0)
					Error_Handler(3);
			}
		LED = 0;
		}
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 180000000
  *            HCLK(Hz)                       = 180000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 16000000
  *            PLL_M                          = 8
  *            PLL_N                          = 180
  *            PLL_P                          = 2
  *            PLL_Q                          = 4
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode	
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	
  /** Se configura el HSI como fuente de reloj del PLL y se configuran
	* 	los parametros del PLL para ajusta la frecuencia a 180 MHz con una
	* 	frecuencia del HSI de 16 MHZ (por defecto).
	* 	SYSCLK =[(16MHz(frecuencia HSI)/8(PLLM))*180 (PLLN)]/2 (PLLP) = 180 MHz
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler(1);
  }
  /** Se activa el modo de Over Drive para poder alcanzar los 180 MHz
	* 	como frecuencia del sistema
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler(1);
  }
  /** Se selecciona el PLL como fuente de reloj del sistema y se configuran los parametros
	*		para configurar el HCLK, PCLK1 y PCLK2. La frecuencia máxima del HCLK es 180 MHZ, la 
	*		frecuencia máxima del PCLK1 es de 45 MHZ y la frecuencia máxima del PCLK2 es de 90 MHz
	*		HCLK = SYSCK/AHB = 180 MHz / 1 = 180 MHz
	*		PCLK1 = HCLK/APB1 = 180 MHz / 4 = 45 MHZ
	*		PCLK2 = HCLK/APB2 = 180 MHz / 2 = 90 MHZ
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler(1);
  }
}
/**
  * @brief Inicialización y configuración del Timer 3 con una frecuencia de 1 Hz
	*				 y con cuenta ascendente. 
	* 			 Como la frecuencia del sistema es de 180 MHz y el Timer 3 tiene como
	*				 fuente de reloj el APB1 de 90 MHz, se establece un counter de 65536 y 
	*				 un prescaler de 1372 para tener la frecuencia de 1 HZ.
	*				
	*					f = 90*10^6/(65536*1372) = 1 Hz aproximadamente
	*
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{


  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

	/* Configuración del Tim3*/
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 1372;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 0xffff;;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler(4);
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler(4);
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler(4);
  }


}





/**
  * @brief Función de callback del Timer para realizar el encendido de los LEDs
	* @param htim: Timer que produce la interrupción al llegar a la cuenta establecida.  
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	
	/* Se selecciona la interrupción del Timer 3 */
	if(htim == &htim3){
		
		LED = 1;
		
		if (mode == 0){
			
		/*Se enciende el LED azul y se apaga el LED verde*/
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
		mode = 1;
		
		}
	else if (mode == 1){
				
		/*Se enciende el LED rojo y se apaga el LED azul*/
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
		mode = 2;
	
	}
	else {

		/*Se enciende el LED verde y se apaga el LED rojo*/
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
		mode = 0;
		
	}
		
		
	}
}

/**
  * @brief Función de callback de las lineas de interrupción. En este caso la pulsación del botón.
	* 			 Al realizar la pulsacion se varia la frecuencia del Timer.
	* @param GPIO_Pin: Pin que ha provocado la interrupción en este caso GPIO_PIN_13 (boton)
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{	

	boton = 1;
	
	if(pulsacion == 0){
		
	/*Se para el Timer3*/
	HAL_TIM_Base_Stop_IT(&htim3);
	/* Se establece el nuevo prescaler con la frecuencia de 1 Hz*/
	htim3.Instance->PSC = 1372;
	/*Se arranca de nuevo el Timer3*/
	HAL_TIM_Base_Start_IT(&htim3);
	pulsacion = 1;
		
	}
	else if(pulsacion == 1){
	
	/*Se para el Timer3*/
	HAL_TIM_Base_Stop_IT(&htim3);
	/* Se establece el nuevo prescaler con la frecuencia de 2 Hz*/
	htim3.Instance->PSC = 685;
	/*Se arranca de nuevo el Timer3*/
	HAL_TIM_Base_Start_IT(&htim3);
	pulsacion = 2;
		
	}

	else if(pulsacion == 2){
	
	/*Se para el Timer3*/
	HAL_TIM_Base_Stop_IT(&htim3);
	/* Se establece el nuevo prescaler con la frecuencia de 4 Hz*/
	htim3.Instance->PSC = 342;
	/*Se arranca de nuevo el Timer3*/
	HAL_TIM_Base_Start_IT(&htim3);
	pulsacion = 3;
		
	}
	else {
		
	/*Se para el Timer3*/
	HAL_TIM_Base_Stop_IT(&htim3);
	/* Se establece el nuevo prescaler con la frecuencia de 8 Hz*/
	htim3.Instance->PSC = 171;
	/*Se arranca de nuevo el Timer3*/
	HAL_TIM_Base_Start_IT(&htim3);
	pulsacion = 0;
	
	}
	
}
/**
  * @brief  This function is executed in case of error occurrence.
	* @param  fallo: variable que indica donde se ha producido el error
  * @retval None
  */
static void Error_Handler(int fallo)
{
  if(fallo == 0)
		/* Mensaje si se ha producido un error en la inicializacón de la librería HAL*/
		printf(buf,"\r Se ha producido un error al inicializar la librería HAL\n");
	else if (fallo == 1)
		/* Mensaje si se ha producido un error en la inicializacón del reloj del sistema*/
		printf(buf,"\r Se ha producido un error al inicializar el reloj del sistema\n");
	else if(fallo == 2)
		/* Mensaje si se ha producido un error en la inicializacón de la USART*/
		printf(buf,"\r Se ha producido un error al inicializar la USART\n");
	else if (fallo == 3)
		/* Mensaje si se ha producido un error en el envío de datos de la USART*/
		printf(buf,"\r Se ha producido un error al enviar datos por la USART\n");
	else if (fallo == 4)
		/* Mensaje si se ha producido un error en la inicialización del Timer 3*/
		printf(buf,"\r Se ha producido un error al inicializar el Timer 3\n");

  while(1)
  {
  }
}

 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
