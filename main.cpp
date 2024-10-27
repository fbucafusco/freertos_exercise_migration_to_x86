#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include "gpio.h"
#include "keys.h"

// Configuraciones y definiciones
#define RATE 1000
#define LED_RATE std::chrono::milliseconds(RATE)
#define WELCOME_MSG "Ejercicio D_2.\r\n"

std::vector<gpioMap_t> leds_t = { gpioMap_t::LEDB, gpioMap_t::LED1, gpioMap_t::LED2, gpioMap_t::LED3 };
std::vector<gpioMap_t> gpio_t = { gpioMap_t::GPIO7, gpioMap_t::GPIO5, gpioMap_t::GPIO3, gpioMap_t::GPIO1 };

// std::mutex mtx;
// std::condition_variable cv;

void gpio_init() {
    gpioInit(gpioMap_t::GPIO0, true);
    for (auto& gpio : gpio_t) {
        gpioInit(gpio, true);
    }
}

void tarea_led(size_t index) {

    std::cout << "Tarea LED " << index << " iniciada" << std::endl;
    auto xPeriodicity = LED_RATE;
    auto lastWakeTime = std::chrono::steady_clock::now();

    while (true) 
    {
        std::unique_lock<std::mutex> lock(mtx);        
        cv.wait(lock); // Espera hasta recibir una señal de tecla
        int dif = keys_get_diff(index);
        keys_clear_diff(index);

        gpioWrite(leds_t[index], true);  // Encender
        gpioWrite(gpio_t[index], true);
        std::this_thread::sleep_for(std::chrono::milliseconds(dif));
        gpioWrite(leds_t[index], false); // Apagar
        gpioWrite(gpio_t[index], false);

        lastWakeTime += xPeriodicity;
        std::this_thread::sleep_until(lastWakeTime);
    }
}

int main() {
    std::cout << WELCOME_MSG << std::endl;

    gpio_init();

    // Inicializar hilos para cada LED
    std::vector<std::thread> led_threads;
    for (size_t i = 0; i < leds_t.size(); ++i) {
        led_threads.emplace_back(tarea_led, i);
    }

    // Inicializar teclas
    keys_init();

    // Ejecutar los hilos
    for (auto& thread : led_threads) {
        thread.join();
    }

    return 0;
}
