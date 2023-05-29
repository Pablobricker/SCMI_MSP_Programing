#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include "eUSCIA1_UART.h"
#include "STMF407xx_bootloaderCommands.h"
#include "eUSCIB0_SPI.h"
#include  "FRAM_commands.h"
#include "TIMERA0.h"
#define Flash_Begin 0x0800


uint16_t REad_Bff[100]={0};


uint8_t Frame_Verify_Checksum(uint8_t data[], bool in_nout){
    uint8_t i = 0;
    uint8_t local_checksum = 0;
    uint8_t received_checksum = 0;
    if(in_nout){
    for(i = 0; i<= data[0]+2; i++){
        local_checksum ^=  data[i];
    }
    received_checksum = data[data[0]+3];
    }
    else{
        for(i = 0; i<= data[0]; i++){//Asi si toma en cuenta el numero de datos para el calculo del checksum
                local_checksum ^=  data[i];
            }
            received_checksum = data[data[0]+1];
    }
    if(local_checksum == received_checksum){
        return 1;
    }else{
        return 0;
    }
}
void masterReprogramationRutine(uint32_t FRAM_initialAddress, uint32_t Flash_initialAddress1, int Rx_ready_buffers){
    uint32_t FRAM_actualAddress = FRAM_initialAddress;
    uint32_t Flash_actualAddress = Flash_initialAddress1;

    //Esta funcion asume que el respaldo del programa del master ya ha sido cargado en la FRAM.

    uint16_t FRAM2MSP_VB[100]; //Vector de Buffer de FRAM ---> MSP
    uint8_t MSP2Master_VB_CHK[100];//Vector de Buffer de MSP ---> Master
                                //Truncados a 25 bytes
    uint32_t MSP2Master_VB[100];//Vector de Buffer de MSP ---> Master

    //int FRAMvectorBufferSize = sizeof(FRAMvectorBuffer)/sizeof(FRAMvectorBuffer[0]);
    unsigned int i;


    ACK= BootloaderAccess();
    eeraseCommand(0);       //Region de Flash para hacer pruebas (sector 7 0x08060000) en la nucleo F446RE

    for (i=0; i<Rx_ready_buffers; i++){
        //Lectura de frames almacenados en FRAM

        uint16_t BFFSZ_HX;  //Numero datos formato 16-bit
            FRAM_read(0x00,((FRAM_actualAddress)>>8)&0xFF, FRAM_actualAddress&0xFF,&BFFSZ_HX, 1);
            //FRAM_actualAddress++;
            int BufferVectorSize = (int) BFFSZ_HX;  //Numero de datos formato int

        FRAM_read(0x00,((FRAM_actualAddress)>>8)&0xFF, FRAM_actualAddress & 0xFF, FRAM2MSP_VB, BufferVectorSize+2);
        //Ejecutar alguna rutina para verificar la integridad de los datos (No se tiene que desarrollar ahora).
        //Hacer la conversion de 16 a 8 bits para que se puedan enviar bien los datos
        //Para mas optimizazion modificar las funciones de escritura y lectura de la FRAM a 8 bits
        //Formato de 16-bit necesario para que funcionen los  Drivers de SPI. La FIFO de rececpion SPI es de 16 bits

        //Conversion de formato para compatibilidad con el UART del maestro 16-->8-bit
        unsigned int j;
        for (j=0; j<=BufferVectorSize+2; j++){
                    MSP2Master_VB_CHK[j] = FRAM2MSP_VB[j]&0xFF;}
        if (Frame_Verify_Checksum(MSP2Master_VB_CHK,false) == 1){
            for (j=0; j<=BufferVectorSize; j++){
                        MSP2Master_VB[j] = MSP2Master_VB_CHK[j+1];}
            //Escritura UART hacia la Flash
        writeMemoryCommand(0x0800,(Flash_actualAddress)&0xFFFF , MSP2Master_VB, BufferVectorSize);
        FRAM_actualAddress = FRAM_actualAddress + BufferVectorSize+2;       //El +2 es para saltar el checksum y el numero de datos
        Flash_actualAddress = Flash_actualAddress + BufferVectorSize;
        }
        else {i=i-1;}
    }

}



main(){
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
        //PM5CTL0 &= ~LOCKLPM5;

        P1_Init();                  //Habilita pines GPIO para el patron de acceso al bootloader
        timer_Init();               //Habilita un timer para el patron de acceso al bootloader
        eUSCIA1_UART_Init();        //Habilita comunicacion UART para conectar con Flash
        eUSCIB0_SPI_init();         //Habilita comunicacion SPI para conectar con FRAM

       //ACK = BootloaderAccess();

       //eeraseCommand(0);

       //FRAM_erase(0x00,0x00,0x00,2000);
        //FRAM_read(0x00,0x3B,0xBF,REad_Bff,40);//09
        //FRAM_read(0x00,0x00,0x22,REad_Bff,34);
        //FRAMREPROG guarda un cheksum que toma en calculo el numero de datos, no solo los datos
        masterReprogramationRutine(0x0000,0x0000,36);//mandar solo los primeros 16 bits a pesar de que diga que soporta de 32 bits
        goCommand(0x0800,0x0000);
        while(1);
}
