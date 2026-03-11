#ifndef FINGERPRINT_MODULE_H
#define FINGERPRINT_MODULE_H

#include <Adafruit_Fingerprint.h>

void setupFingerprint();
void checkFingerprint();

extern Adafruit_Fingerprint finger;

#endif