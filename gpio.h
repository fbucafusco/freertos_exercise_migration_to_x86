#ifndef GPIO_H_
#define GPIO_H_

#include <iostream>

#ifdef _WIN32
#include <windows.h> // Incluimos Windows.h para GetAsyncKeyState en Windows
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif

// Tipo básico para representar un pin GPIO
enum class gpioMap_t
{
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
inline void gpioInit( gpioMap_t pin, bool is_output )
{
    std::cout << "GPIO: " << static_cast<int>( pin ) <<
              ( is_output ? " as out.\n" : " as in.\n" );
}

// Función simulada para escribir en un pin GPIO
inline void gpioWrite( gpioMap_t pin, bool state )
{
    std::cout << "GPIO " << static_cast<int>( pin ) << " is " << ( state ? "ON\n" : "OFF\n" );
}

// Función de ayuda para lectura de teclado en Linux
#ifndef _WIN32
inline bool kbhit()
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr( STDIN_FILENO, &oldt );
    newt = oldt;
    newt.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newt );
    oldf = fcntl( STDIN_FILENO, F_GETFL, 0 );
    fcntl( STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK );

    ch = getchar();

    tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
    fcntl( STDIN_FILENO, F_SETFL, oldf );

    if ( ch != EOF )
    {
        ungetc( ch, stdin );
        return true;
    }

    return false;
}
#endif

// Función simulada para leer el estado de un pin GPIO
inline bool gpioRead( gpioMap_t pin )
{
#ifdef _WIN32
    std::cout << "read " << static_cast<int>( pin ) << std::endl;

    // Solo los pines asociados a las teclas tienen lectura permitida
    switch ( pin )
    {
        case gpioMap_t::TEC1:
            return ( GetAsyncKeyState( '1' ) & 0x8000 ) == 0; // Lee tecla '1' para TEC1
        case gpioMap_t::TEC2:
            return ( GetAsyncKeyState( '2' ) & 0x8000 ) == 0; // Lee tecla '2' para TEC2
        case gpioMap_t::TEC3:
            return ( GetAsyncKeyState( '3' ) & 0x8000 ) == 0; // Lee tecla '3' para TEC3
        case gpioMap_t::TEC4:
            return ( GetAsyncKeyState( '4' ) & 0x8000 ) == 0; // Lee tecla '4' para TEC4
        default:
            std::cerr << "Error: Intento de leer un GPIO no asociado a una tecla.\n";
            return false; // GPIO no válido para lectura
    }
#else
    // Solo los pines asociados a las teclas tienen lectura permitida
    if (!kbhit() )
    {
        return false; // No hay tecla presionada
    }

    int ch = getchar();
    switch ( pin )
    {
        case gpioMap_t::TEC1:
            return ch == '1';
        case gpioMap_t::TEC2:
            return ch == '2';
        case gpioMap_t::TEC3:
            return ch == '3';
        case gpioMap_t::TEC4:
            return ch == '4';
        default:
            std::cerr << "Error: Intento de leer un GPIO no asociado a una tecla.\n";
            return false; // GPIO no válido para lectura
    }
#endif
}

#endif /* GPIO_H_ */
