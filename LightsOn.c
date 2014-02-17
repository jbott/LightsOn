//*****************************************************************************
//
// Lights!
// 
// Written by John Ott
//
//*****************************************************************************

#include <math.h>
#include "inc/lm4f120h5qr.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/interrupt.h"
#include "utils/uartstdio.h"
#include "rgb.h"

#ifdef DEBUG_FLAG
#define D(call) call
#else
#define D(call)
#endif

// Define LED Pins
#define RED_LED     GPIO_PIN_1
#define BLUE_LED    GPIO_PIN_2
#define GREEN_LED   GPIO_PIN_3

// Define RGB offsets
#define RED     0
#define GREEN   1
#define BLUE    2

#define PI          3.1415926535897932384626433832795f
#define STEP_SIZE   (PI / 100.0f)

#define LED_MAX 0xFFFF
#define LED_MIN 0x0000



volatile unsigned long CurrColor[3] = {LED_MIN, LED_MIN, LED_MIN};
float colorWheelPos = 0;

void
setColor(float pos)
{
    float fTemp;

    //
    // Adjust the BLUE value based on the control state
    //
    fTemp = 65535.0f * sinf(pos);
    if(fTemp < 0)
    {
        CurrColor[GREEN] = 0;
    }
    else
    {
        CurrColor[GREEN] = (unsigned long) fTemp;
    }

    //
    // Adjust the RED value based on the control state
    //
    fTemp = 65535.0f * sinf(pos - PI / 2.0f);
    if(fTemp < 0)
    {
        CurrColor[BLUE] = 0;
    }
    else
    {
        CurrColor[BLUE] = (unsigned long) fTemp;
    }

    //
    // Adjust the GREEN value based on the control state
    //
    if(pos < PI)
    {
        fTemp = 65535.0f * sinf(pos + PI * 0.5f);
    }
    else
    {
        fTemp = 65535.0f * sinf(pos + PI);
    }
    if(fTemp < 0)
    {
        CurrColor[RED] = 0;
    }
    else
    {
        CurrColor[RED] = (unsigned long) fTemp;
    }
}

void
SysTickIntHandler(void)
{
    // Update color wheel pos
    colorWheelPos += STEP_SIZE;
    if (colorWheelPos > (PI * 1.5f)) {
        colorWheelPos = 0.0f;
    }
    if (colorWheelPos < 0.0f) {
        colorWheelPos = PI * 1.5f;
    }
    setColor(colorWheelPos);
    RGBColorSet(CurrColor);
}

int
main()
{

    // Initialize UART for debugging

    //
    // Enable lazy stacking for interrupt handlers.  This allows floating-point
    // instructions to be used within interrupt handlers, but at the expense of
    // extra stack usage.
    //
    ROM_FPUEnable();
    ROM_FPULazyStackingEnable();

    //
    // Set the clocking to run directly from the crystal at 40 Mhz
    //
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
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

    RGBInit(0);
    RGBIntensitySet(0.9f);
    RGBEnable();

    // Initialize Interrupts
    SysTickPeriodSet(SysCtlClockGet() / 32);
    SysTickEnable();
    SysTickIntEnable();
    IntMasterEnable();

    UARTprintf("Lets get this party started!\n");

    while (1) {}
}

