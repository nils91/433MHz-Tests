//this program shows traffic on 433MHz
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
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
int main ( int argc, char **argv)
{
	int verbose=0;
	char *logfile=0;
	
  wiringPiSetup () ;
  //setup pins
  pinMode (0, OUTPUT) ;
  pinMode (2, INPUT) ;
  //pull-down on receiver input pin
  pullUpDnControl (2, PUD_DOWN); 
  int signal_status=0;
  long last_edge_detection=get_micros();  
  for (;;)
  {
	  int r_status=digitalRead(2);	 
	  if(r_status!=signal_status){ 
		long edge_detection_time=get_micros();
		  printf("Signal change\n");
		  printf("Signal is now %i\n",r_status);
		  if(r_status==0){ 
			  printf("Signal was on %ld\n",edge_detection_time-last_edge_detection);
			 
		  }else{
			  printf("Signal was off %ld\n",edge_detection_time-last_edge_detection);
			 
		  }
		  last_edge_detection=edge_detection_time;
		  signal_status=r_status;
		  
	  }
	  digitalWrite(0, r_status);
	  
	  
  }
  return 0 ;
}