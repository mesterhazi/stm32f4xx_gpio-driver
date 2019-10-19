/*
 * irq.c
 *
 *  Created on: 2019. okt. 4.
 *      Author: adam
 */

#include "stm32f407xx.h"
#include "main.h"

/**
  * @brief EXTI0 line interrupt handler
  * @retval None
  */
void EXTI0_IRQHandler(void){
	GPIO_IRQHandler(0);
}
/**
  * @brief EXTI1 line interrupt handler
  * @retval None
  */
void EXTI1_IRQHandler(void){
	GPIO_IRQHandler(1);
}
/**
  * @brief EXTI2 line interrupt handler
  * @retval None
  */
void EXTI2_IRQHandler(void){
	GPIO_IRQHandler(2);
}
/**
  * @brief EXTI3 line interrupt handler
  * @retval None
  */
void EXTI3_IRQHandler(void){
	GPIO_IRQHandler(3);
}
/**
  * @brief EXTI4 line interrupt handler
  * @retval None
  */
void EXTI4_IRQHandler(void){
	GPIO_IRQHandler(4);
}
/**
  * @brief EXTI5-9 line interrupt handler
  * @retval None
  */
void EXTI9_5_IRQHandler(void) {
	if (EXTI->PR & (uint32_t) 1U << 5) {
		GPIO_IRQHandler(5);
	}
	if (EXTI->PR & (uint32_t) 1U << 6) {
		GPIO_IRQHandler(6);
	}
	if (EXTI->PR & (uint32_t) 1U << 7) {
		GPIO_IRQHandler(7);
	}
	if (EXTI->PR & (uint32_t) 1U << 8) {
		GPIO_IRQHandler(8);
	}
	if (EXTI->PR & (uint32_t) 1U << 9) {
		GPIO_IRQHandler(9);
	}

}

/**
  * @brief EXTI10-15 line interrupt handler
  * @retval None
  */
void EXTI15_10_IRQHandler(void) {
	if (EXTI->PR & (uint32_t) 1U << 10) {
		GPIO_IRQHandler(10);
	}
	if (EXTI->PR & (uint32_t) 1U << 11) {
		GPIO_IRQHandler(11);
	}
	if (EXTI->PR & (uint32_t) 1U << 12) {
		GPIO_IRQHandler(12);
	}
	if (EXTI->PR & (uint32_t) 1U << 13) {
		GPIO_IRQHandler(13);
	}
	if (EXTI->PR & (uint32_t) 1U << 14) {
		GPIO_IRQHandler(14);
	}
	if (EXTI->PR & (uint32_t) 1U << 15) {
		GPIO_IRQHandler(15);
	}
}


