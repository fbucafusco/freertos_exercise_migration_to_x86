#include <thread>
#include <condition_variable>
#include <vector>
#include <mutex>
#include <chrono>
#include <iostream>  // Para std::cin y std::cout
#include <conio.h>   // Para kbhit y getch en Windows (si estás en Linux, necesitarías ncurses o una alternativa)

#include "gpio.h"
#include "keys.h"

// Definiciones y configuraciones de las teclas
const std::vector<gpioMap_t> btn_t = { gpioMap_t::TEC1, gpioMap_t::TEC2, gpioMap_t::TEC3, gpioMap_t::TEC4 };
std::vector<t_key_data> keys_data(btn_t.size());
std::vector<t_key_config> keys_config(btn_t.size());

// Definición de las variables globales
std::mutex mtx;
std::condition_variable cv;

int keys_get_diff(uint32_t index) {
    return keys_data[index].time_diff;
}

void keys_clear_diff(uint32_t index) {
    keys_data[index].time_diff = KEYS_INVALID_TIME;
}

void keys_init() {
    for (size_t i = 0; i < btn_t.size(); ++i) {
        keys_data[i] = t_key_data();
        keys_config[i].btn = btn_t[i];
    }

    std::thread(keys_service_task).detach();
}

void keys_Update(uint32_t index) {
    std::cout << "Actualizando estado de TEC" << index + 1 << std::endl;
    switch (keys_data[index].state) {
        case keys_ButtonState_t::BUTTON_UP:
            if (true /* Simulación de lectura de botón presionado */) {
                keys_data[index].state = keys_ButtonState_t::BUTTON_FALLING;
            }
            break;
        case keys_ButtonState_t::BUTTON_FALLING:
            if (true /* Simulación de botón presionado */) {
                keys_data[index].state = keys_ButtonState_t::BUTTON_DOWN;
                keys_event_handler_button_pressed(index);
            } else {
                keys_data[index].state = keys_ButtonState_t::BUTTON_UP;
            }
            break;
        case keys_ButtonState_t::BUTTON_DOWN:
            if (false /* Simulación de lectura de botón suelto */) {
                keys_data[index].state = keys_ButtonState_t::BUTTON_RISING;
            }
            break;
        case keys_ButtonState_t::BUTTON_RISING:
            if (false /* Simulación de botón suelto */) {
                keys_data[index].state = keys_ButtonState_t::BUTTON_UP;
                keys_event_handler_button_release(index);
            } else {
                keys_data[index].state = keys_ButtonState_t::BUTTON_DOWN;
            }
            break;
        default:
            keys_reset(index);
            break;
    }
}

void keys_event_handler_button_pressed(uint32_t index) {
    keys_data[index].time_down = std::chrono::steady_clock::now();
}

void keys_event_handler_button_release(uint32_t index) {
    keys_data[index].time_up = std::chrono::steady_clock::now();
    keys_data[index].time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(
                                     keys_data[index].time_up - keys_data[index].time_down)
                                     .count();

    if (keys_data[index].time_diff > 0) {
        gpioWrite(gpioMap_t::GPIO0, true);
        cv.notify_all(); // Notificar a las tareas de LEDs
        gpioWrite(gpioMap_t::GPIO0, false);
    }
}

void keys_reset(uint32_t index) {
    keys_data[index].state = keys_ButtonState_t::BUTTON_UP;
}

void keys_service_task() {
    while (true) {
        if (_kbhit()) {  // Solo en Windows; para Linux se puede implementar con otra biblioteca o técnica
            char tecla = _getch();  // Lee la tecla sin necesidad de presionar Enter

            switch (tecla) {
                case '1':
                    keys_Update(0);  // Actualiza el estado de TEC1
                    break;
                case '2':
                    keys_Update(1);  // Actualiza el estado de TEC2
                    break;
                case '3':
                    keys_Update(2);  // Actualiza el estado de TEC3
                    break;
                case '4':
                    keys_Update(3);  // Actualiza el estado de TEC4
                    break;
                default:
                    std::cout << "Tecla no asignada: " << tecla << std::endl;
                    break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(40)); // Tiempo de debounce
    }
}
