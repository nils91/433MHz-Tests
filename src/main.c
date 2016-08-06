//For Brennestuhl RCS 1000 N remote power socket
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <wiringPi.h>

#define NOMINAL_SHORT_PULSE 320
#define NOMINAL_LONG_PULSE 960
#define NOMINAL_XTRA_LONG_PULSE 9920

#define MAX_DEVIATION 0.2
static int get_min_pulslen(int pulslen,double deviation){
	return pulslen-pulslen*deviation;
}
static int get_max_pulslen(int pulslen,double deviation){
	return pulslen+pulslen*deviation;
}
static int is_pulse_type(int pulslen,int pulstype){
	switch (pulstype){
		case NOMINAL_SHORT_PULSE:
			return (pulslen>=get_min_pulslen(NOMINAL_SHORT_PULSE,MAX_DEVIATION)||pulslen<get_max_pulslen(NOMINAL_SHORT_PULSE,MAX_DEVIATION));
		case NOMINAL_LONG_PULSE:
			return (pulslen>=get_min_pulslen(NOMINAL_LONG_PULSE,MAX_DEVIATION)||pulslen<get_max_pulslen(NOMINAL_LONG_PULSE,MAX_DEVIATION));
		case NOMINAL_XTRA_LONG_PULSE:
			return (pulslen>=get_min_pulslen(NOMINAL_XTRA_LONG_PULSE,MAX_DEVIATION)||pulslen<get_max_pulslen(NOMINAL_XTRA_LONG_PULSE,MAX_DEVIATION));
		default:
			break;
	}
	return 0;
}
static int get_pulse_type(int pulslen){
	if(is_pulse_type(pulslen,NOMINAL_SHORT_PULSE)){
		return NOMINAL_SHORT_PULSE;
	}
	if(is_pulse_type(pulslen,NOMINAL_LONG_PULSE)){
		return NOMINAL_LONG_PULSE;
	}
	if(is_pulse_type(pulslen,NOMINAL_XTRA_LONG_PULSE)){
		return NOMINAL_XTRA_LONG_PULSE;
	}
	return 0;
}

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
		int pulslen=edge_detection_time-last_edge_detection;
		switch(get_pulse_type(pulslen)){
			case NOMINAL_SHORT_PULSE:
				printf("NOMINAL_SHORT_PULSE %i\n",r_status);
				break;
			case NOMINAL_LONG_PULSE:
				printf("NOMINAL_LONG_PULSE %i\n",r_status);
				break;
			case NOMINAL_XTRA_LONG_PULSE:
				printf("NOMINAL_XTRA_LONG_PULSE %i\n",r_status);
				break;
			default:
				break;
		}
		 
		  last_edge_detection=edge_detection_time;
		  signal_status=r_status;
		  
	  }
	  digitalWrite(0, r_status);
	  
	  
  }
  return 0 ;
}