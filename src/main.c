#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <wiringPi.h>
int main (void)
{
  wiringPiSetup () ;
  //setup pins
  pinMode (0, OUTPUT) ;
  pinMode (1, OUTPUT) ;
  pinMode (2, INPUT) ;
  
  digitalWrite (1, LOW) ;
  //pull-down on receiver input pin
  pullUpDnControl (2, PUD_DOWN); 
  int signal_status=0;
  for (;;)
  {
	  int r_status=digitalRead(2);
	  if(r_status!=signal_status){
		  printf("Signal change\n");
		  printf("Signal is now %i\n",r_status);
		  signal_status=r_status;
		  
	  }
	  digitalWrite(0, r_status);
	  
	  
  }
  return 0 ;
}