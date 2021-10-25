
/*-----------------------------------------------------------
 * Simple IO routines to control the LEDs.
 *-----------------------------------------------------------*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "os_task.h"

/* Demo includes. */
#include "partest.h"
#include "gio.h"

/*-----------------------------------------------------------*/

void vParTestToggleLED()
{
unsigned long ulBitState;
    ulBitState = gioGetBit( gioPORTB, 1 );
    gioSetBit( gioPORTB, 1, !ulBitState );
}
							


