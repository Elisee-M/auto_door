#ifndef FIREBASE_MODULE_H
#define FIREBASE_MODULE_H

#include <Arduino.h>

void sendFirebase(String access,String door_state,String userID);
String getNameFromFirebase(String uid);

#endif