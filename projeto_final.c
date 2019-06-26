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

#include "ourdefines.h"

#define RED_LED   GPIO_PIN_1    //PF1
#define BLUE_LED  GPIO_PIN_2    //PF2
#define GREEN_LED GPIO_PIN_3    //PF3
#define SW1       GPIO_PIN_4    //PF4
#define SW2       GPIO_PIN_0    //PF0
#define GPIO_PIN_TYPE_STD_WPU 0x0000000A
#define GPIO_STRENGTH_4MA 0x00000002



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

Remedios remedios[NUM_REMEDIOS];

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
    // Limpa a interrupção
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Incrementa a variável dos segundos 24h = 86400s
    tempo = (tempo + 1) % 86400;
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
}

/********************************************************************************
 *
 * Função que mostra o menu
 *
 ********************************************************************************/
void Menu() {
    // Variável para armazenar a opção escolhida pelo usuário
    int opc;

    // Mostra as opções do menu
    Lcd_Out(1, 1, "1. Adicione");
    Lcd_Out(2, 1, "2. Remova");

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
    int teclado;

    // Desabilita a interrupção do timer
    TimerIntDisable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    tempo = 0;

    // Limpa o LCD
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);

    // Pergunta as horas
    Lcd_Out(1, 1, "Digite a hora: ");

    // Configura a dezena da hora
    setTime(HORA, DEZENA, 2);

    // Escreve a dezena das horas no LCD
    sprintf(str, "%d", getTime(HORA, DEZENA));
    Lcd_Out(2, 1, str);

    delay_ms(300);

    if(getTime(HORA, DEZENA) == 2) {
        setTime(HORA, UNIDADE, 3);
    } else {
        setTime(HORA, UNIDADE, 9);
    }

    // Escreve a unidade das horas no LCD
    sprintf(str, "%d", getTime(HORA, UNIDADE));
    Lcd_Out(2, 2, str);

    // Aguarda para ir para a etapa seguinte
    delay_ms(1500);
    
    // Limpa o LCD
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);

    // Pergunta os minutos
    Lcd_Out(1, 1, "Digite o minuto: ");

    setTime(MINUTO, DEZENA, 5);

    // Escreve a dezena dos minutos no LCD
    sprintf(str, "%d", getTime(MINUTO, DEZENA));
    Lcd_Out(2, 1, str);

    delay_ms(300);

    setTime(MINUTO, UNIDADE, 9);

    // Escreve a unidade dos minutos no LCD
    sprintf(str, "%d", getTime(MINUTO, UNIDADE));
    Lcd_Out(2, 2, str);

    // Limpa o LCD
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);

    // Formata as horas e escreve no LCD
    sprintf(str, "%02d:%02d", getTime(HORA, 0), getTime(MINUTO, 0));
    Lcd_Out(1, 5, str);

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
 * Função para setar a dezena ou unidade da hora ou minuto
 *
 ********************************************************************************/
void setTime(int timeFactor, int algarismo, int maxValue) {
    readKeyboard(maxValue);
    tempo += ans * timeFactor * algarismo;
}

/********************************************************************************
 *
 * Função para obter a hora ou minuto
 *
 ********************************************************************************/
int getTime(int timeFactor, int algarismo) {
    // Extrai a parcela referente ao timeFactor (hora, minuto ou segundo)
    int ans = tempo / timeFactor % 60;

    // Extrai a dezena ou unidade, se especificado um destes
    if(algarismo == DEZENA)
        ans /= 10;
    else if(algarismo == UNIDADE)
        ans %= 10; 

    return ans;
}

/********************************************************************************
 *
 * Função para ler o teclado quando for entrada em horas/minutos
 *
 ********************************************************************************/
int readKeyboard(int maxValue) {
    // Atribuição para verificação
    int ans -1;
    do {
        // Le a tecla que foi clicada
        ans = Keypad_Key_Click();

        // Verifica se é um algarismo válido
        ans = (ans > maxValue ? -1 : ans);
        delay_ms(5);
    } while(ans > 0);

    return ans;
}

/********************************************************************************
 *
 * Função para adicionar remédios ao sistema
 *
 ********************************************************************************/
void Adiciona_Remedio() {

    Remedios remedio_novo;

    /****************************************************************************
     * Posição
     ****************************************************************************/
    // Pergunta a posição do remédio
    Lcd_Cmd(_LCD_CURSOR_OFF);
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Out(1, 1, "Posicao:");

    // Atribuição para verificação
    int teclado;
    do {
        // Le a tecla que foi clicada
        teclado = Keypad_Key_Click();
        delay_ms(5);

        // Verifica se é uma posição válida e se não está ocupada
        if(teclado > (NUM_REMEDIOS-1) || !remedios[teclado].disponivel) 
            teclado = -1;
    } while(teclado < 0);

    // Salva a posição do remédio
    remedio_novo.posicao = teclado

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

    teclado = readKeyboard(2);

    // Escreve a dezena da hora no LCD
    sprintf(str, "%d", teclado);
    Lcd_Out(1, 7, str);

    // Salva a dezena da hora na variável
    remedio_novo.tempo = teclado * HORA * DEZENA;
    delay_ms(300);

    teclado = (teclado == 2 ? readKeyboard(3) : readKeyboard(9));

    // Escreve a unidade no LCD
    sprintf(str, "%d", teclado);
    Lcd_Out(1, 8, str);

    // Salva a unidade da hora na variável
    remedio_novo.tempo = remedio_novo.tempo + teclado * HORA * UNIDADE;
    delay_ms(1000);

    /****************************************************************************
     * Minutos
     ****************************************************************************/
    // Pergunta os minutos
    Lcd_Cmd(_LCD_CURSOR_OFF);
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Out(1, 1, "Minuto:");

    // Atribuição para verificação
    teclado = readKeyboard(5);

    // Escreve a dezena da hora no LCD
    sprintf(str, "%d", teclado);
    Lcd_Out(1, 9, str);

    // Salva a dezena da hora na variável
    remedio_novo.tempo = teclado * MINUTO * DEZENA;
    delay_ms(300);

    teclado = readKeyboard(9);

    // Escreve a unidade no LCD
    sprintf(str, "%d", teclado);
    Lcd_Out(1, 10, str);

    // Salva a unidade da hora na variável
    remedio_novo.tempo = remedio_novo.tempo + teclado * MINUTO * UNIDADE;
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
    teclado = readKeyboard(9);

    // Escreve a quantidade de comprimidos 
    sprintf(str, "%d", teclado);
    Lcd_Out(1, 13, str);
    UARTprintf("%d", teclado);
    remedio_novo.comprimido = teclado;
    delay_ms(1000);

    /****************************************************************************
     * Intervalo
     ****************************************************************************/
    // Pergunta a quantidade de comprimidos
    Lcd_Cmd(_LCD_CURSOR_OFF);
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Out(1, 1, "Intervalo:");
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

    uint8_t str[];
    sprintf(str, "%02d%:02d:%02d",  getTime(HORA, 0),
                                    getTime(MINUTO, 0),
                                    getTime(SEGUNDO, 0));

    Lcd_Out(1, 4, str);
}

/********************************************************************************
 *
 * Função para configurar o timer
 *
 ********************************************************************************/
void setupTimer() {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    IntMasterEnable();
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerPrescaleSet(TIMER0_BASE, TIMER_A, 0);
    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet());
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerEnable(TIMER0_BASE, TIMER_A);
    TimerIntRegister(TIMER0_BASE, TIMER_A, Interrupcao_Timer0);
}
/********************************************************************************
 *
 * Função main
 *
 ********************************************************************************/
int main(void) {
    // Habilita clock geral do sistema para rodar em 50 MHz a partir do PLL com cristal
    SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    // Configura o Timer
    setupTimer();

    // Configura o Teclado
    Teclado_Setup();

    // Configura o display LCD
    LCD_Setup();

    // Configura a hora
    Configura_hora();
    
    // Variável de verificação
    int teclado = -1;
    while(1) {
        // Mostra as horas no LCD
        Imprime_hora();

        // Leitura do teclado matricial, qualquer tecla chama menu
        teclado = Keypad_Key_Click();
        if(teclado > 0) {
            Menu();
        }
    }
}
