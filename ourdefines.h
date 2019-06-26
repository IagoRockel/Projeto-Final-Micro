/********************************************************************************
 * ourdefines.h
 *
 * Criado por: Iago Rockel e Renata Costa
 * Atualizado em: 26/06/2019
 *
 * Header file que contem as constantes a serem utilizadas nas atividades
 * praticas da disciplina de sistemas microcontrolados, com o intuito de 			
 * facilitar a programação dos códigos.					
 ********************************************************************************/

#ifndef OURDEFINES_H_
#define OURDEFINES_H_

/********************************************************************************
 * LED RGB
 ********************************************************************************/

#define LED_D 0x00 		// LED desligado
#define LED_R 0x02 		// LED vermelho  	pino 1 	-> 0b0000 0010
#define LED_B 0x04 		// LED azul 		pino 2 	-> 0b0000 0100
#define LED_G 0x08 		// LED verde		pino 3 	-> 0b0000 1000
#define LED_RGB 0x0E	// Pinos 1, 2 ,3 			-> 0b0000 1110

#define PORT_RGB GPIO_PORTF_BASE // Porta do LED 

/********************************************************************************
 * LEDs Placa DAELT
 ********************************************************************************/

#define PORT_LEDS GPIO_PORTB_BASE // Porta dos LEDs placa tiva

#define LEDS_ENABLE  0x01 // Transistor dos LEDs pino 0 -> 0b0000 0001
#define LEDS_DISABLE 0x00 // Desliga os LEDs

#define PORT_LEDS_ENABLE GPIO_PORTE_BASE // Porta do transistor dos LEDs

/********************************************************************************
 * Botoes
 ********************************************************************************/

#define SW1 0x10 // Pino 4 -> 0b0001 0000 
#define SW2 0x01 // Pino 0 -> 0b0000 0001

#define PORT_SW1 GPIO_PORTF_BASE // Porta SW1
#define PORT_SW2 GPIO_PORTF_BASE // Porta SW2

/********************************************************************************
 * Display 7 segmentos
 ********************************************************************************/

#define DS1_ENABLE 0x40 // Transistor dos LEDS do display 1 pino 6 -> 0b0100 0000 
#define DS2_ENABLE 0x10 // Transistor dos LEDS do display 2 pino 4 -> 0b0001 0000 

#define PORT_DS1_ENABLE GPIO_PORTD_BASE // Porta DS1
#define PORT_DS2_ENABLE GPIO_PORTF_BASE // Porta DS2

#define PORT_D_LED GPIO_PORTB_BASE // Porta LEDs display

#define D0 0x3F // Decodificacao do numero 0
#define D1 0x06 // Decodificacao do numero 1 
#define D2 0x5B // Decodificacao do numero 2
#define D3 0x4F // Decodificacao do numero 3
#define D4 0x66 // Decodificacao do numero 4
#define D5 0x6D // Decodificacao do numero 5
#define D6 0x7D // Decodificacao do numero 6
#define D7 0x07 // Decodificacao do numero 7
#define D8 0x7F // Decodificacao do numero 8
#define D9 0x6F // Decodificacao do numero 9
#define DA 0x77 // Decodificacao da letra 'A'
#define DB 0x7F // Decodificacao da letra 'b'
#define DC 0x7F // Decodificacao da letra 'C'
#define DD 0x7F // Decodificacao da letra 'd'
#define DE 0x79 // Decodificacao da letra 'E'
#define DF 0x71 // Decodificacao da letra 'F'

/********************************************************************************
 * Motor de passo
 ********************************************************************************/

#define PORT_STEP_MOTOR GPIO_PORTD_BASE // Porta do motor de passo

#define STEP_MOTOR 0x0F // Pinos 0, 1, 2, 3 

/********************************************************************************
 * Projeto Final
 ********************************************************************************/

#define NUM_REMEDIOS 6 // Numero de compartimentos da estrutura dos remedios

#define HORA 3600 // Equivalente em segundos 
#define MINUTO 60 // Equivalente em segundos
#define SEGUNDO 1 // Equivalente em segundos
#define DEZENA 10 // Equivalente algebrico
#define UNIDADE 1 // Equivalente algebrico

unsigned char tempo;

typedef struct tipo_remedio {
    int posicao; // compartimento em que o remédio será alocado
    int tempo // tempo para tomar o remédio
    int comprimido; // quantidade de comprimidos em uma dose
    int intervalo; // intervalo em horas entre as doses
    int disponivel; // indica se o compartimento esta disponivel
} Remedios;

#endif // OURDEFINES_H_