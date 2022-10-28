#include <msp430.h>

void timer_Wait(){
    TA0R = 0;         //Pone la cuenta en 0
    while(TA0R <= 1000){} //Espera hasta el timer cuente hasta 1000 o 1 ms
}

void timer_Wait_ms(int retardo){
    int i;
    for (i=0;i<=retardo;i++)
        timer_Wait();
}

void timer_Init(void){
    CSCTL0_H = CSKEY >> 8;                    // Desbloquea los registros del reloj
    CSCTL1 = DCOFSEL_0 | DCORSEL;             //configura el DCO Digitally controlled Oscillator 1 MHz
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK; //Selecciona las fuentes de reloj para cada una de las señales de reloj
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;     // Configura el divisor para todas las señales de reloj en 1
    CSCTL0_H = 0;

    TA0CTL = TACLR; //Limpua el timer
    TA0CTL = TASSEL__SMCLK | MC__UP; //Configura SMCLK como fuente de reloj, para el timer.
    TA0CCR0 = 50000; //Valor de recarga. La bandera se dispara de 50 ms
}
