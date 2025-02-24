#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "inc/ssd1306.h"
#include "neopixel.c"

#define BUTTON_A 5
#define BUTTON_B 6
#define JOYSTICK_BUTTON 22
#define MIC_CHANNEL 2
#define MIC_PIN (26 + MIC_CHANNEL)
#define RANDOM_DELAY_MIN_MS 100
#define RANDOM_DELAY_MAX_MS 5000
#define LED_PIN 7
#define LED_COUNT 25
#define BUZZER_PIN 21

enum Screen {
    SCREEN_INITIAL,
    SCREEN_COUNTDOWN,
    SCREEN_MEASURING,
    SCREEN_RESULT
};

void beep_simple(uint pin, uint duration_ms) {
    uint slice_num = pwm_gpio_to_slice_num(pin);
    uint16_t wrap = 1000;
    float freq = 1000.0f;
    float divider = (float)clock_get_hz(clk_sys) / (freq * (wrap + 1));
    pwm_set_clkdiv(slice_num, divider);
    pwm_set_wrap(slice_num, wrap);
    pwm_set_gpio_level(pin, wrap / 2);
    pwm_set_enabled(slice_num, true);
    sleep_ms(duration_ms);
    pwm_set_enabled(slice_num, false);
}

void beep_agudo_longo(uint pin, uint duration_ms) {
    uint slice_num = pwm_gpio_to_slice_num(pin);
    uint16_t wrap = 1000;
    float freq = 4000.0f;
    float divider = (float)clock_get_hz(clk_sys) / (freq * (wrap + 1));
    pwm_set_clkdiv(slice_num, divider);
    pwm_set_wrap(slice_num, wrap);
    pwm_set_gpio_level(pin, (uint)(wrap * 0.75));
    pwm_set_enabled(slice_num, true);
    sleep_ms(duration_ms);
    pwm_set_enabled(slice_num, false);
}

void wait_for_button_release() {
    while (!gpio_get(BUTTON_A) || !gpio_get(BUTTON_B) || !gpio_get(JOYSTICK_BUTTON))
        sleep_ms(10);
}

void draw_initial_screen(uint8_t *ssd, int x1, int x2, int y_texto, int x_piscar, int y_piscar, bool exibir_piscar, char *linha1, char *linha2, char *texto_piscar) {
    memset(ssd, 0, ssd1306_buffer_length);
    for (int i = 0; i < ssd1306_width; i++) {
        ssd[i] |= 0x01;
        ssd[ssd1306_buffer_length - ssd1306_width + i] |= 0x80;
    }
    for (int page = 0; page < ssd1306_n_pages; page++) {
        ssd[page * ssd1306_width] |= 0xFF;
        ssd[(page + 1) * ssd1306_width - 1] |= 0xFF;
    }
    ssd1306_draw_string(ssd, x1, y_texto, linha1);
    ssd1306_draw_string(ssd, x2, y_texto + 8, linha2);
    if (exibir_piscar)
        ssd1306_draw_string(ssd, x_piscar, y_piscar, texto_piscar);
}

void draw_countdown_screen(uint8_t *ssd, int count) {
    memset(ssd, 0, ssd1306_buffer_length);
    char count_str[3];
    snprintf(count_str, sizeof(count_str), "%d", count);
    int x_count = (ssd1306_width - strlen(count_str) * 6) / 2;
    int y_count = (ssd1306_height - 8) / 2;
    ssd1306_draw_string(ssd, x_count, y_count, count_str);
}

void draw_waiting_screen(uint8_t *ssd) {
    memset(ssd, 0, ssd1306_buffer_length);
    char *texto = "AGORA";
    int x = (ssd1306_width - strlen(texto) * 6) / 2;
    int y = (ssd1306_height - 8) / 2;
    ssd1306_draw_string(ssd, x, y, texto);
}

void draw_reaction_time_screen(uint8_t *ssd, float reaction_time) {
    memset(ssd, 0, ssd1306_buffer_length);
    char reaction_time_str[16];
    snprintf(reaction_time_str, sizeof(reaction_time_str), "%.2f s", reaction_time);
    int x_reaction = (ssd1306_width - strlen(reaction_time_str) * 6) / 2;
    int y_reaction = (ssd1306_height - 8) / 2;
    ssd1306_draw_string(ssd, x_reaction, y_reaction, reaction_time_str);
}

void display_digit(uint8_t digit) {
    npClear();
    uint8_t pattern[LED_COUNT];
    if (digit == 3) {
        uint8_t pat3[LED_COUNT] = {
            0,1,1,1,0,
            0,0,0,1,0,
            0,1,1,1,0,
            0,0,0,1,0,
            0,1,1,1,0
        };
        memcpy(pattern, pat3, LED_COUNT);
    } else if (digit == 2) {
        uint8_t pat2[LED_COUNT] = {
            0,1,1,1,0,
            1,0,0,0,1,
            0,0,1,1,0,
            0,1,0,0,0,
            1,1,1,1,1
        };
        memcpy(pattern, pat2, LED_COUNT);
    } else if (digit == 1) {
        uint8_t pat1[LED_COUNT] = {
            0,0,1,0,0,
            0,1,1,0,0,
            0,0,1,0,0,
            0,0,1,0,0,
            0,1,1,1,0
        };
        memcpy(pattern, pat1, LED_COUNT);
    } else {
        memset(pattern, 0, LED_COUNT);
    }
    for (int i = 0; i < LED_COUNT; i++) {
        int row = i / 5;
        int col = i % 5;
        int design_index = ((4 - row) * 5) + col;
        if (pattern[design_index])
            npSetLED(i, 255, 0, 0);
        else
            npSetLED(i, 0, 0, 0);
    }
    npWrite();
}

void display_agora() {
    npClear();
    for (int i = 0; i < LED_COUNT; i++)
        npSetLED(i, 0, 255, 0);
    npWrite();
}

int main() {
    stdio_init_all();
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(14, GPIO_FUNC_I2C);
    gpio_set_function(15, GPIO_FUNC_I2C);
    gpio_pull_up(14);
    gpio_pull_up(15);
    ssd1306_init();
    npInit(LED_PIN, LED_COUNT);
    gpio_init(BUZZER_PIN);
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    adc_gpio_init(MIC_PIN);
    adc_init();
    adc_select_input(MIC_CHANNEL);
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);
    gpio_init(JOYSTICK_BUTTON);
    gpio_set_dir(JOYSTICK_BUTTON, GPIO_IN);
    gpio_pull_up(JOYSTICK_BUTTON);
    struct render_area frame_area = {0, ssd1306_width - 1, 0, ssd1306_n_pages - 1};
    calculate_render_area_buffer_length(&frame_area);
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    char *linha1 = "Human";
    char *linha2 = "Benchmark";
    char *texto_piscar = "Pressione algo";
    int x1 = (ssd1306_width - strlen(linha1) * 6) / 2 - 8;
    int x2 = (ssd1306_width - strlen(linha2) * 6) / 2 - 8;
    int y_texto = (ssd1306_height - 16) / 2 - 8;
    int x_piscar = (ssd1306_width - strlen(texto_piscar) * 6) / 2 - 15;
    int y_piscar = y_texto + 35;
    enum Screen current_screen = SCREEN_INITIAL;
    absolute_time_t start_time, end_time;
    float reaction_time = 0.0f;
    int countdown = 3;
    bool exibir_piscar = true;
    bool beep_agudo_played = false;
    absolute_time_t last_press = get_absolute_time();
    absolute_time_t last_blink = get_absolute_time();
    while (true) {
        bool button_pressed = false;
        if (absolute_time_diff_us(last_press, get_absolute_time()) > 50000) {
            button_pressed = (!gpio_get(BUTTON_A) || !gpio_get(BUTTON_B) || !gpio_get(JOYSTICK_BUTTON));
            if (button_pressed)
                last_press = get_absolute_time();
        }
        switch (current_screen) {
            case SCREEN_INITIAL:
                beep_agudo_played = false;
                if (absolute_time_diff_us(last_blink, get_absolute_time()) > 500000) {
                    exibir_piscar = !exibir_piscar;
                    last_blink = get_absolute_time();
                }
                draw_initial_screen(ssd, x1, x2, y_texto, x_piscar, y_piscar, exibir_piscar, linha1, linha2, texto_piscar);
                if (button_pressed) {
                    beep_simple(BUZZER_PIN, 120);
                    wait_for_button_release();
                    current_screen = SCREEN_COUNTDOWN;
                    countdown = 3;
                }
                break;
            case SCREEN_COUNTDOWN:
                beep_agudo_played = false;
                draw_countdown_screen(ssd, countdown);
                display_digit(countdown);
                beep_simple(BUZZER_PIN, 50);
                render_on_display(ssd, &frame_area);
                sleep_ms(1000);
                countdown--;
                if (countdown < 0) {
                    uint16_t adc_val = adc_read();
                    srand(adc_val);
                    int random_delay = (rand() % (RANDOM_DELAY_MAX_MS - RANDOM_DELAY_MIN_MS + 1)) + RANDOM_DELAY_MIN_MS;
                    sleep_ms(random_delay);
                    start_time = get_absolute_time();
                    current_screen = SCREEN_MEASURING;
                }
                break;
            case SCREEN_MEASURING:
                if (!beep_agudo_played) {
                    beep_agudo_longo(BUZZER_PIN, 100);
                    beep_agudo_played = true;
                }
                draw_waiting_screen(ssd);
                display_agora();
                if (button_pressed) {
                    wait_for_button_release();
                    end_time = get_absolute_time();
                    beep_simple(BUZZER_PIN, 50);
                    reaction_time = absolute_time_diff_us(start_time, end_time) / 1000000.0f;
                    current_screen = SCREEN_RESULT;
                }
                break;
            case SCREEN_RESULT:
                beep_agudo_played = false;
                draw_reaction_time_screen(ssd, reaction_time);
                npClear();
                npWrite();
                if (button_pressed) {
                    wait_for_button_release();
                    current_screen = SCREEN_INITIAL;
                }
                break;
        }
        render_on_display(ssd, &frame_area);
        sleep_ms(10);
    }
    return 0;
}
