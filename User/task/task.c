#include "task.h"
#include "led.h"
//#include "usart.h"
#include "hal_usart1.h"
#include "timer.h"
#include "systick.h"
#include "adc_dma.h"

// ADC1转换的电压值通过MDA方式传到SRAM
extern __IO uint16_t ADC_ConvertedValue;

// 局部变量，用于保存转换计算后的电压值
float ADC_ConvertedValueLocal;

int task_init(void)
{
    // LED灯的初始化 PC13
    LED_Init();

    /* // 串口1的初始化 */
    /* USART1_Configuration( 115200 ); */
    // 串口1的初始化
    USART1_init();

    // 串口1的DMA初始化
    /* USART1_DMA_Config(); */

    // ADC初始化 PC1
    ADC1_Init();

    // 定时器2的初始化
    TIM2_Init();

    /* USART1_WriteString("hello world!\r\n"); */

    /* USART1_WriteString("DMA send Text!\r\n"); */

    usart1_send_str("hello world!\r\n");

    /* ADC_ConvertedValueLocal =(float) ADC_ConvertedValue/4096*3.3; // 读取转换的AD值 */

    return 0;
}

int task_driver(void)
{
    /* LED_Driver();   //LED driver */
    /* Key_Driver(); */
    usart1_driver();

    return 0;
}
