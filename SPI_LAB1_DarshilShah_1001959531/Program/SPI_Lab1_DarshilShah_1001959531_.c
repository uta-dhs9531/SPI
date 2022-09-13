//SPI Expander LAB 1
//Darshil Shah

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL
// Target uC:       TM4C123GH6PM
// System Clock:    -


// Hardware configuration:
// ST7565R Graphics LCD Display Interface:
//   MOSI on PD3 (SSI1Tx)
//   SCLK on PD0 (SSI1Clk)
//   ~CS on PD1 (SSI1Fss)
//   INT connected to PE3
/**
 * main.c
 */



#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "tm4c123gh6pm.h"
#include "uart0.h"
#include "gpio.h"
#include "spi1.h"
#include "clock.h"
#include "nvic.h"
#include "wait.h"

#define IO_EXPANDER_GREEN_LED_MASK   (0x01)
#define IO_EXPANDER_RED_LED_MASK     (0x02)
#define IO_EXPANDER_PUSH_BUTTON_MASK (0x04)

#define OPCODER  0x47
#define OPCODEW  0x46
#define IODIR    0x00
#define IPOL     0x01
#define GPINTEN  0x02
#define DEFVAL   0x03
#define INTCON   0x04
#define IOCON    0x05
#define GPPU     0x06
#define INTF     0x07
#define INTCAP   0x08
#define GPIO     0x09
#define OLAT     0x0A
#define DEFAULT  0x00

#define SSI1FSS                PORTD,1
#define GPIOE_VECTOR_INTERRUPT 20


uint8_t temp;
uint8_t INTCAPregValue;;



void settingRegister(uint8_t address, uint8_t value)
{
    setPinValue(SSI1FSS,0);
    writeSpi1Data(OPCODEW);
    temp=readSpi1Data();
    writeSpi1Data(address);
    temp=readSpi1Data();
    writeSpi1Data(value);
    temp=readSpi1Data();
    setPinValue(SSI1FSS,1);
}


uint8_t readingPortValue(uint8_t address)
{
    setPinValue(SSI1FSS,0);
    writeSpi1Data(OPCODER);
    temp=readSpi1Data();
    writeSpi1Data(address);
    temp=readSpi1Data();
    writeSpi1Data(0x00);
    INTCAPregValue=readSpi1Data();
    setPinValue(SSI1FSS,1);
    return INTCAPregValue;;

}

void GPIOE_isr()
{
    settingRegister(GPIO, IO_EXPANDER_GREEN_LED_MASK);
    waitMicrosecond(1000000);
    settingRegister(GPIO, IO_EXPANDER_RED_LED_MASK);
    clearPinInterrupt(PORTE, 3);
    INTCAPregValue= readingPortValue(INTCAP);
}


int main(void)
{
    initSystemClockTo40Mhz();

    // Initialize SPI1 interface
    initSpi1(USE_SSI_RX);
    setSpi1BaudRate(2e6, 40e6);
    setSpi1Mode(1, 1);

    // Enable clocks port E3 as an Input and enable Interrupt on Tiva

    enablePort(PORTE);
    _delay_cycles(3);
    selectPinDigitalInput(PORTE, 3);
    enablePinPullup(PORTE, 3);

    enableNvicInterrupt(GPIOE_VECTOR_INTERRUPT);
    disablePinInterrupt(PORTE, 3);
    selectPinInterruptLowLevel(PORTE,3);
    enablePinInterrupt(PORTE, 3);
    clearPinInterrupt(PORTE, 3);

    //Setting SPI Expander config
    settingRegister(GPINTEN, DEFAULT);
    settingRegister(IODIR, IO_EXPANDER_PUSH_BUTTON_MASK);
    settingRegister(DEFVAL, IO_EXPANDER_PUSH_BUTTON_MASK);
    settingRegister(INTCON, IO_EXPANDER_PUSH_BUTTON_MASK);
    settingRegister(GPINTEN, IO_EXPANDER_PUSH_BUTTON_MASK);
    settingRegister(GPIO, IO_EXPANDER_RED_LED_MASK);




    while(true)
    {


    }
}
