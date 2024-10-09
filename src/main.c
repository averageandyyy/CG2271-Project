#include "MKL25Z4.h"
#include "test/test.h"
#include "motors/motor_driver.h"
#include "labs/blinky.h"

int main(void)
{
  SystemCoreClockUpdate();
  initBlinkyGPIO();
  initBlinkyTimer();
  while (1)
  {
		blinky_control(color_to_show);
  }
}
