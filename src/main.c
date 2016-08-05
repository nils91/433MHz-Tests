#include <wiringPi.h>
int main (void)
{
  wiringPiSetup () ;
  pinMode (0, OUTPUT) ;
  pinMode (1, OUTPUT) ;
  pinMode (2, INPUT) ;
  
  digitalWrite (1, LOW) ;
  pullUpDnControl (2, PUD_DOWN); 
  for (;;)
  {
	  int r_status=digitalRead(2);
	  digitalWrite(0, r_status);
	  
	  
  }
  return 0 ;
}