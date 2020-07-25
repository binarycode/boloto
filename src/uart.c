#include <FreeRTOS.h>
#include <task.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

static inline void usart_putc(char c) {
    usart_send_blocking(USART1, c);
}

static void usart_task(void *args __attribute__((unused))) {
    int c = '0' - 1;

    for (;;) {
        gpio_toggle(GPIOA, GPIO6);
        vTaskDelay(pdMS_TO_TICKS(200));
        if ( ++c >= 'Z' ) {
            usart_putc(c);
            usart_putc('\r');
            usart_putc('\n');
            c = '0' - 1;
        } else	{
            usart_putc(c);
        }
    }
}

static void led_task(void *args __attribute__((unused))) {
    for (;;) {
        gpio_toggle(GPIOA, GPIO7);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

int main(void) {
    rcc_clock_setup_pll(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);

    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_USART1);

    gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO6); // USART LED
    gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO7); // READY LED
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9); // USART

    gpio_set_af(GPIOA, GPIO_AF7, GPIO9);

    usart_set_baudrate(USART1, 9600);
    usart_set_databits(USART1, 8);
    usart_set_stopbits(USART1, USART_STOPBITS_1);
    usart_set_mode(USART1, USART_MODE_TX);
    usart_set_parity(USART1, USART_PARITY_NONE);
    usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
    usart_enable(USART1);

    xTaskCreate(usart_task, "UART", 100, NULL, configMAX_PRIORITIES - 1, NULL);
    xTaskCreate(led_task, "LED", 100, NULL, configMAX_PRIORITIES - 1, NULL);
    vTaskStartScheduler();

    for (;;);

    return 0;
}
