/********************************************************************************
 * DISCIPLINA ET77C - SISTEMAS MICROCONTROLADOS - S22
 * PROJETO FINAL DA DISCIPLINA
 * IAGO ROCKEL & RENATA COSTA
 ********************************************************************************/

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

// Número de compartimentos para remédios
#define NUM_REMEDIOS 6

uint32_t periodo = (50e6/16) / 50; // Frequência de 50 Hz
uint32_t teste; 

/********************************************************************************
 *
 * The error routine that is called if the driver library encounters an error.
 *
 ********************************************************************************/
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
    while(1);
}
#endif

/********************************************************************************
 *
 * Variáveis globais do projeto
 *
 ********************************************************************************/
volatile uint8_t segundos = 0, minutos = 0, horas = 0, tam = 0;

typedef struct tipo_remedio {
    int posicao; // compartimento em que o remédio será alocado
    int hora_inicial; // hora para tomar o remédio
    int minuto_inicial; // minuto para tomar o remédio
    int quant_comprimido; // quantidade de comprimidos em uma dose
    int intervalo; // intervalo em horas entre as doses
} remedios;


remedios *lista_remedios[NUM_REMEDIOS];

/********************************************************************************
 *
 * Função de atraso
 *
 ********************************************************************************/
void delay_ms(uint16_t atraso) {
    if (atraso != 0)
        SysCtlDelay(atraso * (SysCtlClockGet() / (3 * 1000))); // Atraso de 1 ms
}

/********************************************************************************
 *
 * Timer para o relógio
 *
 ********************************************************************************/
void Interrupcao_Timer0() {
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT); // limpa a interrupção

    // incrementa os segundos do relógio
    segundos++;

    // incrementa os minutos do relógio e reseta os segundos
    if (segundos >= 60) {
        minutos++;
        segundos = 0;
    }

    // incrementa as horas do relógio
    if (minutos >= 60) {
        horas++;
        minutos = 0;
    }


    if (horas >= 24) {
        horas = 0;
    }
}

/********************************************************************************
 *
 * Configura o LDC
 *
 ********************************************************************************/
void LCD_Setup() {
    Lcd_Init();
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);
    Lcd_Cmd(_LCD_CLEAR);
}

/********************************************************************************
 *
 * Configura os periféricos para utilizar o teclado matricial
 *
 ********************************************************************************/
void Teclado_Setup() {
	// Configura os periféricos C e D
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

    // Check if the peripheral access is enabled.
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC));

    // Check if the peripheral access is enabled.
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD));
}

/********************************************************************************
 *
 * Função que mostra o menu
 *
 ********************************************************************************/
void Menu() {
	// Variável para armazenar a opção escolhida pelo usuário
    int opc;

	// Configura as teclas do teclado matricial
    PinDefinition KeypadColumns[4], KeypadRows[4];

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
    // Fim da configuração do teclado matricial

    // Mostra as opções do menu
    Lcd_Out(1, 1, "1.Adicione");
    Lcd_Out(2, 1, "2.Remova");

    // Atribuição para verificação
    opc = -1; 
    while(opc < 0) {
    	// Le a tecla que foi clicada
        opc = Keypad_Key_Click();
        delay_ms(5);

        // Verificação
        if(opc > 2) {
            opc = -1;
        }
    }

    // Limpa o LCD
    Lcd_Cmd(_LCD_CURSOR_OFF);
    Lcd_Cmd(_LCD_CLEAR);

    if(opc == 1) {
    	// Chama a função para adicionar um remédio
        Adiciona_Remedio();
    }
    else if (opc == 2) {
    	// Chama a função para remover um remédio
        Remove_Remedio();
    }
}

/********************************************************************************
 *
 * Função para configurar a hora
 *
 ********************************************************************************/
void Configura_hora() {

    uint8_t str[16];

    // Configura o teclado matricial
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
    // Fim da configuração do teclado matricial

    // Desabilita a interrupção do timer
    TimerIntDisable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Limpa o LDC
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);

    // Pergunta a hora
    UARTprintf("Digite a hora \r\n");
    Lcd_Out(1, 1, "Digite a hora: ");

    // Atribuição para verificação
    teclado = -1;
    while(teclado < 0) {
    	// Le a tecla que foi clicada
        teclado = Keypad_Key_Click();

        // Algarismo da dezena deve ser <= 2
        if(teclado > 2)
            teclado = -1;
    }

    // Escreve a dezena da hora no LCD
    sprintf(str, "%d", teclado);
    Lcd_Out(2, 1, str);
    UARTprintf("%d", teclado);

    // Salva a dezena da hora na variável
    horas = teclado * 10;
    delay_ms(300);

    // Chega a unidade da hora, de acordo com a dezena
    if(horas == 20) {
        teclado = -1;
        while (teclado < 0) {
            teclado = Keypad_Key_Click();
            if(teclado > 3)
                teclado = -1;
        }
    } else {
        teclado = -1;
        while (teclado < 0) {
            teclado = Keypad_Key_Click();
            delay_ms(5);
        }
    }

    // Escreve a unidade da hora no LCD
    sprintf(str, "%d", teclado);
    Lcd_Out(2, 2, str);
    UARTprintf("%d", teclado);

    // Salva a unidade da hora na variável
    horas = horas + teclado;

    // Aguarda para ir para a etapa seguinte
    delay_ms(1500);
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);

    // Pergunta os minutos
    UARTprintf("Digite o minuto \r\n");
    Lcd_Out(1, 1, "Digite o minuto: ");

    // Atribuição para verificação
    teclado = -1;
    while (teclado < 0) {
    	// Le a tecla que foi clicada
        teclado = Keypad_Key_Click();

        // Algarismo da dezena deve ser < 6
        if(teclado > 5)
            teclado = -1;
    }

    // Escreve a dezena dos minutos no LCD
    sprintf(str, "%d", teclado);
    Lcd_Out(2, 1, str);
    UARTprintf("%d", teclado);

    // Salva a dezena dos minutos na variável
    minutos = teclado * 10;
    delay_ms(300);

    // Atribuição de verificação
    teclado = -1;

    while(teclado < 0) {
    	// Le a tecla que foi clicada
        teclado = Keypad_Key_Click();
        delay_ms(5);
    }

    // Escreve a unidade dos minutos no LCD
    sprintf(str, "%d", teclado);
    Lcd_Out(2, 2, str);
    UARTprintf("%d", teclado);

    // Salva a unidade dos minutos na variável
    minutos = minutos + teclado;

    // Limpa o LCD
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);

    // Formata as horas e escreve no LCD
    sprintf(str, "%2d", horas);
    Lcd_Out(1, 5, str);

    // Divisor hora-minuto
    Lcd_Out(1, 7, ":");

    // Formata os minutos e escreve no LCD
    sprintf(str, "%2d", minutos);
    Lcd_Out(1, 8, str);

    UARTprintf("%d", teclado);

    // Escreve legenda no LDC
    Lcd_Out(2, 1, "* C"); // Horário certo
    Lcd_Out(2, 14, "# E"); // Horário errado

    // Atribuição de verificação
    teclado =-1;

    while(teclado < 0) {
		// Le a tecla que foi clicada
        teclado = Keypad_Key_Click();
        delay_ms(5);
    }

	// Limpa o display
    Lcd_Cmd(_LCD_CURSOR_OFF);
    Lcd_Cmd(_LCD_CLEAR);

    if(teclado == 35) {// #
    	// Configura novamente
        Configura_hora();
    }
    else if (teclado == 42) {
    	// Habilita a interrupção do timer para contar o tempo
        TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    }
}

/********************************************************************************
 *
 * Função para adicionar remédios ao sistema
 *
 ********************************************************************************/
void Adiciona_Remedio() {

    int i;
    int k;
    
    remedios remedio_novo;

    // Configura o teclado matricial
    PinDefinition KeypadColumns[4], KeypadRows[4];

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
    // Fim da configuração do teclado matricial

	/****************************************************************************
	 * Posição
	 ****************************************************************************/
    // Pergunta a posição do remédio
    Lcd_Cmd(_LCD_CURSOR_OFF);
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Out(1, 1, "Posicao:");

    // Atribuição para verificação
	int teclado = -1;
    while (teclado < 0) {
    	// Le a tecla que foi clicada
        teclado = Keypad_Key_Click();
        delay_ms(5);

        // Verifica se é uma posição válida
        if(teclado > (NUM_REMEDIOS-1))
            teclado = -1;
    }

    // Salva a posição do remédio
    remedio_novo.posicao=teclado

    // Escreve a posição do remédio
    sprintf(str, "%d", remedio_novo.posicao);
    Lcd_Out(1, 10, str);

    // Aguarda para seguir para a próxima etapa
    delay_ms(1500);

    /****************************************************************************
	 * Hora
	 ****************************************************************************/
	// Pergunta a hora que o remédio deve ser tomado
    Lcd_Cmd(_LCD_CURSOR_OFF);
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Out(1, 1, "Hora:");

	// Atribuição para verificação
    teclado = -1;
	while (teclado < 0) {
	    // Le a tecla que foi clicada
	    teclado = Keypad_Key_Click();

	    // Verificação
	    if(teclado > 2)
	       teclado = -1;
	}

	// Escreve a dezena da hora no LCD
	sprintf(str, "%d", teclado);
	Lcd_Out(1, 7, str);
	UARTprintf("%d", teclado);

	// Salva a dezena da hora na variável
	remedio_novo.hora_inicial = teclado * 10;
	delay_ms(300);

	// Realiza as verificações para a unidade conforme a dezena
	if(remedio_novo.hora_inicial == 20) {
		teclado = -1;
		while (teclado < 0) {
			// Le a tecla que foi clicada
			teclado = Keypad_Key_Click();

			// Não pode ser > 3
			if(teclado > 3)
				teclado = -1;
		}
	} else {
		teclado = -1;
		while (teclado < 0) {
			// Le a tecla que foi clicada
			teclado = Keypad_Key_Click();
			delay_ms(5);
		}
	}
	// Escreve a unidade no LCD
	sprintf(str, "%d", teclado);
	Lcd_Out(2, 2, str);
	UARTprintf("%d", teclado);

	// Salva a unidade da hora na variável
	remedio_novo.hora_inicial = remedio_novo.hora_inicial + teclado;

	/****************************************************************************
	 * Minutos
	 ****************************************************************************/
	// Pergunta os minutos
	Lcd_Cmd(_LCD_CURSOR_OFF);
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Out(1, 1, "Minuto:");

    // Atribuição para verificação
    teclado = -1;
    while (teclado < 0) {
    	// Le a tecla que foi clicada
    	teclado = Keypad_Key_Click();

    	// Verificação
        if(teclado > 5)
        teclado = -1;
	}

	// Escreve a dezena do minuto no LCD
	sprintf(str, "%d", teclado);
	Lcd_Out(2, 1, str);
	UARTprintf("%d", teclado);

	// Salva a dezena do minuto no LCD
	remedio_novo.minuto_inicial = teclado * 10;
	delay_ms(300);

	// Atribuição para verificação
	teclado = -1;

	while (teclado < 0) {
		// Le a tecla que foi clicada
	   	teclado = Keypad_Key_Click();
	   	delay_ms(5);
	}

	// Escreve a unidade do minuto no LCD
	sprintf(str, "%d", teclado);
	Lcd_Out(2, 2, str);
	UARTprintf("%d", teclado);

	// Salva a unidade do minuto na variável
	remedio_novo.minuto_inicial = remedio_novo.minuto_inicial + teclado;
	delay_ms(1500);

	// Limpa o LCD
	Lcd_Cmd(_LCD_CLEAR);
	Lcd_Cmd(_LCD_CURSOR_OFF);

	/****************************************************************************
	 * Comprimidos
	 ****************************************************************************/
	// Pergunta a quantidade de comprimidos
	Lcd_Cmd(_LCD_CURSOR_OFF);
	Lcd_Cmd(_LCD_CLEAR);
	Lcd_Out(1, 1, "Quantidade:");

	// Atribuição para verificação
	teclado = -1;

	while (teclado < 0) {
		// Le qual tecla foi clicada
		teclado = Keypad_Key_Click();
		delay_ms(5);
	}

	// Escreve a quantidade de comprimidos 
	sprintf(str, "%d", teclado);
	Lcd_Out(1, 1, str);
	UARTprintf("%d", teclado);
	remedio_novo.quant_comprimido = teclado * 10;
	delay_ms(300);

	// TODO
	int intervalo;


}

/********************************************************************************
 *
 * Função para remover remédios
 *
 ********************************************************************************/
void Remove_Remedio() {
	// Limpa o LCD
    Lcd_Cmd(_LCD_CURSOR_OFF);
    Lcd_Cmd(_LCD_CLEAR);
}

/********************************************************************************
 *
 * Função para mostrar a hora no LCD
 *
 ********************************************************************************/
void Imprime_hora() {

    uint8_t segundos_imprimir [4] = "   ";
    uint8_t minutos_imprimir [4] = "   ";
    uint8_t horas_imprimir [4] = "   ";


    segundos_imprimir[0] = segundos/10;
    segundos_imprimir[0] += 0x30;
    segundos_imprimir[1] = segundos%10;
    segundos_imprimir[1] += 0x30;
    minutos_imprimir[0] = minutos/10;
    minutos_imprimir[0] += 0x30;
    minutos_imprimir[1] = minutos%10;
    minutos_imprimir[1] += 0x30;
    horas_imprimir[0] = horas/10;
    horas_imprimir[0] += 0x30;
    horas_imprimir[1] = horas%10;
    horas_imprimir[1] += 0x30;


    Lcd_Out(1, 4, horas_imprimir);
    Lcd_Out(1, 6, ":");
    Lcd_Out(1, 7, minutos_imprimir);
    Lcd_Out(1, 9, ":");
    Lcd_Out(1, 10, segundos_imprimir);
}

/********************************************************************************
 *
 * Função main
 *
 ********************************************************************************/
int main(void) {
    int i = 0;
    // Habilita clock geral do sistema para rodar em 50 MHz a partir do PLL com cristal
    SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    // Habilita e espera o acesso ao PORTF
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    {
    }

    // Configura o GPIOF para operação com LEDs
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, RED_LED | BLUE_LED | GREEN_LED);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    // Configura o Timer
    IntMasterEnable();
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerPrescaleSet(TIMER0_BASE, TIMER_A, 0);
    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet());
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerEnable(TIMER0_BASE, TIMER_A);
    TimerIntRegister(TIMER0_BASE, TIMER_A, Interrupcao_Timer0);

    for(i = 0; i < NUM_REMEDIOS; i++) {
        lista_remedios[i] = 0;
    }

    // Configura UART
    ConfigureUART();

    // Configura o Teclado
    Teclado_Setup();

    // Configura o display LCD
    LCD_Setup();

    // Exibe o menu
    Menu();

    // Configura a hora
    Configura_hora();

    while(1) {
        Imprime_hora();
    }
}
