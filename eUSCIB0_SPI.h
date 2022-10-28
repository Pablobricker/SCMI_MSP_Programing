#include <msp430.h>

//Pruebaaaa

int eUSCIB0_SPI_readAvailable = 0;
int eUSCIB0_writeAvailable = 0;
int datosRecibidos[30];
int n = 0;
int interrupcion = 0;

void eUSCIB0_CS1_set_state(int a);

void eUSCIB0_SPI_init(){
    UCB0CTLW0 = UCSWRST; //resetea el modulo USCI
    UCB0CTLW0 |= UCSSEL__SMCLK; //Selecciona SMCLK como fuente de reloj (1 MHz)
    UCB0CTLW0 |= UCSYNC; //Configura modo sincrono.
    UCB0CTLW0 |=  UCMODE_0; //Se configura para 3 wire SPI.
    UCB0CTLW0 |= UCMST; //Se configura en modo maestro.
    UCB0CTLW0 |= UCMSB; //Configura MSB primero
    UCB0CTLW0 |= UCCKPH;
    //Configuraci�n de la velocidad
    UCB0BRW = 0; //Velocidad de SMCLK, 1 Mbit/s


    //configura puertos
    PM5CTL0 &= ~LOCKLPM5;
    //Configuraci�n de los GPIO
    P1SEL1 |= BIT6 | BIT7;             //Configura SOMI, MISO //La configuraci�n de puertos esta bien
    P2SEL1 |= BIT2;                    //UCB0CLK

    P1DIR |= BIT0; //Configura P1.0 (LED) como salida;
    P1DIR |= BIT2; //Configura P1.2 como salida (CS)

    P1OUT &= ~BIT0; //Apaga LED.
    P1OUT |= BIT2; // (1.2) CS esta en alto, el esclavo esta desactivado.



    UCB0IE |= UCRXIE | UCTXIE; //Habilita la interrupcion de recepcion y transmision.
    __enable_interrupt(); //Habilita las interrupciones enmascarables.


    UCB0CTLW0 &= ~UCSWRST; //Pone a funcionar el modulo eUSCIB
}

void eUSCIB0_SPI_writeByte(int dato){
    while((UCB0STATW & UCBUSY) == UCBUSY){} //esperar mientras el buffer de escritura no este vac�o.
    UCB0TXBUF = dato;
    while((UCB0STATW & UCBUSY) == UCBUSY){}
}

int eUSCIB0_SPI_readByte(){
    int dato = 0;
    while((UCB0STATW & UCBUSY) == UCBUSY){} //esperar mientras el buffer de escritura no este lleno.
    dato = UCB0RXBUF;
    while((UCB0STATW & UCBUSY) == UCBUSY){} //esperar mientras el buffer de escritura no este lleno.
    eUSCIB0_SPI_readAvailable = 0;
    return dato;
}

void eUSCIB0_CS1_set_state(int a){
    if (a==0) P1OUT &= ~BIT2; //CS = LOW
    if (a==1) P1OUT |= BIT2; //CS = HIGH
}

#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void){
    //Se levanta la bandera UCRXIFG cuando UCB0RXBUF ha recibido un caracter completo.
    if ((UCB0IFG & UCRXIFG) == UCRXIFG){
        UCB0IFG &= ~UCRXIFG;        // Limpia bandera RX de SPI.
        datosRecibidos[n] = UCB0RXBUF;
        eUSCIB0_SPI_readAvailable = 1;
        n++;
    }

    //Se levanta la bandera UCTXIFG cuando UCB0RXBUF esta vacio
    if ((UCB0IFG & UCTXIFG) == UCTXIFG){
        UCB0IFG &= ~UCTXIFG;        // Limpia bandera TX de SPI.
        eUSCIB0_writeAvailable = 1;         // Entonces se puede escribir en el buffer UCB0RXBUF
        interrupcion++;
    }
}
