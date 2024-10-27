#ifndef KEYS_H_
#define KEYS_H_

#include "gpio.h"
#include <condition_variable>
#include <mutex>
#include <chrono>
#include <vector>

/* Public macros */
#define KEYS_INVALID_TIME -1

/* Types */
enum class keys_ButtonState_t {
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_FALLING,
    BUTTON_RISING
};

struct t_key_config {
    gpioMap_t btn;                         // Configuración del botón
    std::condition_variable sem_btn;       // Semáforo simulado
};

struct t_key_data {
    keys_ButtonState_t state;
    std::chrono::steady_clock::time_point time_down;
    std::chrono::steady_clock::time_point time_up;
    int time_diff;

    // Constructor por defecto
    t_key_data() : state(keys_ButtonState_t::BUTTON_UP), time_diff(KEYS_INVALID_TIME) {}
};

/* Variables globales */
extern std::mutex mtx;
extern std::condition_variable cv;

/* Methods */
void keys_init();
int keys_get_diff(uint32_t index);
void keys_clear_diff(uint32_t index);

// Declaración de funciones adicionales
void keys_service_task();
void keys_event_handler_button_pressed(uint32_t index);
void keys_event_handler_button_release(uint32_t index);
void keys_reset(uint32_t index);

#endif /* KEYS_H_ */
