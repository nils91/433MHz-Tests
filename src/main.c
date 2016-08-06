#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <wiringPi.h>

static long get_nanos(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC,&ts);
    return (long)ts.tv_sec * 1000000000L + ts.tv_nsec;
}
static long get_micros(void) {
    return get_nanos()/1000;
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
  long last_edge_detection=get_micros();  
  for (;;)
  {
	  int r_status=digitalRead(2);	 
	  if(r_status!=signal_status){ 
		long edge_detection_time=get_micros();
		  if(r_status==0){ 
			  printf("%i %ld\n",signal_status,edge_detection_time-last_edge_detection);
			 
		  }else{
			  printf("%i %ld\n",signal_status,edge_detection_time-last_edge_detection);
			 
		  }
		  last_edge_detection=edge_detection_time;
		  signal_status=r_status;
		  
	  }
	  digitalWrite(0, r_status);
	  
	  
  }
  return 0 ;
}