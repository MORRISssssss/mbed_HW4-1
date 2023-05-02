#include "mbed.h"
#include "TextLCD.h"
#include <cstdint>
#include "drivers/DigitalOut.h"
#include "erpc_simple_server.hpp"
#include "erpc_basic_codec.hpp"
#include "erpc_crc16.hpp"
#include "UARTTransport.h"
#include "DynamicMessageBufferFactory.h"
#include "textLCD_erpc_head_server.h"

// I2C Communication
I2C i2c_lcd(D14, D15); // SDA, SCL
TextLCD_I2C lcd(&i2c_lcd, 0x4E, TextLCD::LCD16x2);

/****** erpc declarations *******/

void Locate(uint8_t col, uint8_t row) {
    printf("locate(%d, %d)\r\n", col, row);
    lcd.locate(col, row);
}

void Cls() {
    lcd.cls();
    printf("cls()\r\n");
}

uint8_t Rows() {
    uint8_t rows;
    rows = lcd.rows();
    printf("rows() = %d\r\n", rows);
    return rows;
}

uint8_t Columns() {
    uint8_t cols;
    cols = lcd.columns();
    printf("columns() = %d\r\n", cols);
    return cols;
}

uint8_t Putc(uint8_t c) {
    printf("putc(%c)\r\n", c);
    return lcd.putc(c);
}

/** erpc infrastructure */
ep::UARTTransport uart_transport(D1, D0, 9600);
ep::DynamicMessageBufferFactory dynamic_mbf;
erpc::BasicCodecFactory basic_cf;
erpc::Crc16 crc16;
erpc::SimpleServer rpc_server;

/** LED service */
textLCDService_service textLCD_service;




int main(void) {
    lcd.setCursor(TextLCD::CurOff_BlkOn);
    lcd.printf("start");

    // Initialize the rpc server
    uart_transport.setCrc16(&crc16);

    printf("Initializing server.\n");
    rpc_server.setTransport(&uart_transport);
    rpc_server.setCodecFactory(&basic_cf);
    rpc_server.setMessageBufferFactory(&dynamic_mbf);

    // Add the textLCD service to the server
    printf("Adding textLCD server.\n");
    rpc_server.addService(&textLCD_service);

    // Run the server. This should never exit
    printf("Running server.\n");
    rpc_server.run();
}