/** @file init.c
 * @brief File for initialization code
 *
 * This file should contain the user initialize() function and any functions related to it.
 *
 * Copyright (c) 2011-2014, Purdue University ACM SIG BOTS.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Purdue University ACM SIG BOTS nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL PURDUE UNIVERSITY ACM SIG BOTS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Purdue Robotics OS contains FreeRTOS (http://www.freertos.org) whose source code may be
 * obtained from http://sourceforge.net/projects/freertos/files/ or on request.
 */

#include "main.h"

/*
 * Runs pre-initialization code. This function will be started in kernel mode one time while the
 * VEX Cortex is starting up. As the scheduler is still paused, most API functions will fail.
 *
 * The purpose of this function is solely to set the default pin modes (pinMode()) and port
 * states (digitalWrite()) of limit switches, push buttons, and solenoids. It can also safely
 * configure a UART port (usartOpen()) but cannot set up an LCD (lcdInit()).
 */
void initializeIO() {
}

/*
 * Runs user initialization code. This function will be started in its own task with the default
 * priority and stack size once when the robot is starting up. It is possible that the VEXnet
 * communication link may not be fully established at this time, so reading from the VEX
 * Joystick may fail.
 *
 * This function should initialize most sensors (gyro, encoders, ultrasonics), LCDs, global
 * variables, and IMEs.
 *
 * This function must exit relatively promptly, or the operatorControl() and autonomous() tasks
 * will not start. An autonomous mode selection menu like the pre_auton() in other environments
 * can be implemented in this task if desired.
 */
void initialize() {

		driveTrainStyle = DTFOURWHEELS;
		controlStyle = CTCHEEZYDRIVE;

	//	IMEARMLEFT = 0;
	//	IMEARMRIGHT = 1;

		riceBotInitialize();

		MOTDTFrontLeft = initMotor(7, 1);
		MOTDTFrontRight = initMotor(6, -1);
		MOTDTBackLeft = initMotor(3, -1);
		MOTDTBackRight = initMotor(2, 1);

		MOTARMBack = initMotor(9, 1);
		MOTARMBottomLeft = initMotor(4, 1);
		MOTARMBottomRight = initMotor(8, -1);

		MOTCOL = initMotor(5, 1);

		EncDTLeft = initRicencoder(627.2, 1, 1, 1, 0, NULL, false);
		EncDTRight = initRicencoder(627.2, 1, 1, 0, 0, NULL, true);

		EncARMBottom = initRicencoder(627.2, 1, 1, 2, 0, NULL, false);
		EncARMTop = initRicencoder(627.2, 1, 1, 3, 0, NULL, true);
		PotARMFront = initRicepot(1);

		gyro = initRicegyro(2, 196);

		printf("Preparing IMEs...\n\r");
		printf("IME Count: %d\n\r", imeInitializeAll());
		imeReset(IMEARMLEFT);
		imeReset(IMEARMRIGHT);

		PidARMBottom = initPid(.3, 0, 0);
		PidARMTop = initPid(.3, 0, 0);
		PidARMBottom.running = 1;
		PidARMTop.running = 0;

		taskCreate(startIOTask, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_HIGHEST);
		taskCreate(startPidTask, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);

	}
	void startIOTask(void *ignore) {
		while(1) {
			setDriveTrainMotors(DTFOURWHEELS);

			motorSet(MOTARMFront.port, MOTARMFront.out * MOTARMFront.reflected);
			motorSet(MOTARMBack.port, MOTARMBack.out * MOTARMBack.reflected);
			motorSet(MOTARMTop.port, MOTARMTop.out * MOTARMTop.reflected);
			motorSet(MOTARMMiddle.port, MOTARMMiddle.out * MOTARMMiddle.reflected);
			motorSet(MOTARMBottom.port, MOTARMBottom.out * MOTARMBottom.reflected);
			motorSet(MOTARMLeft.port, MOTARMLeft.out * MOTARMLeft.reflected);
			motorSet(MOTARMRight.port, MOTARMRight.out * MOTARMRight.reflected);
			motorSet(MOTARMTopLeft.port, MOTARMTopLeft.out * MOTARMTopLeft.reflected);
			motorSet(MOTARMTopRight.port, MOTARMTopRight.out * MOTARMTopRight.reflected);
			motorSet(MOTARMBottomLeft.port, MOTARMBottomLeft.out * MOTARMBottomLeft.reflected);
			motorSet(MOTARMBottomRight.port, MOTARMBottomRight.out * MOTARMBottomRight.reflected);

			motorSet(MOTCOL.port, MOTCOL.out * MOTCOL.reflected);
			motorSet(MOTCOLLeft.port, MOTCOLLeft.out * MOTCOLLeft.reflected);
			motorSet(MOTCOLRight.port, MOTCOLRight.out * MOTCOLRight.reflected);

			PotARMFront.value = -analogReadCalibrated(PotARMFront.port);
			PotARMLeft.value = analogReadCalibrated(PotARMLeft.port);
			PotARMRight.value = -analogReadCalibrated(PotARMRight.port);

			updateRicencoder(&EncDTLeft);
			updateRicencoder(&EncARMRight);
			updateRicencoder(&EncDTRight);
			updateRicencoder(&EncARMBottom);
			updateRicencoder(&EncARMTop);

			updateRicegyro(&gyro);

			delay(10);
		}
	}

	void startPidTask(void *ignore) {
		while(1) {
			//Manually add each pid loop here
			processPid(&PidARMLeft, EncARMLeft.adjustedValue);
			processPid(&PidARMRight, EncARMRight.adjustedValue);
			processPid(&PidARMBottom, EncARMBottom.adjustedValue);
			processPid(&PidARMTop, EncARMTop.adjustedValue);
			processPid(&PidARMFront, PotARMFront.value);
			if(PidARMLeft.running) {
				MOTARMBottomLeft.out = PidARMLeft.output;
			}
			if(PidARMRight.running) {
				MOTARMBottomRight.out = PidARMRight.output;
			}
			if(PidARMFront.running) {
				MOTARMFront.out = PidARMFront.output;
			}
			if(PidARMBottom.running) {
				MOTARMBottom.out = PidARMBottom.output;
			}
			if(PidARMTop.running) {
				MOTARMTop.out = PidARMTop.output;
			}

					printf("Setpoint: %d|%d, IME: %d|%d, Adj: %d|%d, Out: %d|%d\n\r",
							PidARMBottom.setPoint, PidARMTop.setPoint,
							EncARMBottom.rawValue, EncARMTop.rawValue,
							EncARMBottom.adjustedValue, EncARMTop.adjustedValue,
							PidARMBottom.output, PidARMTop.output);
	//				printf("Setpoint: %d, Current: %d, Out: %d\n\r", PidARMFront.setPoint, PidARMFront.current, PidARMFront.output);
	//		printf("Power: %dmV\n\r", powerLevelMain());
			delay(20);
		}
	}
