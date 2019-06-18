#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "daelttiva/daelttiva.h"
#include "inc/hw_types.h"
#include "driverlib/interrupt.h"
#include "inc/hw_ints.h"

#define RED_LED   GPIO_PIN_1    //PF1
#define BLUE_LED  GPIO_PIN_2    //PF2
#define GREEN_LED GPIO_PIN_3    //PF3
#define SW1       GPIO_PIN_4    //PF4
#define SW2       GPIO_PIN_0    //PF0
#define GPIO_PIN_TYPE_STD_WPU 0x0000000A
#define GPIO_STRENGTH_4MA 0x00000002

#define NUM_REMEDIOS 6

uint32_t periodo = (50e6 / 16) / 50;    // Frequência de 50 Hz
uint32_t teste;

#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
    while(1);
}
#endif

volatile uint8_t segundos = 0, minutos = 0, horas = 0, tam=0;

typedef struct tipo_remedio{
    int posicao;
    int hora_inicial;
    int minuto_inicial;
    int quant_comprimido;
    int intervalo;
}remedios;


remedios *lista_remedios[NUM_REMEDIOS];


void delay_ms(uint16_t atraso)
{

    if (atraso != 0)
        SysCtlDelay(atraso * (SysCtlClockGet() / (3 * 1000))); // Atraso de 1s
}

void Interrupcao_Timer0()
{
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    segundos++;
    if (segundos >= 60)
    {
        minutos++;
        segundos = 0;
    }
    if (minutos >= 60)
    {
        horas++;
        minutos = 0;
    }
    if (horas >= 24)
    {
        horas = 0;
    }
}

void LCD_Setup()
{
    Lcd_Init();
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);
    Lcd_Cmd(_LCD_CLEAR);
}

void Teclado_Setup()
{

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

    // Check if the peripheral access is enabled.
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC));

    // Check if the peripheral access is enabled.
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD));

}

void Menu()
{


    PinDefinition KeypadColumns[4], KeypadRows[4];
    int opc;

    KeypadColumns[0].ui32Port = GPIO_PORTC_BASE;
    KeypadColumns[0].ui8Pin = GPIO_PIN_4;

    KeypadColumns[1].ui32Port = GPIO_PORTC_BASE;
    KeypadColumns[1].ui8Pin = GPIO_PIN_5;

    KeypadColumns[2].ui32Port = GPIO_PORTC_BASE;
    KeypadColumns[2].ui8Pin = GPIO_PIN_6;

    KeypadColumns[3].ui32Port = GPIO_PORTC_BASE;
    KeypadColumns[3].ui8Pin = GPIO_PIN_7;

    KeypadRows[0].ui32Port = GPIO_PORTD_BASE;
    KeypadRows[0].ui8Pin = GPIO_PIN_0;

    KeypadRows[1].ui32Port = GPIO_PORTD_BASE;
    KeypadRows[1].ui8Pin = GPIO_PIN_1;

    KeypadRows[2].ui32Port = GPIO_PORTD_BASE;
    KeypadRows[2].ui8Pin = GPIO_PIN_2;

    KeypadRows[3].ui32Port = GPIO_PORTD_BASE;
    KeypadRows[3].ui8Pin = GPIO_PIN_3;

    Keypad_Init(KeypadColumns, KeypadRows);

    Lcd_Out(1, 1, "1.Adicione");
    Lcd_Out(2, 1, "2.Remova");

    opc = -1;
    while (opc < 0)
        {
            opc = Keypad_Key_Click();
            delay_ms(5);
            if(opc > 2)
            {
                opc = -1;
            }
        }
    Lcd_Cmd(_LCD_CURSOR_OFF);
    Lcd_Cmd(_LCD_CLEAR);

    if(opc == 1)
    {

        Adiciona_Remedio();
    }
    else if (opc == 2)
    {
        Remove_Remedio();
    }

}

void Configura_hora()
{

    uint8_t str[16];

    PinDefinition KeypadColumns[4], KeypadRows[4];
    int teclado;

    KeypadColumns[0].ui32Port = GPIO_PORTC_BASE;
    KeypadColumns[0].ui8Pin = GPIO_PIN_4;

    KeypadColumns[1].ui32Port = GPIO_PORTC_BASE;
    KeypadColumns[1].ui8Pin = GPIO_PIN_5;

    KeypadColumns[2].ui32Port = GPIO_PORTC_BASE;
    KeypadColumns[2].ui8Pin = GPIO_PIN_6;

    KeypadColumns[3].ui32Port = GPIO_PORTC_BASE;
    KeypadColumns[3].ui8Pin = GPIO_PIN_7;

    KeypadRows[0].ui32Port = GPIO_PORTD_BASE;
    KeypadRows[0].ui8Pin = GPIO_PIN_0;

    KeypadRows[1].ui32Port = GPIO_PORTD_BASE;
    KeypadRows[1].ui8Pin = GPIO_PIN_1;

    KeypadRows[2].ui32Port = GPIO_PORTD_BASE;
    KeypadRows[2].ui8Pin = GPIO_PIN_2;

    KeypadRows[3].ui32Port = GPIO_PORTD_BASE;
    KeypadRows[3].ui8Pin = GPIO_PIN_3;

    Keypad_Init(KeypadColumns, KeypadRows);

    TimerIntDisable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);


    UARTprintf("Digite a hora \r\n");
    Lcd_Out(1, 1, "Digite a hora: ");


    teclado = -1;
    while (teclado < 0)
    {
        teclado = Keypad_Key_Click();
        if(teclado > 2)
            teclado = -1;
    }
    sprintf(str,"%d",teclado);
    Lcd_Out(2, 1, str);
    UARTprintf("%d", teclado);
    horas = teclado*10;
    delay_ms(300);
    if(horas==20)
    {
        teclado = -1;
        while (teclado < 0)
        {
            teclado = Keypad_Key_Click();
            if(teclado > 3)
                teclado = -1;
        }
    }
    else
    {
        teclado = -1;
        while (teclado<0)
        {
            teclado = Keypad_Key_Click();
            delay_ms(5);

        }
    }
    sprintf(str,"%d",teclado);
    Lcd_Out(2, 2, str);
    UARTprintf("%d", teclado);
    horas = horas+teclado;

    delay_ms(1500);
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);

    UARTprintf("Digite o minuto \r\n");
    Lcd_Out(1, 1, "Digite o minuto: ");


    teclado = -1;
    while (teclado < 0)
    {
        teclado = Keypad_Key_Click();
        if(teclado > 5)
            teclado = -1;
    }
    sprintf(str,"%d",teclado);
    Lcd_Out(2, 1, str);
    UARTprintf("%d", teclado);
    minutos = teclado*10;
    delay_ms(300);
    teclado = -1;
    while (teclado<0)
    {
        teclado = Keypad_Key_Click();
        delay_ms(5);

    }
    sprintf(str,"%d",teclado);
    Lcd_Out(2, 2, str);
    UARTprintf("%d", teclado);
    minutos = minutos+teclado;

    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);



    sprintf(str,"%2d",horas);
    Lcd_Out(1, 5, str);
    Lcd_Out(1, 7, ":");
    sprintf(str,"%2d",minutos);
    Lcd_Out(1, 8, str);
    UARTprintf("%d", teclado);
    Lcd_Out(2, 1, "* C");
    Lcd_Out(2, 14, "# E");

    teclado =-1;
    while (teclado<0)
        {
            teclado = Keypad_Key_Click();
            delay_ms(5);

        }
    Lcd_Cmd(_LCD_CURSOR_OFF);
    Lcd_Cmd(_LCD_CLEAR);

    if(teclado == 35)
    {

        Configura_hora();
    }
    else if (teclado == 42)
    {
        TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    }


}

void Adiciona_Remedio()
{

    PinDefinition KeypadColumns[4], KeypadRows[4];
    int i;
    int k;
    int teclado;
    remedios remedio_novo;

    KeypadColumns[0].ui32Port = GPIO_PORTC_BASE;
    KeypadColumns[0].ui8Pin = GPIO_PIN_4;

    KeypadColumns[1].ui32Port = GPIO_PORTC_BASE;
    KeypadColumns[1].ui8Pin = GPIO_PIN_5;

    KeypadColumns[2].ui32Port = GPIO_PORTC_BASE;
    KeypadColumns[2].ui8Pin = GPIO_PIN_6;

    KeypadColumns[3].ui32Port = GPIO_PORTC_BASE;
    KeypadColumns[3].ui8Pin = GPIO_PIN_7;

    KeypadRows[0].ui32Port = GPIO_PORTD_BASE;
    KeypadRows[0].ui8Pin = GPIO_PIN_0;

    KeypadRows[1].ui32Port = GPIO_PORTD_BASE;
    KeypadRows[1].ui8Pin = GPIO_PIN_1;

    KeypadRows[2].ui32Port = GPIO_PORTD_BASE;
    KeypadRows[2].ui8Pin = GPIO_PIN_2;

    KeypadRows[3].ui32Port = GPIO_PORTD_BASE;
    KeypadRows[3].ui8Pin = GPIO_PIN_3;

    Keypad_Init(KeypadColumns, KeypadRows);
//Posição
    Lcd_Cmd(_LCD_CURSOR_OFF);
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Out(1,1, "Posicao:");

    while (teclado<0)
    {
        teclado = Keypad_Key_Click();
        delay_ms(5);
        if(teclado > (NUM_REMEDIOS-1))
            teclado = -1;
    }
    remedio_novo.posicao=teclado
    sprintf(str,"%d",remedio_novo.posicao);
    Lcd_Out(1, 10, str);
    delay_ms(1500);

//Hora inicial
    Lcd_Cmd(_LCD_CURSOR_OFF);
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Out(1,1, "Hora:");

    teclado = -1;
       while (teclado < 0)
       {
           teclado = Keypad_Key_Click();
           if(teclado > 2)
               teclado = -1;
       }
       sprintf(str,"%d", teclado);
       Lcd_Out(1, 7, str);
       UARTprintf("%d", teclado);
       remedio_novo.hora_inicial = teclado*10;
       delay_ms(300);
       if(remedio_novo.hora_inicial==20)
       {
           teclado = -1;
           while (teclado < 0)
           {
               teclado = Keypad_Key_Click();
               if(teclado > 3)
                   teclado = -1;
           }
       }
       else
       {
           teclado = -1;
           while (teclado<0)
           {
               teclado = Keypad_Key_Click();
               delay_ms(5);

           }
       }
       sprintf(str,"%d",teclado);
       Lcd_Out(2, 2, str);
       UARTprintf("%d", teclado);
       remedio_novo.hora_inicial = remedio_novo.hora_inicial+teclado;

//Minuto_inicial
       Lcd_Cmd(_LCD_CURSOR_OFF);
           Lcd_Cmd(_LCD_CLEAR);
           Lcd_Out(1,1, "Minuto:");
       teclado = -1;
           while (teclado < 0)
           {
               teclado = Keypad_Key_Click();
               if(teclado > 5)
                   teclado = -1;
           }
           sprintf(str,"%d",teclado);
           Lcd_Out(2, 1, str);
           UARTprintf("%d", teclado);
           remedio_novo.minuto_inicial = teclado*10;
           delay_ms(300);
           teclado = -1;
           while (teclado<0)
           {
               teclado = Keypad_Key_Click();
               delay_ms(5);

           }
           sprintf(str,"%d",teclado);
           Lcd_Out(2, 2, str);
           UARTprintf("%d", teclado);
           remedio_novo.minuto_inicial = remedio_novo.minuto_inicial+teclado;
           delay_ms(1500);


           Lcd_Cmd(_LCD_CLEAR);
           Lcd_Cmd(_LCD_CURSOR_OFF);

//Quantidade de comprimido

           Lcd_Cmd(_LCD_CURSOR_OFF);
               Lcd_Cmd(_LCD_CLEAR);
               Lcd_Out(1,1, "Quantidade:");
           teclado = -1;
               while (teclado < 0)
               {
                   teclado = Keypad_Key_Click();
                   delay_ms(5);
               }
               sprintf(str,"%d",teclado);
               Lcd_Out(1, 1, str);
               UARTprintf("%d", teclado);
               remedio_novo.quant_comprimido = teclado*10;
               delay_ms(300);

        int intervalo;

*/
}

void Remove_Remedio()
{
    Lcd_Cmd(_LCD_CURSOR_OFF);
    Lcd_Cmd(_LCD_CLEAR);
}

void Imprime_hora()
{
    uint8_t segundos_imprimir [4] = "   ";
    uint8_t minutos_imprimir [4] = "   ";
    uint8_t horas_imprimir [4] = "   ";


    segundos_imprimir[0]=segundos/10;
    segundos_imprimir[0]+=0x30;
    segundos_imprimir[1]=segundos%10;
    segundos_imprimir[1]+=0x30;
    minutos_imprimir[0]=minutos/10;
    minutos_imprimir[0]+=0x30;
    minutos_imprimir[1]=minutos%10;
    minutos_imprimir[1]+=0x30;
    horas_imprimir[0]=horas/10;
    horas_imprimir[0]+=0x30;
    horas_imprimir[1]=horas%10;
    horas_imprimir[1]+=0x30;


    Lcd_Out(1,4, horas_imprimir);
    Lcd_Out(1,6, ":");
    Lcd_Out(1,7, minutos_imprimir);
    Lcd_Out(1,9, ":");
    Lcd_Out(1,10, segundos_imprimir);
}


int main(void)
{
    int i = 0;
    // Habilita clock geral do sistema para rodar em 50 MHz a partir do PLL com cristal
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ| SYSCTL_OSC_MAIN);

    // Habilita e espera o acesso ao PORTF
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    {
    }

    // Configura o GPIOF para operação com LEDs
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, RED_LED | BLUE_LED | GREEN_LED);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    IntMasterEnable();
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerPrescaleSet(TIMER0_BASE, TIMER_A, 0);
    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet());
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerEnable(TIMER0_BASE, TIMER_A);
    TimerIntRegister(TIMER0_BASE, TIMER_A, Interrupcao_Timer0);

    for(i=0;i<NUM_REMEDIOS;i++)
    {
        lista_remedios[i]=0;
    }

    ConfigureUART();

    Teclado_Setup();

    LCD_Setup();

    Menu();

    Configura_hora();

    while(1){
        Imprime_hora();
    }
}
