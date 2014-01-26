//*****************************************************************************
//
// Lights!
// 
// Written by John Ott
//
//*****************************************************************************

#include "inc/lm4f120h5qr.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"

#ifdef DEBUG_FLAG
#define D(call) call
#else
#define D(call)
#endif

// Define LED Pins
#define RED_LED     GPIO_PIN_1
#define BLUE_LED    GPIO_PIN_2
#define GREEN_LED   GPIO_PIN_3

int main() {

    // Initialize UART for debugging

    //
    // Enable lazy stacking for interrupt handlers.  This allows floating-point
    // instructions to be used within interrupt handlers, but at the expense of
    // extra stack usage.
    //
    ROM_FPULazyStackingEnable();

    //
    // Set the clocking to run directly from the crystal at 50 Mhz
    //
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
                       SYSCTL_OSC_MAIN);
                       
    //
    // Initialize the UART.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTStdioInit(0);

    UARTprintf("\n\n\nWelcome to LightsOn!\n");
    UARTprintf("Built on %s %s\n\n", __DATE__, __TIME__); 


    UARTprintf("Initializing light show...\n");

    // Initialize port F for LEDS
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, RED_LED|BLUE_LED|GREEN_LED);

    // Create colors!
    int color = 0;
    int colors[] = {RED_LED, BLUE_LED, GREEN_LED,
                     RED_LED|BLUE_LED, BLUE_LED|GREEN_LED, RED_LED|GREEN_LED,
                     RED_LED|BLUE_LED|GREEN_LED};

    UARTprintf("Lets get this party started!\n");

    while (++color) {
        D(UARTprintf("Color: %u\n", color - 1));
        GPIOPinWrite(GPIO_PORTF_BASE, RED_LED|BLUE_LED|GREEN_LED, colors[color - 1]);

        // Reset after looping through all of them
        if (color > 6)
            color = 0;

        SysCtlDelay(2500000);
    }
}

