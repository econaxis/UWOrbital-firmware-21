// Always include include guards
#ifdef CDH_H
#define CDH_H

#include "comms.h"
#include "eps.h"
#include "adcs.h"

/**
    CDH .h file
    
    0.0.1 Added first pass of endpoints for comms
**/

// The satellite at any one time will be in one of these states which can notify systems of what they have to do
enum state {Launch, Startup, Stabilize, CommPass, PhotoPass, Downtime, Telemetry, ADCSOps, SafeMode};

// This variable is updated by the CDH main thread, and can be read by the other threads to trigger things.
extern state currentState;

// function called to notify main of command from comms
// commandString: decoded string from uplink
int commandRecieved(char* commandString);

#endif
