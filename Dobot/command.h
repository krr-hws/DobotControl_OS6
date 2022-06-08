#ifndef COMMAND_H
#define COMMAND_H

/****************************************Copyright(c)*****************************************************
**                            Shenzhen Yuejiang Technology Co., LTD.
**
**                                 http://www.dobot.cc
**
**--------------File Info---------------------------------------------------------------------------------
** File name:           command.h
** Latest modified Date:
** Latest Version:      V1.0.0
** Descriptions:
**
**--------------------------------------------------------------------------------------------------------
** Created by:          LiYi
** Created date:        2016-06-01
** Version:             V1.0.0
** Descriptions:        Data definition
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/

#include <stdint.h>
#include <stdbool.h>

/*********************************************************************************************************
** Data structure
*********************************************************************************************************/
#pragma pack(push)
#pragma pack(1)

enum
{
    JUMP_XYZ,
    MOVJ_XYZ,
    MOVL_XYZ,
    JUMP_ANGLE,
    MOVJ_ANGLE,
    MOVL_ANGLE,
    MOVJ_INC,
    MOVL_INC,
};

typedef enum tagColorPort{
    CL_PORT_GP1,
    CL_PORT_GP2,
    CL_PORT_GP4,
    CL_PORT_GP5
}ColorPort;

typedef enum tagInfraredPort{
    IF_PORT_GP1,
    IF_PORT_GP2,
    IF_PORT_GP4,
    IF_PORT_GP5
}InfraredPort;

typedef enum tagVersion{
	VER_V1,
	VER_V2
}DLL_Version;

typedef struct tagDevice{
	uint8_t isEnable;
	uint8_t port;
	uint8_t version;
}Sensor;

typedef struct tagEMotor {
    uint8_t index;
    uint8_t isEnabled;
    int32_t speed;
}EMotor;

/*
 * HOME related
 */
typedef struct tagHOMEParams {
    float x;
    float y;
    float z;
    float r;
}HOMEParams;

typedef struct tagHOMECmd {
    uint32_t reserved;
}HOMECmd;

typedef struct tagAutoLevelingCmd {
    uint8_t controlFlag;
    float precision;
}AutoLevelingCmd;

typedef struct tagEndEffectorParams {
    float xBias;
    float yBias;
    float zBias;
}EndEffectorParams;

typedef struct tagJOGJointParams {
    float velocity[4];
    float acceleration[4];
}JOGJointParams;

typedef struct tagJOGCoordinateParams {
    float velocity[4];
    float acceleration[4];
}JOGCoordinateParams;

typedef struct tagJOGCommonParams {
    float velocityRatio;
    float accelerationRatio;
}JOGCommonParams;

typedef struct tagJOGCmd {
    uint8_t isJoint;
    uint8_t cmd;
}JOGCmd;

typedef struct tagPTPJointParams {
    float velocity[4];
    float acceleration[4];
}PTPJointParams;

typedef struct tagPTPCoordinateParams {
    float xyzVelocity;
    float rVelocity;
    float xyzAcceleration;
    float rAcceleration;
}PTPCoordinateParams;

typedef struct tagPTPJumpParams {
    float jumpHeight;
    float maxJumpHeight;
}PTPJumpParams;

typedef struct tagPTPCommonParams {
    float velocityRatio;
    float accelerationRatio;
}PTPCommonParams;

typedef struct tagPTPCmd {
uint8_t ptpMode;
    float x;
    float y;
    float z;
    float r;
}PTPCmd;

#pragma pack(pop)


extern int GetDeviceName(void);

/*********************************************************************************************************
** HOME
*********************************************************************************************************/
extern int SetHOMEParams(HOMEParams *homeParams, bool isQueued, uint64_t *queuedCmdIndex);
extern int GetHOMEParams(void);
extern int SetHOMECmd(HOMECmd *homeCmd, bool isQueued, uint64_t *queuedCmdIndex);

/*********************************************************************************************************
** End effector function
*********************************************************************************************************/
extern int SetEndEffectorParams(EndEffectorParams *endEffectorParams, bool isQueued, uint64_t *queuedCmdIndex);
extern int SetEndEffectorLaser(bool on, bool isQueued, uint64_t *queuedCmdIndex);
extern int SetEndEffectorSuctionCup(bool suck, bool isQueued, uint64_t *queuedCmdIndex);
extern int SetEndEffectorGripper(bool grip, bool isQueued, uint64_t *queuedCmdIndex);

/*********************************************************************************************************
** JOG function
*********************************************************************************************************/
extern int SetJOGJointParams(JOGJointParams *jogJointParams, bool isQueued, uint64_t *queuedCmdIndex);
extern int SetJOGCoordinateParams(JOGCoordinateParams *jogCoordinateParams, bool isQueued, uint64_t *queuedCmdIndex);
extern int SetJOGCommonParams(JOGCommonParams *jogCommonParams, bool isQueued, uint64_t *queuedCmdIndex);
extern int SetJOGCmd(JOGCmd *jogCmd, bool isQueued, uint64_t *queuedCmdIndex);

/*********************************************************************************************************
** PTP function
*********************************************************************************************************/
extern int SetPTPJointParams(PTPJointParams *ptpJointParams, bool isQueued, uint64_t *queuedCmdIndex);
extern int SetPTPCoordinateParams(PTPCoordinateParams *ptpCoordinateParams, bool isQueued, uint64_t *queuedCmdIndex);
extern int SetPTPJumpParams(PTPJumpParams *ptpJumpParams, bool isQueued, uint64_t *queuedCmdIndex);
extern int SetPTPCommonParams(PTPCommonParams *ptpCommonParams, bool isQueued, uint64_t *queuedCmdIndex);
extern int SetPTPCmd(PTPCmd *ptpCmd, bool isQueued, uint64_t *queuedCmdIndex);

/*********************************************************************************************************
** Motor (conveyorbelt)
*********************************************************************************************************/
extern int SetEMotor(EMotor *eMotor, int8_t speed_mmPs, bool isQueued, uint64_t *queuedCmdIndex);
extern int SetInfraredSensor(Sensor *irSensor, bool isQueued);
extern int GetInfraredSensor(uint8_t port);
extern int SetColorSensor(Sensor *colorSensor, bool isQueued);
extern int GetColorSensor(void);

// Receive-Flags
extern bool irSensorAnswer;
extern bool colorSensorAnswer;

#endif
