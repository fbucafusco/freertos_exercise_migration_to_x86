#ifndef KEYS_H_
#define KEYS_H_

#include "gpio.h"
#include <semaphore> // Cambiado para usar binary_semaphore
#include <chrono>
#include <vector>

/* public macros ================================================================= */
#define KEYS_INVALID_TIME -1
#define KEY_COUNT 4

/* types ================================================================= */

enum keys_ButtonState_t
{
    STATE_BUTTON_UP,
    STATE_BUTTON_DOWN,
    STATE_BUTTON_FALLING,
    STATE_BUTTON_RISING
};

/* Configuración y datos de cada tecla */
struct t_key_config
{
    gpioMap_t btn;                      // Configuración de GPIO
};

struct t_key_data
{
    keys_ButtonState_t state;
    std::chrono::steady_clock::time_point time_down;
    std::chrono::steady_clock::time_point time_up;
    std::binary_semaphore sem_btn{0};   // Semáforo binario para sincronización de la tecla
    int time_diff;

    t_key_data();
};

/* methods ================================================================= */

void keys_init();
int keys_get_diff( uint32_t index );
void keys_clear_diff( uint32_t index );
void keys_service_task();
void keys_Update( uint32_t index );
void key_wait( uint32_t index );
#endif /* KEYS_H_ */
