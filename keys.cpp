#include <thread>
#include <semaphore>
#include <vector>
#include <chrono>
#include <iostream>
#include <windows.h> // Para GetAsyncKeyState en Windows
/*==================[ Inclusions ]============================================*/
#include "keys.h"
#include "gpio.h"

/*=====[ Definitions of private data types ]===================================*/
const std::vector<gpioMap_t> btn_t = {gpioMap_t::TEC1, gpioMap_t::TEC2, gpioMap_t::TEC3, gpioMap_t::TEC4};
/*=====[Definition macros of private constants]==============================*/

#define DEBOUNCE_TIME 200

/*=====[Prototypes (declarations) of private functions]======================*/
static void keys_reset( uint32_t index );
static void keys_event_handler_button_pressed( uint32_t index );
static void keys_event_handler_button_release( uint32_t index );
/*=====[Definitions of public global variables]==============================*/
std::vector<t_key_data> keys_data( KEY_COUNT );
std::vector<t_key_config> keys_config( KEY_COUNT );

/*=====[Implementations of public functions]=================================*/
t_key_data::t_key_data() : state( STATE_BUTTON_UP ), time_diff( KEYS_INVALID_TIME )
{
}

int keys_get_diff( uint32_t index )
{
    return keys_data[index].time_diff;
}

void keys_clear_diff( uint32_t index )
{
    keys_data[index].time_diff = KEYS_INVALID_TIME;
}

void keys_init()
{
    uint32_t i;
    
    for ( i = 0; i < KEY_COUNT; ++i )
    {
        keys_data[i] = t_key_data();
        keys_config[i].btn = btn_t[i];
    }

    std::thread( keys_service_task ).detach();
}

void keys_Update( int index )
{
    switch ( keys_data[index].state )
    {
        case STATE_BUTTON_UP:
            /* CHECK TRANSITION CONDITIONS */
            if (!gpioRead( keys_config[index].btn ) )
            {
                keys_data[index].state = STATE_BUTTON_FALLING;
            }
            break;

        case STATE_BUTTON_FALLING:
            /* ENTRY */

            /* CHECK TRANSITION CONDITIONS */
            if (!gpioRead( keys_config[index].btn ) )
            {
                keys_data[index].state = STATE_BUTTON_DOWN;

                /* ACCION DEL EVENTO !*/
                keys_event_handler_button_pressed( index );
            }
            else
            {
                keys_data[index].state = STATE_BUTTON_UP;
            }
            break;

        case STATE_BUTTON_DOWN:
            /* CHECK TRANSITION CONDITIONS */
            if ( gpioRead( keys_config[index].btn ) )
            {
                keys_data[index].state = STATE_BUTTON_RISING;
            }
            break;

        case STATE_BUTTON_RISING:
            /* ENTRY */

            /* CHECK TRANSITION CONDITIONS */

            if ( gpioRead( keys_config[index].btn ) )
            {
                keys_data[index].state = STATE_BUTTON_UP;

                /* ACCION DEL EVENTO ! */
                keys_event_handler_button_release( index );
            }
            else
            {
                keys_data[index].state = STATE_BUTTON_DOWN;
            }

            /* LEAVE */
            break;

        default:
            keys_reset( index );
            break;
    }
}

/*=====[Implementations of private functions]================================*/
/* accion de el evento de tecla pulsada */
void keys_event_handler_button_pressed( uint32_t index )
{
    keys_data[index].time_down = std::chrono::steady_clock::now();
}

/* accion de el evento de tecla liberada */
void keys_event_handler_button_release( uint32_t index )
{
    keys_data[index].time_up = std::chrono::steady_clock::now();

    keys_data[index].time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(
                                     keys_data[index].time_up - keys_data[index].time_down )
                                 .count();

    if ( keys_data[index].time_diff > 0 )
    {
        keys_config[index].sem_btn.release(); // Libera el semáforo binario solo para el LED asociado a esta tecla
    }
}

void keys_reset( uint32_t index )
{
    keys_data[index].state = STATE_BUTTON_UP;
}

/*=====[Implementations of private functions]=================================*/
void keys_service_task()
{
    while ( true )
    {
        // for ( auto &key : keys_config )

        for ( int i = 0; i < KEY_COUNT; i++ )
        {
            keys_Update( i );
        }
        std::this_thread::sleep_for( std::chrono::milliseconds( DEBOUNCE_TIME ) );
    }
}
