#ifndef GPIO_H_
#define GPIO_H_

#include <iostream>

// Tipo básico para representar un pin GPIO
enum class gpioMap_t {
    LEDB,
    LED1,
    LED2,
    LED3,
    TEC1,
    TEC2,
    TEC3,
    TEC4,
    GPIO0,
    GPIO7,
    GPIO5,
    GPIO3,
    GPIO1
};

// Función simulada para inicializar un pin GPIO
inline void gpioInit(gpioMap_t pin, bool is_output) {
    std::cout << "Inicializando GPIO: " << static_cast<int>(pin) << (is_output ? " como salida.\n" : " como entrada.\n");
}

// Función simulada para escribir en un pin GPIO
inline void gpioWrite(gpioMap_t pin, bool state) {
    std::cout << "GPIO " << static_cast<int>(pin) << " está ahora " << (state ? "ENCENDIDO\n" : "APAGADO\n");
}

#endif /* GPIO_H_ */
