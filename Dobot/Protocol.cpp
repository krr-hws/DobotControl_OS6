/****************************************Copyright(c)*****************************************************
**                            Shenzhen Yuejiang Technology Co., LTD.
**
**                                 http://www.dobot.cc
**
**--------------File Info---------------------------------------------------------------------------------
** File name:           Protocol.cpp
** Latest modified Date:2016-06-01
** Latest Version:      V1.0.0
** Descriptions:        Protocol interface
**
**--------------------------------------------------------------------------------------------------------
** Created by:          Liu Zhufu
** Created date:        2016-03-14
** Version:             V1.0.0
** Descriptions:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "stm32f4xx.h"
#include "Protocol.h"
#include <stdio.h>
#include <string.h>
#include "stm32f4xx_hal_uart.h"
#include "main.h"
#include "command.h"
#include "ProtocolID.h"
#include "mbed.h"

/*********************************************************************************************************
** Protocol buffer definition
*********************************************************************************************************/
#define RAW_TX_BYTE_BUFFER_SIZE    1024
#define RAW_RX_BYTE_BUFFER_SIZE    4096
#define RAW_BYTE_BUFFER_SIZE    4096
#define PACKET_BUFFER_SIZE  8

extern UnbufferedSerial dobot_serial;

// UART4
uint8_t gUART4TXRawByteBuffer[RAW_BYTE_BUFFER_SIZE];
uint8_t gUART4RXRawByteBuffer[RAW_BYTE_BUFFER_SIZE];
Packet gUART4TXPacketBuffer[PACKET_BUFFER_SIZE];
Packet gUART4RXPacketBuffer[PACKET_BUFFER_SIZE];

ProtocolHandler gUART4ProtocolHandler;
Message gIRRxMessage;
Message gColorRxMessage;


/*********************************************************************************************************
** Function name:       ProtocolInit
** Descriptions:        Init the protocol buffer etc.
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/
void ProtocolInit(void)
{
    // Init UART4 protocol
    RingBufferInit(&gUART4ProtocolHandler.txRawByteQueue, gUART4TXRawByteBuffer, RAW_BYTE_BUFFER_SIZE, sizeof(uint8_t));
    RingBufferInit(&gUART4ProtocolHandler.rxRawByteQueue, gUART4RXRawByteBuffer, RAW_BYTE_BUFFER_SIZE, sizeof(uint8_t));
    RingBufferInit(&gUART4ProtocolHandler.txPacketQueue, gUART4TXPacketBuffer, PACKET_BUFFER_SIZE, sizeof(Packet));
    RingBufferInit(&gUART4ProtocolHandler.rxPacketQueue, gUART4RXPacketBuffer, PACKET_BUFFER_SIZE, sizeof(Packet));

}

/*********************************************************************************************************
** Function name:       ProtocolProcess
** Descriptions:        Process the protocol
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/
void ProtocolProcess(void)
{
    Message message;
    MessageProcess(&gUART4ProtocolHandler);
    while(RingBufferGetCount(&gUART4ProtocolHandler.txRawByteQueue) != 0){
    //if (RingBufferGetCount(&gUART4ProtocolHandler.txRawByteQueue)) {
    	uint8_t data;
    	RingBufferDequeue(&gUART4ProtocolHandler.txRawByteQueue, &data);
        dobot_serial.write(&data, 1);
    	//HAL_UART_Transmit_IT(&huart1, &data, sizeof (data));
    }

    if(MessageRead(&gUART4ProtocolHandler, &message)==ProtocolNoError)
    {
    	if(message.id == ProtocolIRSwitch && message.rw == 0){
    		irSensorAnswer = true;
    		gIRRxMessage = message;
    	}
    	if(message.id == ProtocolColorSensor && message.rw == 0){
    		colorSensorAnswer = true;
    	    gColorRxMessage = message;
    	}

    }
}
