//For Brennestuhl RCS 1000 N remote power socket
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <wiringPi.h>

#define NOMINAL_SHORT_PULSE 320
#define NOMINAL_LONG_PULSE 960
#define NOMINAL_XTRA_LONG_PULSE 9920

#define MAX_DEVIATION 0.2

#define PULSE_CNT 50
#define CMD_LEN 12

static int get_min_pulslen(int pulslen,double deviation){
	return pulslen-pulslen*deviation;
}
static int get_max_pulslen(int pulslen,double deviation){
	return pulslen+pulslen*deviation;
}
static int is_pulse_type(int pulslen,int pulstype){
	switch (pulstype){
		case NOMINAL_SHORT_PULSE:
			return (pulslen>=get_min_pulslen(NOMINAL_SHORT_PULSE,MAX_DEVIATION)&&pulslen<get_max_pulslen(NOMINAL_SHORT_PULSE,MAX_DEVIATION));
		case NOMINAL_LONG_PULSE:
			return (pulslen>=get_min_pulslen(NOMINAL_LONG_PULSE,MAX_DEVIATION)&&pulslen<get_max_pulslen(NOMINAL_LONG_PULSE,MAX_DEVIATION));
		case NOMINAL_XTRA_LONG_PULSE:
			return (pulslen>=get_min_pulslen(NOMINAL_XTRA_LONG_PULSE,MAX_DEVIATION)&&pulslen<get_max_pulslen(NOMINAL_XTRA_LONG_PULSE,MAX_DEVIATION));
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
static int decode_signal(int* signal){
	char* cmd=malloc(CMD_LEN);
	int cmd_real_len=0;
	int i=0;
	for(i=0;i<PULSE_CNT;i+=4){
		if(i+4<=PULSE_CNT){
			if(signal[i]==NOMINAL_SHORT_PULSE&&signal[i+1]==NOMINAL_LONG_PULSE&&signal[i+2]==NOMINAL_SHORT_PULSE&&signal[i+3]==NOMINAL_LONG_PULSE){
				cmd[i/4]=1;
				cmd_real_len++;
			}
			if(signal[i]==NOMINAL_SHORT_PULSE&&signal[i+1]==NOMINAL_LONG_PULSE&&signal[i+2]==NOMINAL_LONG_PULSE&&signal[i+3]==NOMINAL_SHORT_PULSE){
				cmd[i/4]=0;				
				cmd_real_len++;
			}
		}
		if(i+2<=PULSE_CNT){
			if(signal[i]==NOMINAL_SHORT_PULSE&&signal[i+1]==NOMINAL_XTRA_LONG_PULSE){
				if(cmd_real_len==12){
					cmd_real_len++;
				}
			}
		}
	}
	if(cmd_real_len==13){
		printf("Signal for Brennenstuhl RCS 1000N detected\n");
		printf("System code (DP) is ");
		for(i=0;i<5;i++){
			printf("%i",cmd[i]);
		}
		printf("\nUnit code is ");
		for(i=5;i<10;i++){
			if(cmd[i]==1){
				printf("%c",'A'+(i-6));
			}
		}
		printf("\nCommand is ");
		if(cmd[10]==1){
			printf("on\n");
		}else{
			printf("off\n");
		}
		if(cmd[10]!=cmd[11]){
			printf("Command verified\n");
		}else{
			printf("Command notverified\n");
		}
	}
	free(cmd);
	
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

	//signal detection init
	int* signal=malloc(PULSE_CNT);
	int pulse_counter=0;
	int signal_start=0; //flag to signal a new signal
  for (;;)
  {
	  int r_status=digitalRead(2);	 
	  if(r_status!=signal_status){ 
		long edge_detection_time=get_micros();
		int pulslen=edge_detection_time-last_edge_detection;
		if(signal_start==1){
			signal_start=0;
			pulse_counter=0;
		}
		switch(get_pulse_type(pulslen)){
			case NOMINAL_SHORT_PULSE:
				signal[pulse_counter]=NOMINAL_SHORT_PULSE;
				break;
			case NOMINAL_LONG_PULSE:
				signal[pulse_counter]=NOMINAL_LONG_PULSE;
				break;
			case NOMINAL_XTRA_LONG_PULSE:
				signal[pulse_counter]=NOMINAL_XTRA_LONG_PULSE;
				decode_signal(signal);
				if(signal_status==0){
					signal_start=1;
				}
				break;
			default:
				break;
		}
		if(pulse_counter<PULSE_CNT)
			pulse_counter++;
		 
		  last_edge_detection=edge_detection_time;
		  signal_status=r_status;
		  
	  }
	  digitalWrite(0, r_status);
	  
	  
  }
  free(signal);
  return 0 ;
}