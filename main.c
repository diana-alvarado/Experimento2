//**************************************************************************************************************
//Laboratorio 6
//Diana Alvarado
//20525

//**************************************************************************************************************
// Librerias para que funcione
//**************************************************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
//**************************************************************************************************************
// Variables Globales
//**************************************************************************************************************

uint32_t ui32Period;
uint8_t b = 0;
uint8_t LED = 0;
uint8_t Prev = 0;
uint8_t Rec = 0;

//**************************************************************************************************************
// Prototipos de Funcion
//**************************************************************************************************************
void InitUART(void);

//**************************************************************************************************************
// Codigo Principal
//**************************************************************************************************************
int main(void)
{
    // Configuracion del oscilador externo, usando PLL y teniendo una frecuencia de 40MHz
    SysCtlClockSet(
            SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ
                    | SYSCTL_OSC_MAIN);

    //Iniciar UART
    InitUART();

    // Se habilita el reloj para el puerto F
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // Se habilita el reloj para el temporizador
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    // Se establecen como salidas el puerto F
     GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,
                              GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

    // Configuracion del Timer 0 como temporizador peroodico
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    // Se calcula el peroodo para el temporizador (1 seg)
    ui32Period = (SysCtlClockGet()) / 2;
    // Establecer el periodo del temporizador
    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period - 1);

    // Se habilita la interrupcion por el TIMER0A
    IntEnable(INT_TIMER0A);
    // Se establece que exista la interrupcion por Timeout
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    // Se habilitan las interrupciones Globales
    IntMasterEnable();
    // Se habilita el Timer
    TimerEnable(TIMER0_BASE, TIMER_A);
    //**********************************************************************************************************
    // Loop Principal
    //**********************************************************************************************************
    while (1)
    {
    }
}

//**************************************************************************************************************
// Handler de la interrupcion del TIMER 0 - Recordar modificar el archivo tm4c123ght6pm_startup_css.c
//**************************************************************************************************************
void Timer0IntHandler(void)
{
    // Clear the timer interrupt
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    b++;
    // Read the current state of the GPIO pin and
    // write back the opposite state
    if (b == 1)
    {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, LED);
    }
    else if (b == 2)
    {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_3, 0);
        b = 0;
    }
}

void UART0IntHandler(void)
{
    UARTIntClear(UART0_BASE, UART_INT_RX | UART_INT_RT);
    Rec = UARTCharGet(UART0_BASE);

    if(Rec == Prev){
        LED = 0;
        Rec = 0;
    }

    else{
        if(Rec == 'r'){
            LED = 2;
        }
        else if(Rec == 'b'){
            LED = 4;
        }
        else if(Rec == 'g'){
            LED = 8;
        }
        Prev = Rec;
    }


}


void InitUART(void)
{
    /*Enable the GPIO Port A*/
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    /*Enable the peripheral UART Module 0*/
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //GPIOPinConfigure(GPIO_PA0_U0RX);

    /* Make the UART pins be peripheral controlled. */
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    /* Sets the configuration of a UART. */
    UARTConfigSetExpClk(
            UART0_BASE, SysCtlClockGet(), 115200,
            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    UARTFIFOEnable(UART0_BASE);

    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
    UARTIntRegister(UART0_BASE, UART0IntHandler);
    UARTEnable(UART0_BASE);
    IntEnable(INT_UART0);
}
