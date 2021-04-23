#include "elevator_interface.h"
#include <cstdio>
void pressFloorButton(int floor) {
  if (floor == 1){
    digitalWrite(FL_1_CALL_PIN, LOW);
    delay(1000);
    digitalWrite(FL_1_CALL_PIN, HIGH);
  }
  else if (floor == 2){
    digitalWrite(FL_2_CALL_PIN, LOW);
    delay(1000);
    digitalWrite(FL_2_CALL_PIN, HIGH);
  }  
  else if (floor == 3){
    digitalWrite(FL_3_CALL_PIN, LOW);
    delay(1000);
    digitalWrite(FL_3_CALL_PIN, HIGH);
  }
}

void holdDoorOpen(){
  digitalWrite(DOOR_OPEN_CALL_PIN, LOW);
  delay(1000);
  digitalWrite(DOOR_OPEN_CALL_PIN, HIGH);
 }

 void initializeGPIOPins(){
  // setup GPIO pin modes and pull-up resistors
  wiringPiSetup();
  //Car call floor 1
  pinMode(FL_1_CALL_PIN, OUTPUT);
  digitalWrite(FL_1_CALL_PIN, HIGH);
  //Position indicator floor 1
  pinMode(FL_1_STATUS_PIN, INPUT);
  pullUpDnControl(FL_1_STATUS_PIN, PUD_DOWN);
  //Car call floor 2
  pinMode(FL_2_CALL_PIN, OUTPUT);
  digitalWrite(FL_2_CALL_PIN, HIGH);
  //Position indicator floor 2
  pinMode(FL_2_STATUS_PIN, INPUT);
  pullUpDnControl(FL_2_STATUS_PIN, PUD_DOWN);
  //Car call floor 3
  pinMode(FL_3_CALL_PIN, OUTPUT);
  digitalWrite(FL_3_CALL_PIN, HIGH);
  //Position indicator floor 3
  pinMode(FL_3_STATUS_PIN, INPUT);
  pullUpDnControl(FL_3_STATUS_PIN, PUD_DOWN);
  //Car call door open
  pinMode(DOOR_OPEN_CALL_PIN, OUTPUT);
  digitalWrite(DOOR_OPEN_CALL_PIN, HIGH);
  //Door open limit switch
  pinMode(DOOR_STATUS_OPEN_PIN, INPUT);
  pullUpDnControl(DOOR_STATUS_OPEN_PIN, PUD_DOWN);
  //Door closed limit switch
  pinMode(DOOR_STATUS_CLOSED_PIN, INPUT);
  pullUpDnControl(DOOR_STATUS_CLOSED_PIN, PUD_DOWN);
 }

 void updateElevatorStatus(ElevatorStatus &status){

  if (digitalRead(FL_1_STATUS_PIN) == HIGH) {
    status.floor = 1;
  }
  else if (digitalRead(FL_2_STATUS_PIN) == HIGH) {
    status.floor = 2;
  }
  else if (digitalRead(FL_3_STATUS_PIN) == HIGH) {
    status.floor = 3;
  } else  {
    status.floor = 0;
  }
  
  if (digitalRead(DOOR_STATUS_OPEN_PIN) == HIGH) {
    status.door = 1;
  } else if (digitalRead(DOOR_STATUS_CLOSED_PIN) == HIGH) {
    status.door = 0;
  } else {
    status.door = 2;
  }
  
 }
