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
	moveLeftSide(BACKWARD, FAST);
	delay(DELAY_DURATION);
	moveRightSide(FORWARD, FAST);
	moveLeftSide(FORWARD, FAST);
	delay(DELAY_DURATION);
	moveTest();
  while (1)
  {
    stop();
  }
}
