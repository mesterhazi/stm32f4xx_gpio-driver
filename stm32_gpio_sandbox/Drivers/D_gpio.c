/*
 * D_gpio.c
 *
 *  Created on: Sep 14, 2019
 *      Author: adam
 */
#include "D_gpio.h"
#include "assert.h"
//#include "ret_values.h"

#define ONES_2BIT 		3U
#define LEN_2BIT		2
#define ONES_4BIT 		0xFU
#define LEN_4BIT		4

/*
 *
 */
void D_GPIO_ClockEn(GPIO_TypeDef* GpioPort, uint8_t Enabled) {
	uint8_t offset = 0;
	uint32_t reg_temp;

	if (GpioPort == GPIOA) {
		offset = 0;
	} else if (GpioPort == GPIOB) {
		offset = 1;
	} else if (GpioPort == GPIOC) {
		offset = 2;
	} else if (GpioPort == GPIOD) {
		offset = 3;
	} else if (GpioPort == GPIOE) {
		offset = 4;
	} else if (GpioPort == GPIOF) {
		offset = 5;
	} else if (GpioPort == GPIOG) {
		offset = 6;
	} else if (GpioPort == GPIOH) {
		offset = 7;
	} else if (GpioPort == GPIOI) {
		offset = 8;
	}

	reg_temp = RCC->AHB1ENR;
	reg_temp &= ~(1U << offset);
	reg_temp |= Enabled << offset;
	RCC->AHB1ENR = reg_temp;
}

void D_Init_Gpio(GPIO_TypeDef* GpioPort, D_GPIO_InitTypeDef* GpioInit) {
	uint32_t reg_mask = 0;
	volatile uint32_t reg_temp = 0;
	uint32_t offset = 0;
	uint16_t pin_mask;

	// check parameters
//	assert(IS_GPIO_ALL_INSTANCE(GpioPort));
//	assert(D_IS_GPIO_PIN(GpioInit->Pin));
//	assert(D_IS_GPIO_MODE(GpioInit->Mode));
//	assert(D_IS_GPIO_PuPd(GpioInit->PuPd));

	for (uint16_t pin = 0; pin < 16; pin++) {
		pin_mask = 1U << pin;
		if (pin_mask & GpioInit->Pin) {
			reg_mask = GpioInit->Mode << (pin * LEN_2BIT);
			// clear mode first
			reg_temp = GpioPort->MODER;
			reg_temp &= ~(ONES_2BIT << (pin * LEN_2BIT));
			reg_temp |= reg_mask;
			GpioPort->MODER = reg_temp;

			// set OutPut speed and type
			if ((GpioInit->Mode == D_GPIO_MODE_OUT)
					|| (GpioInit->Mode == D_GPIO_MODE_AF)) {
				// speed
//				assert(D_IS_GPIO_SPEED(GpioInit->OutSpeed));
				reg_mask = GpioInit->Mode << (pin * LEN_2BIT);
				reg_temp = GpioPort->OSPEEDR;
				// clear speed
				reg_temp &= ~(ONES_2BIT << (pin * LEN_2BIT));
				reg_temp |= reg_mask;
				GpioPort->MODER = reg_temp;

				// output type
//				assert(D_IS_GPIO_OTYPE(GpioInit->OutputType));
				reg_mask = (GpioInit->OutputType << pin);
				reg_temp = GpioPort->OTYPER;
				reg_temp &= ~(1U << pin);
				reg_temp |= reg_mask;
			}

			// set Pu Pd resistors
			reg_mask = (GpioInit->PuPd << (pin * LEN_2BIT));
			reg_temp = GpioPort->PUPDR;
			reg_temp &= ~(ONES_2BIT << (pin * LEN_2BIT));
			reg_temp |= reg_mask;
			GpioPort->PUPDR = reg_temp;

			// Alternate function
			if (GpioInit->Mode == D_GPIO_MODE_AF) {
				reg_mask = GpioInit->AlternateFunc << ((pin % 8) * LEN_4BIT);

				if (pin < 8U) {
					reg_temp = GpioPort->AFR[0];
					reg_temp &= ~(ONES_4BIT << ((pin % 8) * LEN_4BIT));
					reg_temp |= reg_mask;
					GpioPort->AFR[0] = reg_temp;
				} else if (pin < 16U) {
					reg_temp = GpioPort->AFR[1];
					reg_temp &= ~(ONES_4BIT << ((pin % 8) * LEN_4BIT));
					reg_temp |= reg_mask;
					GpioPort->AFR[1] = reg_temp;
				}
			}

			// IT (EXTI)
			if (D_IS_GPIO_IT_MODE(GpioInit->Mode)) {
				RCC->APB2ENR |= (0x1U << 14U);
				while (~(RCC->APB2ENR & (0x1U << 14U))) {
					__NOP();
				}
				// Enable EXTI is SYSCONF_EXTICRx
				reg_temp = SYSCFG->EXTICR[(pin >> 2U)];
				offset = (LEN_4BIT * (pin & 0x3U)); // & 0x3U same as % 4
				reg_temp &= ~(0xFU << offset);
				reg_temp |= ((uint32_t) D_GPIO_GET_INDEX(GpioPort)) << offset;
				SYSCFG->EXTICR[(pin >> 2U)] = reg_temp;
				// Config EXTI_IMR
				reg_temp = EXTI->IMR;
				reg_temp |= (0x1U << pin);
				EXTI->IMR = reg_temp;

				// Config Trigger selection EXTI_RTSR, EXTI_FTSR
				// Falling trigger
				reg_temp = EXTI->FTSR;
				reg_temp &= ~(0x1U << pin);
				if ((GpioInit->Mode == D_GPIO_MODE_IT_FE)
						|| (GpioInit->Mode == D_GPIO_MODE_IT_FRE)) {
					reg_temp |= 0x1U << pin;
				}
				EXTI->FTSR = reg_temp;
				// rising trigger
				reg_temp = EXTI->RTSR;
				reg_temp &= ~(0x1U << pin);
				if ((GpioInit->Mode == D_GPIO_MODE_IT_RE)
						|| (GpioInit->Mode == D_GPIO_MODE_IT_FRE)) {
					reg_temp |= 0x1U << pin;
				}
				EXTI->RTSR = reg_temp;

			}

		} else {/* nothin to init here */
		}
	}

}

void D_GPIO_SetPin(GPIO_TypeDef* GpioPort, uint16_t Pin) {
	assert(Pin & D_GPIO_Pin_MASK);
	GpioPort->ODR |= Pin;
}
void D_GPIO_ResetPin(GPIO_TypeDef* GpioPort, uint16_t Pin) {
	assert(Pin & D_GPIO_Pin_MASK);
	GpioPort->ODR &= Pin;
}
void D_GPIO_TogglePin(GPIO_TypeDef* GpioPort, uint16_t Pin) {
	assert(Pin & D_GPIO_Pin_MASK);
	GpioPort->ODR ^= Pin;
}

D_GPIO_PinState D_GPIO_ReadPin(GPIO_TypeDef* GpioPort, uint16_t Pin){
	uint32_t reg_temp;
	D_GPIO_PinState ret;
	assert(Pin & D_GPIO_Pin_MASK);
	reg_temp = GpioPort->IDR;
	if(reg_temp & Pin){
		ret = D_GPIO_PIN_SET;
	}
	else{
		ret = D_GPIO_PIN_RESET;
	}
	return ret;
}
