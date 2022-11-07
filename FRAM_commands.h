#include <stdint.h>
#define WREN   0x06
#define WRDI   0x04
#define RDSR   0x05
#define WRSR   0x01
#define READ   0x03
#define FSTRD  0x0B
#define WRITE  0x02
#define SLEEP  0xB9
#define RDID   0x9F

//Probando que git funcione bien esto es la ramab 

//MSP SPI-MOSI P1.6 <-> 51 o ICSP-4
//MSP SPI-MISO P.1.7 <-> 50 o ICSP-1
//MSP SPI-CLK P2.2 <-> 52 o ICSP-3
//MSP SPI-SS P1.3 <-> 53 o

int FRAM_dataW[]={0x05,0x08,0x2,0x03};
uint32_t FRAM_dataR[4];

void FRAM_write(int ADDRESS_1,int ADDRESS_2,int ADDRESS_3,int Nbytes){
    unsigned int i;
    eUSCIB0_CS1_set_state(0); //CS LOW
    eUSCIB0_SPI_writeByte(WREN);
    eUSCIB0_CS1_set_state(1);

    //_delay_cycles(100);

    eUSCIB0_CS1_set_state(0);
    eUSCIB0_SPI_writeByte(WRITE);
    eUSCIB0_SPI_writeByte(ADDRESS_1);
    eUSCIB0_SPI_writeByte(ADDRESS_2);
    eUSCIB0_SPI_writeByte(ADDRESS_3);
    for(i=0;i<=Nbytes;i++){
        eUSCIB0_SPI_writeByte(FRAM_dataW[i]);
    }
    eUSCIB0_CS1_set_state(1);
}

void FRAM_read(int ADDRESS_1,int ADDRESS_2,int ADDRESS_3,int Nbytes){
    unsigned int i;
    eUSCIB0_CS1_set_state(0);
    eUSCIB0_SPI_writeByte(READ);
    eUSCIB0_SPI_writeByte(ADDRESS_1);
    eUSCIB0_SPI_writeByte(ADDRESS_2);
    eUSCIB0_SPI_writeByte(ADDRESS_3);
    for(i=0;i<=Nbytes;i++){
        FRAM_dataR[i] = eUSCIB0_SPI_readByte();
    }
    eUSCIB0_CS1_set_state(1);
}
