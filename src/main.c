#include "motors/motor_driver.h"
#include "labs/blinky.h"

int main(void)
{
  SystemCoreClockUpdate();
  // initBlinkyGPIO();
  // initBlinkyTimer();
  initGPIO();
  initTimers();
	moveRightSide(BACKWARD, FAST);
  while (1)
  {
    // stop();
  }
}
