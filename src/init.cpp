#include "stm32.h"
#include "codal_target_hal.h"
#include "CodalDmesg.h"
#include "ErrorNo.h"

void target_init();

extern "C" __attribute__((weak)) void
apply_clock_init(RCC_OscInitTypeDef *oscInit, RCC_ClkInitTypeDef *clkConfig, uint32_t flashLatency)
{
    HAL_RCC_OscConfig(oscInit);
    HAL_RCC_ClockConfig(clkConfig, flashLatency);
}

void init_clocks()
{

    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;

    /* Enable Power Control clock */
    __HAL_RCC_PWR_CLK_ENABLE();

    /* The voltage scaling allows optimizing the power consumption when the device is
       clocked below the maximum system frequency, to update the voltage scaling value
       regarding system frequency refer to product datasheet.  */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

    /* Enable HSE Oscillator and activate PLL with HSE as source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

    uint32_t pllm = HSE_VALUE / 1000000;

    CODAL_ASSERT(pllm >= 4, DEVICE_HARDWARE_CONFIGURATION_ERROR);
    CODAL_ASSERT(pllm <= 25, DEVICE_HARDWARE_CONFIGURATION_ERROR);
    CODAL_ASSERT(pllm * 1000000 == HSE_VALUE, DEVICE_HARDWARE_CONFIGURATION_ERROR);

    RCC_OscInitStruct.PLL.PLLM = pllm;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
    RCC_OscInitStruct.PLL.PLLQ = 7;

    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
       clocks dividers */
    RCC_ClkInitStruct.ClockType =
        (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    apply_clock_init(&RCC_OscInitStruct, &RCC_ClkInitStruct, FLASH_LATENCY_2);

    SystemCoreClockUpdate();

    __HAL_RCC_TIM5_CLK_ENABLE();

    // enable backup registers (for reboot into bootloader or into app)
    PWR->CR |= PWR_CR_DBP;
    RCC->BDCR |= RCC_BDCR_RTCEN;

    // HACK: In some versions of the bootloader PA9 is used as VSENSE for the usb
    // and enters the application space with PA9 in that mode.
    // Here we disable that mode.
    __HAL_RCC_USB_OTG_FS_CLK_ENABLE();
#ifndef USB_OTG_GCCFG_VBUSASEN
#define USB_OTG_GCCFG_VBUSASEN 0x00040000
#define USB_OTG_GCCFG_VBUSBSEN 0x00080000
#endif
    USB_OTG_FS->GCCFG &= ~(USB_OTG_GCCFG_VBUSASEN | USB_OTG_GCCFG_VBUSBSEN);
    __HAL_RCC_USB_OTG_FS_CLK_DISABLE();
}

extern "C" void cpu_init()
{
    SystemCoreClockUpdate();
    target_init();
    init_clocks();
}

void target_deepsleep()
{
    HAL_PWREx_EnableFlashPowerDown();
    HAL_PWREx_EnableLowRegulatorLowVoltage();
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
    HAL_PWREx_DisableFlashPowerDown();
    init_clocks();
}
