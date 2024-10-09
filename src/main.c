#include "MKL25Z4.h"
#include "test/test.h"
#include "motors/motor_driver.h"

int main(void)
{
  SystemCoreClockUpdate();
	
	initMotors();
	moveLeftSide(FORWARD, SLOW);
  while (1)
  {
		stop();
  }
}
