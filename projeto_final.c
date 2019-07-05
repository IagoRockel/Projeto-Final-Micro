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
#include "inc/hw_types.h"
#include "driverlib/interrupt.h"
#include "inc/hw_ints.h"

#include "daelttiva/daelttiva.h"
#include "ourdefines.h"

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
 * Função que mostra o menu
 *
 ********************************************************************************/
void Menu() {

    // Indica no LCD que o menu foi aberto
    Lcd_Cmd(_LCD_CURSOR_OFF);
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Out(1, 7, "Menu");
    Lcd_Out(2, 6, "Aberto");

    // Variável para armazenar a opção escolhida pelo usuário
    uint8_t option;

    do {
        // Apresenta o menu no UART
        UARTprintf("\n\nMENU\n\n");
        UARTprintf("1. Cadastrar remédio\n");
        UARTprintf("2. Remover remédio\n");
        UARTprintf("3. Reabastecer remédio\n");
        UARTprintf("0. Voltar\n");
        UARTprintf("\nOpção: ");

        // Le a opção escolhida pelo usuário
        option = UARTgetc();

    // Verifica se é uma opção válida
    } while ('0' > option || option > '3');

    switch(option) {
        case '1':
            cadastraRemedio();
            break;
        case '2':
            deletaRemedio();
            break;
        case '3':
            abasteceRemedio();
            break;
    }
}

/********************************************************************************
 *
 * Função para configurar a hora
 *
 ********************************************************************************/
void Configura_hora() {
    // Desabilita a interrupção do timer
    TimerIntDisable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Zera a variável que conta o tempo
    tempo = 0;

    // Limpa o LCD e indica que o horário está sendo configurado
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);
    Lcd_Out(1,3,"Configurando");
    Lcd_Out(1,5,"horário");

    uint8_t str[3];

    // Mostra a configuração na UART
    UARTprintf("\n\nCONFIGURAÇÃO DE HORÁRIO\n\n");
    UARTprintf("Hora: ");
    int read = UARTgets(str, 2+1);

    if(read) {
        int dezena, minuto;

    }

    // Habilita a interrupção do timer para contar o tempo
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
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
 * Função para adicionar remédios ao sistema
 *
 ********************************************************************************/
void cadastraRemedio() {

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
    remedio_novo.posicao = teclado;

    // Escreve a posição do remédio
    //char *str;
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

    //teclado = readKeyboard(2);

    // Escreve a dezena da hora no LCD
    sprintf(str, "%d", teclado);
    Lcd_Out(1, 7, str);

    // Salva a dezena da hora na variável
    remedio_novo.tempo = teclado * HORA * DEZENA;
    delay_ms(300);

    //teclado = (teclado == 2 ? readKeyboard(3) : readKeyboard(9));

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
    //teclado = readKeyboard(5);

    // Escreve a dezena da hora no LCD
    sprintf(str, "%d", teclado);
    Lcd_Out(1, 9, str);

    // Salva a dezena da hora na variável
    remedio_novo.tempo = teclado * MINUTO * DEZENA;
    delay_ms(300);

    //teclado = readKeyboard(9);

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
    //teclado = readKeyboard(9);

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
void deletaRemedio() {
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

    uint8_t str[16];
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

    // Configura o display LCD
    LCD_Setup();

    // Configura a hora
    Configura_hora();

    // Configura a comunicação UART
    ConfigureUART();

    // Configura o uso dos botões SW1 e SW2
    ButtonsInit();

    // Variável de verificação
    uint32_t sw1Pressed;

    while(1) {
        // Mostra as horas no LCD
        Imprime_hora();

        // Verifica se o botão 1 foi pressionado
        sw1Pressed = !GPIOPinRead(PORT_SW1, SW1);

        if(sw1Pressed) { // se pressionado abre o menu
            Menu();
        }
    }
}
