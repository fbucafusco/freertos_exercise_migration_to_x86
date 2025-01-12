/*=============================================================================
 * Copyright (c) 2021, Franco Bucafusco <franco_bucafusco@yahoo.com.ar>
 * 					   Martin N. Menendez <mmenendez@fi.uba.ar>
 * All rights reserved.
 * License: Free
 * Date: 2024/10/27
 * Version: v1.2
 *===========================================================================*/

/*==================[inclusiones]============================================*/
#include "gpio.h"

#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

#include "keys.h"

/*==================[definiciones y macros]==================================*/
#define RATE 1000
#define LED_RATE std::chrono::milliseconds(RATE)
#define WELCOME_MSG "Ejercicio D_2.\r\n"
/*==================[definiciones de datos internos]=========================*/

std::vector<gpioMap_t> leds_t = {gpioMap_t::LEDB, gpioMap_t::LED1, gpioMap_t::LED2, gpioMap_t::LED3};

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/
void gpio_init( void );

// Prototipo de funcion de la tarea
void tarea_led( size_t index );

/*==================[funcion principal]======================================*/
int main()
{
    // ---------- CONFIGURACIONES ------------------------------

    gpio_init();

    std::cout << WELCOME_MSG << std::endl;

    std::vector<std::thread> led_threads;

    // Crear tarea 
    for ( size_t i = 0; i < leds_t.size(); ++i )
    {
        led_threads.emplace_back( tarea_led, i );
    }

    // Inicializo driver de teclas
    keys_init();

    // joining threads
    for ( auto &thread : led_threads )
    {
        thread.join();
    }

    return 0;
}

/*==================[definiciones de funciones internas]=====================*/
void gpio_init()
{
}

/*==================[definiciones de funciones externas]=====================*/
// Implementacion de funcion de la tarea
void tarea_led( size_t index )
{
    while ( true )
    {
        key_wait(  index ); // Espera a la liberación del semáforo de su tecla específica

        int dif = keys_get_diff( index );

        if ( dif == KEYS_INVALID_TIME )
        {
            continue;
        }

        keys_clear_diff( index );

        gpioWrite( leds_t[index], true );
        std::this_thread::sleep_for( std::chrono::milliseconds( dif ) );
        gpioWrite( leds_t[index], false );
    }
}
