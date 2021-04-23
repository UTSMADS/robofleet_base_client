#pragma once
#ifndef _ENCODE_ELEVATOR_H
#define _ENCODE_ELEVATOR_H
#include <wiringPi.h>

// Declare pins as constants
const int FL_1_STATUS_PIN = 0;
const int FL_2_STATUS_PIN = 1;
const int FL_3_STATUS_PIN = 2;
const int DOOR_STATUS_OPEN_PIN = 3;
const int DOOR_STATUS_CLOSED_PIN = 4;
const int FL_1_CALL_PIN = 7;
const int FL_2_CALL_PIN = 6;
const int FL_3_CALL_PIN = 5;
const int DOOR_OPEN_CALL_PIN = 12;

struct ElevatorStatus{
  // int floor: integer 1 for floor 1, 2 for floor 2, etc. 0 for in between floors.
  int floor;
  // int door_status: integer 1 for open, 0 for closed, and 2 if in between.
  int door;
};

struct ElevatorCommand {
  int floor_cmd;
  bool hold_door;
};

void pressFloorButton(int floor);
void holdDoorOpen();
void initializeGPIOPins();
void updateElevatorStatus(ElevatorStatus &status);

#endif