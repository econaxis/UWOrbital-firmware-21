// Always include include guards
#ifdef TELEM_H
#define TELEM_H

#include "cdh.h"

/**

Telemetry
Used to define the processes and structured needed for the once per minute telemetry readings taken.
Called by cdh regularily, calls other systems for telemetry updates.
Operates entirely on and is called from the CDH thread

**/

// The storage format of a telemetry reading
struct telemetryReading {
  int temp; // internal temperature
  int posX; // estimated position
  int posY; // 
  int posZ; //
  int rotX; // estimated pointing direction
  int rotY; // 
  int rotZ; //
};

// Take a reading of all telemetery points and add it to the passed in pointer
// tr: an emptry telemetryReading to be filled with lovely data when this function completes  
void takeReading(struct telemetryReading* tr);

#endif
