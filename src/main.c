#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <wiringPi.h>

static long get_nanos(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC,&ts);
    return (long)ts.tv_sec * 1000000000L + ts.tv_nsec;
}
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
  long signal_start_nanosec=get_nanos();
  long signal_stop_nanosec=get_nanos();
  for (;;)
  {
	  int r_status=digitalRead(2);
	  
	  if(r_status!=signal_status){
		  printf("Signal change\n");
		  printf("Signal is now %i\n",r_status);
		  if(r_status==0){
			  printf("Signal was on %ld\n",signal_stop_nanosec-signal_start_nanosec);
			  signal_stop_nanosec=get_nanos();
		  }else{
			  
			  printf("Signal was off %ld\n",signal_start_nanosec-signal_stop_nanosec);
			  signal_start_nanosec=get_nanos();
		  }
		  signal_status=r_status;
		  
	  }
	  digitalWrite(0, r_status);
	  
	  
  }
  return 0 ;
}