/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "mbed.h"
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "RingBuffer.h"
#include "command.h"
#include "Protocol.h"
#include "ProtocolID.h"

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

__IO uint8_t ubButtonPress = 0;
uint8_t rx_data;

JOGJointParams gJOGJointParams;
JOGCommonParams gJOGCommonParams;
JOGCmd gJOGCmd;

PTPCoordinateParams gPTPCoordinateParams;
PTPCommonParams gPTPCommonParams;
PTPCmd gPTPCmd;

HOMEParams gHOMEParams;
HOMECmd gHOMECmd;
EndEffectorParams gENDEFFECTORParams;
EMotor gEMotorParams;
Sensor gIRSensorParams;
Sensor gColorSensorParams;

uint64_t gQueuedCmdIndex;

uint8_t buttonPressed = 1;

Packet *packet;

void InitRAM(void);
bool waitForAnswer(bool *flag, bool answer, uint32_t timeout);
/* USER CODE END PFP */

// Use  USART3: TX:PC_10  RX:PC_11
//      USART1: TX:PA_9 RX: PA_10
UnbufferedSerial dobot_serial(PA_9, PA_10);

Ticker processTicker;

DigitalOut led1(LED1);

void on_rx_interrupt()
{
    dobot_serial.read(&rx_data, 1);

    if (RingBufferIsFull(&gUART4ProtocolHandler.rxRawByteQueue) == false) {
        RingBufferEnqueue(&gUART4ProtocolHandler.rxRawByteQueue, &rx_data);
    }
}

void tickerFunction()
{
    
    led1= !led1;
    // Send and receive commands/messages
    ProtocolProcess();
}

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  uint32_t j = 0;

  

    dobot_serial.baud(115200);
    dobot_serial.format(
        /* bits */ 8,
        /* parity */ SerialBase::None,
        /* stop bit */ 1
    );
  dobot_serial.attach(&on_rx_interrupt, SerialBase::RxIrq);
  
  ProtocolInit();
  InitRAM();
  // start timer after ProtocolInit();
  processTicker.attach(&tickerFunction, 1s);
  // ProtocolProcess() is called in tim2 callback

  while (1)
  {
    
	  if(buttonPressed == 1){

		  buttonPressed = 0;

		  SetPTPCoordinateParams(&gPTPCoordinateParams, true, &gQueuedCmdIndex);

		  SetPTPCommonParams(&gPTPCommonParams, true, &gQueuedCmdIndex);

		  SetEndEffectorParams(&gENDEFFECTORParams, true, &gQueuedCmdIndex);
	  	  SetEndEffectorSuctionCup(false, true, &gQueuedCmdIndex);
	  	  SetInfraredSensor(&gIRSensorParams, true);
	  	  SetColorSensor(&gColorSensorParams, true);

	  	  SetHOMECmd(&gHOMECmd, true, &gQueuedCmdIndex);

	  	  ThisThread::sleep_for(25s);

	  	  while(1){

	  		  if((j % 2) == 0){
	  			  gPTPCmd.x += 50;
	  			  SetPTPCmd(&gPTPCmd, true, &gQueuedCmdIndex);
	  			  SetEndEffectorSuctionCup(true, true, &gQueuedCmdIndex);
	  			  SetEMotor(&gEMotorParams, 0, true, &gQueuedCmdIndex);
	  		  }
	  		  else{
	  			  gPTPCmd.x -= 50;
	  			  SetPTPCmd(&gPTPCmd, true, &gQueuedCmdIndex);
	  			  SetEndEffectorSuctionCup(false, true, &gQueuedCmdIndex);

	  			  GetInfraredSensor(gIRSensorParams.port);
	  			  if(waitForAnswer(&irSensorAnswer, true, 500) == true){
	  				  irSensorAnswer = false;
	  				  if(gIRRxMessage.params[0] == 1){
	  					  SetEMotor(&gEMotorParams, 50, true, &gQueuedCmdIndex);
	  				  }
	  			  }

	  			  GetColorSensor();
	  			  if(waitForAnswer(&colorSensorAnswer, true, 500) == true){
	  				  colorSensorAnswer = false;
	  				  if(gColorRxMessage.params[0] == 1){
	  					  // red detected
	  				  }
	  				  ThisThread::sleep_for(1ms);
	  			  }
	  		  }

	  		  j++;

	  		  if(buttonPressed == 1){
	  			  buttonPressed = 0;
	  			  SetEndEffectorSuctionCup(false, true, &gQueuedCmdIndex);
	  			  SetEMotor(&gEMotorParams, 0, true, &gQueuedCmdIndex);
	  			  break;
	  		  }

	  		  // Time for Dobot to move
	  		  ThisThread::sleep_for(3s);
	  	  }
	  }
  }
}

/*********************************************************************************************************
** Function name:       waitForAnswer
** Descriptions:        Compares a flag with a answer for an max. amount of time
** Input parameters:    Reference of flag to be checked, answer to be compared with flag and timeout-value im ms
** Output parameters:   none
** Returned value:      true (flag == answer); false if timeout
*********************************************************************************************************/
bool waitForAnswer(bool *flag, bool answer, uint32_t timeout){

	uint32_t endTime = get_ms_count()+timeout;

	while(*flag != answer){
		if(endTime < get_ms_count()){
			return false;
		}
	}

	return true;
}


/*********************************************************************************************************
** Function name:       InitRAM
** Descriptions:        Initializes a global variable
** Input parameters:    none
** Output parameters:   none
** Returned value:      none
*********************************************************************************************************/
void InitRAM(void)
{
    gJOGJointParams.velocity[0] = 100;
    gJOGJointParams.velocity[1] = 100;
    gJOGJointParams.velocity[2] = 100;
    gJOGJointParams.velocity[3] = 100;
    gJOGJointParams.acceleration[0] = 80;
    gJOGJointParams.acceleration[1] = 80;
    gJOGJointParams.acceleration[2] = 80;
    gJOGJointParams.acceleration[3] = 80;

    gJOGCommonParams.velocityRatio = 50;
    gJOGCommonParams.accelerationRatio = 50;

    gJOGCmd.cmd = 0;
    gJOGCmd.isJoint = true;

    gPTPCoordinateParams.xyzVelocity = 200;
    gPTPCoordinateParams.rVelocity = 200;
    gPTPCoordinateParams.xyzAcceleration = 200;
    gPTPCoordinateParams.rAcceleration = 200;

    gPTPCommonParams.velocityRatio = 200;
    gPTPCommonParams.accelerationRatio = 200;

    gPTPCmd.ptpMode = JUMP_XYZ;
    gPTPCmd.x = 100.8772;
    gPTPCmd.y = 7.5856;
    gPTPCmd.z = -0.6136;
    gPTPCmd.r = 1.8265;

    gQueuedCmdIndex = 0;

    gHOMEParams.r = 0;
    gHOMEParams.x = 0.0;
    gHOMEParams.y = 0.0;
    gHOMEParams.z = 0.0;

    gENDEFFECTORParams.xBias = 0;
    gENDEFFECTORParams.yBias = 0;
    gENDEFFECTORParams.zBias = 0;

    gEMotorParams.index = 0; // Stepper 1
    gEMotorParams.isEnabled = true;
    gEMotorParams.speed = 0;

    gIRSensorParams.isEnable = 1;
    gIRSensorParams.version = (uint8_t)VER_V2;
    gIRSensorParams.port = (uint8_t)IF_PORT_GP4;

    gColorSensorParams.isEnable = 1;
    gColorSensorParams.port = (uint8_t)CL_PORT_GP2;
    gColorSensorParams.version = (uint8_t)VER_V2;

}



/*void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == huart1.Instance){
		if (RingBufferIsEmpty(&gUART4ProtocolHandler.txRawByteQueue) == false) {
			uint8_t data;
			RingBufferDequeue(&gUART4ProtocolHandler.txRawByteQueue, &data);
			HAL_UART_Transmit_IT(&huart1, &data, sizeof (data));
		}
	}else if(huart->Instance == huart2.Instance){

	}

}*/

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
