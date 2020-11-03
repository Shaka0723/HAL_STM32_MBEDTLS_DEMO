/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "mbedtls/config.h"
#include "mbedtls/aes.h"
#include "mbedtls/base64.h"
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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/// 重定向printf函数到串口一
int fputc(int ch, FILE* fp)
{
    while (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TXE) != SET);
    huart1.Instance->DR = (uint8_t)(ch & 0XFF);
    return ch;
}

void mbedtls_base64_demo(void)
{
	int i = 0;
	
	// 原始数据
	uint8_t plaintext[16] = {'A','B','C','D','E','F','G','H','I','J', 'K', 'L', 'M', 'N', 'O', 'P'};

    // base64编码和解码输出数据的长度
    size_t enclen = 0, declen = 0;

    // 存放base64编码输出
    uint8_t encode[32];

    // 存放base64解码输出
    uint8_t decode[32];

	// 编码
    mbedtls_base64_encode(encode, sizeof(encode), &enclen, plaintext, sizeof(plaintext));

	// 解码
    mbedtls_base64_decode(decode, sizeof(decode), &declen, encode, enclen);

    printf("- enclen:%d\r\n", enclen);
    printf("- encode:%s\r\n", encode);
    printf("- declen:%d\r\n", declen);
    printf("- decode:");
	for(i = 0; i < declen; i++)
	{
		printf("%c", (char)decode[i]);
	}
	printf("\r\n");
}

void mbedtls_aes_ecb_demo(void)
{
	int i = 0;
	mbedtls_aes_context ctx;
	
	// 要加密的数据
	uint8_t plaintext[16] = {'A','B','C','D','E','F','G','H','I','J', 'K', 'L', 'M', 'N', 'O', 'P'};

	// 密码
	const uint8_t passwd[32] = "AAAAAAAAAABBBBBBBBBBCCCCCCCCCCDD";
	
	// 加密输出
	uint8_t encrypt[16];
	
	// 解密输出
	uint8_t decrypt[16];

	// 初始化
	mbedtls_aes_init(&ctx);

	// 设置加密密钥
	mbedtls_aes_setkey_enc(&ctx, passwd, 256);
	
	// 加密
	mbedtls_aes_crypt_ecb(&ctx, MBEDTLS_AES_ENCRYPT, plaintext, encrypt);
	
	// 设置解密密钥
	mbedtls_aes_setkey_dec(&ctx, passwd, 256);
	
	// 解密
	mbedtls_aes_crypt_ecb(&ctx, MBEDTLS_AES_DECRYPT, encrypt, decrypt);

	// 清理
	mbedtls_aes_free(&ctx);
	
	printf("encrypt:");
	for(i = 0; i < 16; i++)
	{
		printf("%02X", encrypt[i]);
	}
	printf("\r\n");

	printf("decrypt:");
	for(i = 0; i < 16; i++)
	{
		printf("%c", decrypt[i]);
	}
	printf("\r\n");
}

void mbedtls_aes_cbc_demo(void)
{
	int i = 0;

	mbedtls_aes_context ctx;

	// 密码
	uint8_t passwd[16] = "AAAAAAAAAACCCCDD";

	// 用于加密的向量表
	uint8_t iv_encrypt[16] = { 0X00, 0X01, 0X02, 0X03, 0X10, 0X11, 0X12, 0X13, 0X20, 0X21, 0X22, 0X23, 0X30, 0X31, 0X32, 0X33 };

	// 用于解密的向量表
	uint8_t iv_decrypt[16] = { 0X00, 0X01, 0X02, 0X03, 0X10, 0X11, 0X12, 0X13, 0X20, 0X21, 0X22, 0X23, 0X30, 0X31, 0X32, 0X33 };

	// 待加密的数据
	uint8_t plaintext[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P' };

	// 存储加密后的输出
	uint8_t encrypt[sizeof(plaintext)];

	// 存储解密后的输出
	uint8_t decrypt[sizeof(plaintext)];

	// 加密
	mbedtls_aes_setkey_enc(&ctx, passwd, 128);
	mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_ENCRYPT, sizeof(plaintext), iv_encrypt, plaintext, encrypt);

	// 解密
	mbedtls_aes_setkey_dec(&ctx, passwd, 128);
	mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_DECRYPT, sizeof(plaintext), iv_decrypt, encrypt, decrypt);

	// 打印出加入后的结果
	for (i = 0; i < sizeof(plaintext); i++)
	{
		printf("%02X", encrypt[i]);
	}
	printf("\r\n");

	// 打印处解密后的结果
	for (i = 0; i < sizeof(plaintext); i++)
	{
		printf("%c", decrypt[i]);
	}
	printf("\r\n");
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART1_UART_Init();

    mbedtls_base64_demo();
	mbedtls_aes_ecb_demo();
	mbedtls_aes_cbc_demo();
	
    while (1)
    {
    }
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
