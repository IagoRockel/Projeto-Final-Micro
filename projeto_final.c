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

// Variável que contém os remédios
Remedios remedios[NUM_REMEDIOS];

// Variável que armazena o horário em segundos
int tempo;

// Variável que indica a posição atual do motor
int currentPosition = 0;

// Indica se o alarme para tomar remédios deve ser ligado
int alarmeRemedioOn = 0;

// Indica se o alarme de estoque deve ser ligado
int alarmeEstoqueOn = 0;

/********************************************************************************
 *
 * Função de atraso em milisegundos
 *
 ********************************************************************************/
void delay_ms(uint16_t atraso) {
    if (atraso != 0)
        SysCtlDelay(atraso * (SysCtlClockGet() / (3 * 1000))); // Atraso em ms
}

/********************************************************************************
 *
 * Interrupção do timer0
 *
 ********************************************************************************/
void Interrupcao_Timer0() {
    // Limpa a interrupção
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Incrementa a variável do horário atual em segundos
    tempo = (tempo + 1) % (24 * HORA);

    /****************************************************************************
     * Sinalização para tomar os remédios
     ****************************************************************************/

    // Lê os remédios
    int i;
    for(i = 1; i < NUM_REMEDIOS; i++) {
        // Se tiver remédio na posição i
        if(remedios[i].ocupado) {
            // Se estiver na hora de tomar o remédio
            if(remedios[i].horario == tempo) {
                // Sinaliza que o remédio deve ser tomado
                remedios[i].tomar = 1;
            }
        }
    }

    /****************************************************************************
     * Alarmes
     ****************************************************************************/    

    // Se alarme dos remédios está ativado
    if(alarmeRemedioOn) {
        // Lê o estado do LED RGB e inverte
        int signal = ~(GPIOPinRead(PORT_RGB, LED_G));

        // Envia o sinal invertido para o LED
        GPIOPinWrite(PORT_RGB, LED_G, signal);
    } else {
        // Desliga o LED quando o alarme não estiver ativado
        GPIOPinWrite(PORT_RGB, LED_G, LED_D);
    }

    // Se alarme dos remédios está ativado e dos remédios desativado
    if(alarmeEstoqueOn && !alarmeRemedioOn) {
        // Lê o estado do LED vermelho e inverte
        int signal = ~(GPIOPinRead(PORT_RGB, LED_R));

        // Envia o sinal invertido para o LED
        GPIOPinWrite(PORT_RGB, LED_R, signal);
    } else {
        // Desliga o LED
        GPIOPinWrite(PORT_RGB, LED_R, LED_D);
    }
}

/********************************************************************************
 *
 * Configura o LDC
 *
 ********************************************************************************/
void LCD_Setup() {
    // Inicializa o LCD
    Lcd_Init();

    // Limpa a tela
    Lcd_Cmd(_LCD_CLEAR);

    // Tira o cursor
    Lcd_Cmd(_LCD_CURSOR_OFF);
}

/********************************************************************************
 *
 * Função que mostra o menu na UART
 *
 ********************************************************************************/
void Menu() {

    // Indica no LCD que o menu foi aberto
    Lcd_Cmd(_LCD_CURSOR_OFF);
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Out(1, 7, "Menu");
    Lcd_Out(2, 6, "aberto");

    // Apresenta o menu no UART
    UARTprintf("\n\nMENU\n\n");
    UARTprintf("1. Cadastrar remedio\n");
    UARTprintf("2. Remover remedio\n");
    UARTprintf("3. Reabastecer remedio\n");
    UARTprintf("4. Listar remedios\n");
    UARTprintf("5. Configurar horario\n");
    UARTprintf("6. Voltar\n");

    // Variável para armazenar a opção escolhida pelo usuário
    int option = ask1Number(6);

    switch(option) {
        case 1:
            cadastraRemedio();

            // Se menu configurado para ser persistente, é chamado novamente
            if(MENU_PERSISTENT)
                Menu();
            break;
        case 2:
            deletaRemedio();

            // Se menu configurado para ser persistente, é chamado novamente
            if(MENU_PERSISTENT)
                Menu();
            break;
        case 3:
            abasteceRemedio();

            // Se menu configurado para ser persistente, é chamado novamente
            if(MENU_PERSISTENT)
                Menu();
            break;
        case 4:
            listaRemedios();

            // Se menu configurado para ser persistente, é chamado novamente
            if(MENU_PERSISTENT)
                Menu();
            break;
        case 5:
            Configura_hora();

            // Se menu configurado para ser persistente, é chamado novamente
            if(MENU_PERSISTENT)
                Menu();
            break;
        case 6:
            // Indica que o menu não está mais ativo
            UARTprintf("\nMenu encerrado\n");
            break;
    }
}

/********************************************************************************
 *
 * Função para configurar a hora
 *
 ********************************************************************************/
void Configura_hora() {

    // Limpa o LCD e indica que o horário está sendo configurado
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);
    Lcd_Out(1,3,"Configurando");
    Lcd_Out(2,5,"horario");

    // Mostra a configuração na UART
    UARTprintf("\n\nCONFIGURACAO DE HORARIO\n\n");

    // Pergunta o horário para o usuário e armazena na variável tempo
    tempo = askTime();

    // Informa que o horário foi configurado
    UARTprintf("\nHorario configurado!\n");
}

/********************************************************************************
 *
 * Função que pergunta o horário
 *
 ********************************************************************************/
int askTime() {
    // Retorna as horas e minutos informados convertidos para segundos
    return ask2number("Hora: ", 23) * HORA + 
            ask2number("Minuto: ", 59) * MINUTO;
}

/********************************************************************************
 *
 * Função que pergunta um número de um dígito, com um valor máximo de maxValue
 *
 ********************************************************************************/
int ask1Number(int maxValue) {

    // Variável para armazenar o número informado pelo usuário
    int number;

    // Variável para verificação
    int valid = 0;

    do {
        // Lê a resposta do usuário
        uint8_t ans = UARTgetc();

        // Verifica se é um número
        if(isdigit(ans)) {
            // Converte para inteiro
            number = ans - '0';

            // Verifica se é diferente de 0 e menor ou igual maxValue
            if(number != 0 && number <= maxValue) {
                // Indica que a resposta foi válida
                valid = 1;
            }
        }
    // Enquando a resposta for invalida
    } while(!valid);

    // Retorna o número informado
    return number;
}

/********************************************************************************
 *
 * Função que pergunta um número de dois dígitos, com um valor máximo de maxValue
 *
 ********************************************************************************/
int ask2number(char* text, int maxValue) {

    // Variável para armazenar a resposta do usuário
    uint8_t ans[3];

    // Variável para verificação
    int valid = 0;

    // Armazena o número informado pelo usuário
    int number;

    do {
        // Escreve a mensagem do parâmetro text 
        UARTprintf(text);

        // Lê a resposta do usuário
        UARTgets(ans, 2+1);

        // Verifica se os caracteres são números
        if (isdigit(ans[0])) {
            if(isdigit(ans[1])) {
                // Converte os caracteres para int
                number = (ans[0] - '0') * DEZENA + (ans[1] - '0') * UNIDADE;

                // Verifica se a hora digitada é válida
                if(number <= maxValue) {
                    valid = 1;
                }
            }
        }
    // Enquanto não for válido
    } while(!valid);

    // Retorna o número informado
    return number;
}

/********************************************************************************
 *
 * Função que pergunta a posição para o usuário, levando em conta quem chamou
 *
 ********************************************************************************/
int askPosition(int action) {

    // Pergunta a posição
    UARTprintf("Posicao: ");
    
    // Variável para a posição do remédio
    int pos;

    // Verifica qual função chamou
    switch(action) {
    case ADICIONAR:
        do {
            // Obtém um valor para posição
            pos = ask1Number( NUM_REMEDIOS - 1);
        // Enquanto a posição ocupada
        } while (remedios[pos].ocupado);
        break;
    case REMOVER:
        do {
            // Obtém um valor para posição
            pos = ask1Number( NUM_REMEDIOS - 1);
        // Enquanto a posição não ocupada
        } while (!remedios[pos].ocupado);
        break;
    case ABASTECER:
        do {
            // Obtém um valor para posição
            pos = ask1Number( NUM_REMEDIOS - 1);
        // Enquanto a posição não ocupada
        } while (!remedios[pos].ocupado);
        break;
    }

    // Retorna a posição informada pelo usuário
    return pos;
}

/********************************************************************************
 *
 * Função para adicionar remédios ao sistema
 *
 ********************************************************************************/
void cadastraRemedio() {

    // Informa que um remédio está sendo cadastrado no LCD
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);
    Lcd_Out(1,3,"Cadastrando");
    Lcd_Out(2,5,"remedio");

    // Informa que um remédio será cadastrado
    UARTprintf("\n\nCADASTRO DE REMEDIO\n\n");

    // Variável para verificar se há posições disponíveis
    int posDisponivel = 0;

    // Verifica se existe um compartimento vazio
    int i;
    for(i = 1; i < NUM_REMEDIOS; i++) {
        // Se posição não estiver ocupada
        if(!remedios[i].ocupado) {
            // Existe pelo menos uma posição para cadastro
            posDisponivel = 1;
            break;
        }
    }

    // Se não tiver posições disponíveis
    if(!posDisponivel) {
        // Informa o usuário e encerra a função
        UARTprintf("Nenhuma posicao disponivel!\n");
        UARTprintf("CADASTRO ENCERRADO!\n");
        return;
    }

    /****************************************************************************
     * Posição
     ****************************************************************************/

    // Pergunta a posição para adicionar
    int pos = askPosition(ADICIONAR);

    // Escreve a posição
    UARTprintf("%d\n\n", pos);

    /****************************************************************************
    * Estoque
    *****************************************************************************/

    // Gira para a posição escolhida para guardas os comprimidos
    setMotorTo(posicao[pos]);
    UARTprintf("Coloque os comprimidos no compartimento\n");

    // Variável para armazenar o valor informado pelo usuário
    int estoque = ask2number("Quantidade de comprimidos guardados: ", 99);

    // Retorna para a posição inicial
    setMotorTo(POS0);

    /****************************************************************************
     * Dosagem
     ****************************************************************************/

    // Pergunta quantos comprimidos devem ser tomados 
    UARTprintf("\nQuantidade de comprimidos por dose: ");

    // Variável para armazenar a quantidade de comprimidos numa dose
    int dose = ask1Number(9);

    // Escreve a dose
    UARTprintf("%d\n\n", dose);

    /****************************************************************************
     * Horário
     ****************************************************************************/

    // Indica que o usuário deve informar um horário
    UARTprintf("Informe o horario que o remedio deve ser tomado!\n\n");

    // Armazena o horário
    int horario = askTime();

    /****************************************************************************
     * Intervalo
     ****************************************************************************/

    // Indica que o usuário deve informar um intervalo
    UARTprintf("\nInforme o intervalo que o remedio deve ser tomado!\n\n");

    // Armazena o horário
    int intervalo = ask2number("Intervalo em horas: ", 24) * HORA;

    /****************************************************************************
     * Atualiza o horário para a próxima vez que deverá ser tomado
     ****************************************************************************/

    horario = updateTime(horario, intervalo);

    /****************************************************************************
     * Salva o medicamento
     ****************************************************************************/

    remedios[pos].horario = horario;
    remedios[pos].dose = dose;
    remedios[pos].intervalo = intervalo;
    remedios[pos].estoque = estoque;
    remedios[pos].ocupado = 1;

    // Informa que o remédio foi cadastrado
    UARTprintf("\nMEDICAMENTO CADASTRADO!\n");
}

/********************************************************************************
 *
 * Função para ajustar o horário para a próxima vez que deve ser tomado
 *
 ********************************************************************************/
int updateTime(int horario, int intervalo) {

    // Variável para ser incrementada
    int n = 0;

    // Variável que armazena o fator que deve multiplicar intervalo
    int factor = 0;

    // Variável que terá os horários que o remédio deve ser tomado
    int check = horario;

    // Variável que guarda menor diferença entre horários
    int minTimeDifference = 24 * HORA;

    do {
        // Verifica a diferença do horário calculado e do horário atual
        int timeDifference = check - tempo;

        // Se a hora calculado for maior que a hora atual
        if(check > tempo) {
            // Verifica se a diferença atual é menor que a anterior
            if(timeDifference < minTimeDifference) {
                // Atualiza a diferença mínima
                minTimeDifference = timeDifference;

                // Salva o fator da menor diferença
                factor = n;
            }
        }

        // Incrementa n
        n++;

        // Atualiza para o próximo horário
        check = horario + n * intervalo;

        // Enquanto o horário não se repetir 
    } while(horario != check % (24 * HORA));

    // Retorna o horário atualizado
    return (horario + factor * intervalo) % (24 * HORA);
}

/********************************************************************************
 *
 * Função para remover remédios
 *
 ********************************************************************************/
void deletaRemedio() {
    
    // Informa que um remédio está sendo removido no LCD
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);
    Lcd_Out(1,4,"Removendo");
    Lcd_Out(2,5,"remedio");

    // Informa que um remédio será cadastrado
    UARTprintf("\n\nREMOCAO DE REMEDIO\n\n");

    // Variável para verificar se há posições disponíveis
    int posDisponivel = 0;

    // Verifica se existe remédios para serem removidos
    int i;
    for(i = 1; i < NUM_REMEDIOS; i++) {
        // Se posição estiver ocupada
        if(remedios[i].ocupado) {
            // Existe pelo menos uma posição para remover
            posDisponivel = 1;
            break;
        }
    }

    // Se não tiver posições disponíveis
    if(!posDisponivel) {
        // Informa o usuário e encerra a função
        UARTprintf("Nenhum remedio para remover!\n");
        UARTprintf("REMOCAO ENCERRADA!\n");
        return;
    }

    /****************************************************************************
     * Posição
     ****************************************************************************/

    // Pergunta a posição para remover
    int pos = askPosition(REMOVER);

    // Escreve a posição
    UARTprintf("%d\n\n", pos);

    // Verifica se há comprimidos no compartimento
    if(remedios[pos].estoque != 0) {
        // Gira para a posição escolhida
        setMotorTo(posicao[pos]);

        // Variável de verificação
        int retirado = 0;
        
        do {
            // Informa que os comprimidos devem ser retirados
            UARTprintf("Retire os comprimidos do compartimento!\n");

            // Pergunta se foram retirados
            UARTprintf("Comprimidos retirados? [s/n]: ");

            // Lê a resposta do usuário
            uint8_t c = UARTgetc();

            // Se foi retirado
            if(c == 's') {
                // Escreve a resposta do usuário
                UARTprintf("s\n");

                // Informa que foi retirado
                retirado = 1;
            } else if (c == 'n') {
                // Escreve a resposta do usuário
                UARTprintf("n\n");
            }
        // Enquanto não for retirado
        } while(!retirado);

        // Retorna o motor para posição inicial
        setMotorTo(POS0);
    }

    // Remove o remédio
    remedios[pos].horario = 0;
    remedios[pos].dose = 0;
    remedios[pos].intervalo = 0;
    remedios[pos].ocupado = 0;
    remedios[pos].estoque = 0;
    remedios[pos].tomar = 0;

    // Informa que o medicamento foi removido
    UARTprintf("\nREMEDIO REMOVIDO!\n");
}

/********************************************************************************
 *
 * Função para reabastecer comprimidos de um determinado remédio
 *
 ********************************************************************************/
void abasteceRemedio() {

    // Informa que um remédio está sendo abastecido no LCD
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);
    Lcd_Out(1,3,"Abastecendo");
    Lcd_Out(2,5,"remedio");

    // Informa que um remédio será cadastrado
    UARTprintf("\n\nABASTECIMENTO DE REMEDIO\n\n");

    // Variável para verificar se há posições disponíveis
    int posDisponivel = 0;

    // Verifica se existe remédios para serem abastecidos
    int i;
    for(i = 1; i < NUM_REMEDIOS; i++) {
        // Se posição estiver ocupada
        if(remedios[i].ocupado) {
            // Existe pelo menos uma posição para abastecer
            posDisponivel = 1;
            break;
        }
    }

    // Se não tiver posições disponíveis
    if(!posDisponivel) {
        // Informa o usuário e encerra a função
        UARTprintf("Nenhum remedio para abastecer!\n");
        UARTprintf("ABASTECIMENTO ENCERRADO!\n");
        return;
    }

    // Pergunta a posição para abastecer
    int pos = askPosition(ABASTECER);

    // Escreve a posição
    UARTprintf("%d\n", pos);

    // Gira para o compartimento informado
    setMotorTo(posicao[pos]);

    // Pergunta quantos comprimidos foram colocados
    int comprimidos = ask2number("Quantos comprimidos foram adicionados: ", 99);

    // Atualiza a quantidade em estoque
    remedios[pos].estoque += comprimidos;

    // Retorna o motor para posição inicial
    setMotorTo(POS0);

    // Informa que um remédio será cadastrado
    UARTprintf("\nABASTECIMENTO REALIZADO!\n\n");
}

/********************************************************************************
 *
 * Função que lista os remédios cadastrados
 *
 ********************************************************************************/
void listaRemedios() {

    // Informa que os remédios estão sendo listados no LCD
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);
    Lcd_Out(1,5,"Listando");
    Lcd_Out(2,5,"remedios");

    // Informa que os remédios serão listados
    UARTprintf("\n\nLISTA DE REMEDIOS\n\n");

    // Varre todas as posições de remédios
    int i;
    for(i = 1; i < NUM_REMEDIOS; i++) {
        // Informações do remédio i
        UARTprintf("REMEDIO %d\n", i);

        // Verifica se existe remédio cadastrado na posição i
        if(remedios[i].ocupado) {
            // Extrai o horário da próxima dose
            int horario = remedios[i].horario;

            // Informa o horário
            UARTprintf(" - Proxima dose: %02dh%02d\n", 
                    horario / HORA, horario / MINUTO % 60);

            // Informa quantos comprimidos tomar por dose
            UARTprintf(" - Comprimidos por dose: %d\n", remedios[i].dose);

            // Indica o intervalo entre doses
            UARTprintf(" - Intervalo entre doses: %dh\n", 
                        remedios[i].intervalo / HORA);

            // Indica quantos comprimidos estão no compartimento
            UARTprintf(" - Comprimidos em estoque: %d\n", remedios[i].estoque);
        } else {
            // Informa que não tem remédio cadastrado na posição i
            UARTprintf(" - Nao cadastrado\n");
        }

        // Pula uma linha
        UARTprintf("\n");
    }

    // Informa que todos os medicamentos foram listados 
    UARTprintf("\nFIM DA LISTA!\n");
}

/********************************************************************************
 *
 * Função para mostrar a hora no LCD
 *
 ********************************************************************************/
void Imprime_hora() {

    // Texto a ser mostrado no LCD
    uint8_t str[16];

    // Extrai a hora
    int hora = tempo / HORA;

    // Extrai os minutos
    int minuto = tempo / MINUTO % 60;

    // Extrai os segundos
    int segundo = tempo % 60;

    // Formata o texto
    sprintf(str, "%02d:%02d:%02d",  hora, minuto, segundo);

    // Indica o horário no LCD
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);
    Lcd_Out(1,2,"IAGO && RENATA");
    Lcd_Out(2, 5, str);
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

    // 1s
    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet());

    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerEnable(TIMER0_BASE, TIMER_A);
    TimerIntRegister(TIMER0_BASE, TIMER_A, Interrupcao_Timer0);
}

/********************************************************************************
 *
 * Função para configurar os dados iniciais
 *
 ********************************************************************************/
void configureData() {

    // Varre todas as posições e zera
    int i;
    for(i = 0; i < NUM_REMEDIOS; i++) {
        remedios[i].horario = 0;
        remedios[i].dose = 0;
        remedios[i].intervalo = 0;
        remedios[i].ocupado = 0;
        remedios[i].estoque = 0;
        remedios[i].tomar = 0;
    }

    // Esta posição não está disponível para adicionar remédios
    remedios[0].ocupado = 1;
}

/********************************************************************************
 *
 * Função para configurar o motor de passo
 *
 ********************************************************************************/
void configureStepMotor() {
    // Habilita o periférico D
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    // Espera até estar pronto
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB));

    // Configura os pinos D0-D3 como saída
    GPIOPinTypeGPIOOutput(PORT_STEP_MOTOR, STEP_MOTOR);
}

/********************************************************************************
 *
 * Função que gira o motor até a posição especificada
 *
 ********************************************************************************/
void setMotorTo(int position) {
    // Variável que indica quanto o motor deve girar
    int move;

    // Verifica se o destino está para frente da posição atual
    if(position > currentPosition) {
        // Calcula o quanto deve andar
        move = position - currentPosition;

        // Se for até meia volta gira no sentido anti horário
        if(move <= 100) {
            spinCounterclockwise(move);
        } else {
            // Calcula quanto deve andar no sentido horário
            move = 200 - move;
            spinClockwise(move);
        }

    // Verifica se o destino está para trás da posição atual
    } else if(position < currentPosition) {
        // Calcula o quanto deve andar
        move = currentPosition - position;

        // Se for até meia volta gira no sentido horário
        if(move < 100) {
            spinClockwise(move);
        } else {
            // Calcula o quanto deve andar no sentido horário
            move = 200 - move;
            spinCounterclockwise(move);
        }
    }

    // Atualiza a posição atual
    currentPosition = position;
}

/********************************************************************************
 *
 * Função que gira o motor no sentido horário
 *
 ********************************************************************************/
void spinClockwise(int steps) {

    // Lê qual pino do motor está ativado
    int currentPin = GPIOPinRead(PORT_STEP_MOTOR, STEP_MOTOR);

    // Variável para calibrar de onde começar a girar baseado no pino ativo
    int bias;

    // Regula bias em função de currentPin
    switch(currentPin) {
    case 0x01:
        bias = 0;
        break;
    case 0x02:
        bias = 3;
        break;
    case 0x04:
        bias = 2;
        break;
    case 0x08:
        bias = 1;
        break;
    default:
        bias = 0;
        break;
    }

    // Gira o motor no sentido horário
    int i;
    for(i = 0 + bias; i < steps + bias; i++) {
        GPIOPinWrite(PORT_STEP_MOTOR, STEP_MOTOR, 0x08 >> (i % 4));
        delay_ms(10);
    }
}

/********************************************************************************
 *
 * Função que gira o motor no sentido anti horário
 *
 ********************************************************************************/
void spinCounterclockwise(int steps) {

    // Lê qual pino do motor está ativado
    int currentPin = GPIOPinRead(PORT_STEP_MOTOR, STEP_MOTOR);

    // Variável para calibrar de onde começar a girar baseado no pino ativo
    int bias;

    // Regula bias em função de currentPin
    switch(currentPin) {
    case 0x01:
        bias = 1;
        break;
    case 0x02:
        bias = 2;
        break;
    case 0x04:
        bias = 3;
        break;
    case 0x08:
        bias = 0;
        break;
    default:
        bias = 0;
        break;
    }

    // Gira o motor no sentido anti horario
    int i;
    for(i = 0 + bias; i < steps + bias; i++) {
        GPIOPinWrite(PORT_STEP_MOTOR, STEP_MOTOR, 0x01 << (i % 4));
        delay_ms(10);
    }
}

/********************************************************************************
 *
 * Função que verifica se algum remédio deve ser tomado
 *
 ********************************************************************************/
void checkRemedios() {

    // Varre os remédios para verificar se algum deve ser tomado
    int i;
    for(i = 1; i < NUM_REMEDIOS; i++) {
        if(remedios[i].ocupado && remedios[i].tomar) {
            delay_ms(1000);
            tomarRemedio(i);
        }
    }

    // Varre os remédios para verificar o estoque
    for(i = 1; i < NUM_REMEDIOS; i++) {
        // Se houver remédio cadastrado na posição i com estoque igual zero
        if(remedios[i].ocupado && remedios[i].estoque == 0) {
            // Indica que o alarme deve ser ligado
            alarmeEstoqueOn = 1;
            // Sai do loop
            break;
        } else if(i == NUM_REMEDIOS - 1) {
            // Se chegar aqui significa que todos estão ok, desliga o alarme
            alarmeEstoqueOn = 0;
        }
    }
    // Gira o motor para posição inicial
    setMotorTo(POS0);
}

/********************************************************************************
 *
 * Função que executa as operações para tomar um remédio
 *
 ********************************************************************************/
void tomarRemedio(int position) {

    // Textos para mostrar no LCD
    uint8_t str1[16],str2[16];

    // Indica qual remédio tomar
    sprintf(str1, "TOMAR REMEDIO %d", position);

    // Indica quantos comprimidos tomar
    sprintf(str2, "%d COMPRIMIDOS", remedios[position].dose);

    // Escreve no LCD
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);
    Lcd_Out(1,2,str1);
    Lcd_Out(2,3,str2);

    // Indica que o alarme deve ser ligado
    alarmeRemedioOn = 1;

    // Gira o motor para o compartimento
    setMotorTo(posicao[position]);

    // Variável que indica se SW2 está pressionado
    int sw2Pressed;

    do {
        // Lê se SW2 está pressionado
        sw2Pressed = !GPIOPinRead(PORT_SW2, SW2);

    // Enquanto não for pressionado
    } while(!sw2Pressed);

    // Desativa o alarme
    alarmeRemedioOn = 0;

    // Variável que contém o novo número de comprimidos em estoque
    int estoque = remedios[position].estoque - remedios[position].dose;

    // Se estoque ficar negativo, zera
    if(estoque < 0) 
        estoque = 0;

    // Atualiza o estoque
    remedios[position].estoque = estoque;

    // Atualiza o horário que deve ser tomado
    remedios[position].horario = updateTime(tempo, remedios[position].intervalo);

    // Remove o status de que deve ser tomado
    remedios[position].tomar = 0;
}

/********************************************************************************
 *
 * Função para configurar o alarme
 *
 ********************************************************************************/
void setupAlarme() {

    // Habilita e espera o acesso ao PORTF
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));

    // Configura os pinos do LED RGB como saídas
    GPIOPinTypeGPIOOutput(PORT_RGB, LED_RGB);
}

/********************************************************************************
 *
 * Função main
 *
 ********************************************************************************/
int main(void) {

    // Habilita clock geral do sistema para rodar em 50 MHz
    SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL
            |SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    // Configura a comunicação UART
    ConfigureUART();

    // Configura o Timer
    setupTimer();

    // Configura o alarme
    setupAlarme();

    // Configura o display LCD
    LCD_Setup();

    // Configura a hora
    Configura_hora();

    // Configura o uso dos botões SW1 e SW2
    ButtonsInit();

    // Configura os dados necessários 
    configureData();

    // Configura o motor de passo
    configureStepMotor();
    
    // Variável que indica se SW1 está pressionado
    uint32_t sw1Pressed;

    while(1) {
        // Mostra as horas no LCD
        Imprime_hora();

        // Verifica se o botão 1 foi pressionado
        sw1Pressed = !GPIOPinRead(PORT_SW1, SW1);

        // Se pressionado chama a função menu
        if(sw1Pressed) 
            Menu();

        // Verifica se algum remédio deve ser tomado
        checkRemedios();

        // Espera 50 ms
        delay_ms(50);
    }
}
