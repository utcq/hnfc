#ifndef CONF_H
#define CONF_H

/* 
  PINOUT:
    3.3V -> VCC
    RST  -> D5
    GND  -> GND
    IRQ  -> NONE
    MISO -> D12
    MOSI -> D11
    SCL  -> D13
    SDA  -> D10
*/
#define SS_PIN 10
#define RST_PIN 5

#define BAUD_RATE 9600

#define COMM_PREFIX 0x54
#define COMM_FULL_READ 0x51
#define COMM_READ 0x52

#endif