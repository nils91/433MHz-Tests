//Sender Brennestuhl RCS 1000 N remote power socket
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <wiringPi.h>

#define NOMINAL_SHORT_PULSE 320
#define NOMINAL_LONG_PULSE 960
#define NOMINAL_XTRA_LONG_PULSE 9920

#define PULSE_CNT 50
#define CMD_LEN 12

#define START_PULSE 1

static void send_pulse(int pulslen,int status,int switch_on_end){
	digitalWrite(0,status);
	digitalWrite(1,status);
	usleep(pulslen);
	if(switch_on_end){
		digitalWrite(0,!status);
		digitalWrite(1,!status);
	}
}
static int encode_cmd(char* cmd,int cmd_len,int system_code,char unit_code,char onoff){
	memset((void*)cmd,0,cmd_len);
	if(cmd_len==12){
		//set system code
		if(system_code>=16){
			cmd[0]=1;
			system_code-=16;
		}
		if(system_code>=8){
			cmd[1]=1;
			system_code-=8;
		}
		if(system_code>=4){
			cmd[2]=1;
			system_code-=4;
		}
		if(system_code>=2){
			cmd[3]=1;
			system_code-=2;
		}
		if(system_code>=1){
			cmd[4]=1;
			system_code-=1;
		}
		//set unit code
		switch(unit_code){
			case 'A':
				cmd[5]=1;
				break;
			case 'B':
				cmd[6]=1;
				break;
			case 'C':
				cmd[7]=1;
				break;
			case 'D':
				cmd[8]=1;
				break;
			case 'E':
				cmd[9]=1;
				break;
			default:
				break;
		}
		//set command
		if(onoff==1){
			cmd[10]=1;
			cmd[11]=0;
		}else{
			cmd[10]=0;
			cmd[11]=1;
		}
		return 1;
	}
	return 0;
}
static int encode_signal(int* signal,int signal_len,char *cmd,int cmd_len){
	if(signal_len==50){
		int i=0;
		for(i=0;i<cmd_len;i++){
			if(cmd[i]==1){
				signal[i*4]=NOMINAL_SHORT_PULSE;
				signal[i*4+1]=NOMINAL_LONG_PULSE;
				signal[i*4+2]=NOMINAL_SHORT_PULSE;
				signal[i*4+3]=NOMINAL_LONG_PULSE;
			}else{
				signal[i*4]=NOMINAL_SHORT_PULSE;
				signal[i*4+1]=NOMINAL_LONG_PULSE;
				signal[i*4+2]=NOMINAL_LONG_PULSE;
				signal[i*4+3]=NOMINAL_SHORT_PULSE;
			}
		}
		//insert footer
		signal[48]=NOMINAL_SHORT_PULSE;
		signal[49]=NOMINAL_XTRA_LONG_PULSE;
		return 1;
	}
	return 0;
}
int main ( int argc, char *argv[] )
{
	if(argc<4) {
		printf("Expecting at least four params.\n");
		return 1;
	}
  wiringPiSetup () ;
  //setup pins
  pinMode (0, OUTPUT) ;//LED
  pinMode (1, OUTPUT) ;//Sender
  
  digitalWrite (1, LOW) ;
  int system_type=0;
  char unit_type='A';
  char onoff=0;
  int retries=5;
  system_type=atoi(argv[1]);
  unit_type=argv[2][0];
  if(strcmp(argv[3],"on")==0){
	  onoff=1;
  }
  if(argc>=4){
	  retries=atoi(argv[4]);
  }
  printf("Brennenstuhl remote power socket. \nSystem code: %i \nUnit code: %c\nSwitch %i ",system_type,unit_type,onoff);
  printf(argv[3]);
  printf("\nRetries %i\n",retries);
 
  
  
  
 

	//signal sending init
	int signal[PULSE_CNT];
	char cmd[CMD_LEN];
	if(encode_cmd(cmd,CMD_LEN,system_type,unit_type,onoff)){
		  if(encode_signal(signal,PULSE_CNT,cmd,CMD_LEN)){
			  int j=0;
			  for(j=0;j<retries;j++){
				  char pulse=START_PULSE;
				  int i=0;
				  for(i=0;i<PULSE_CNT;i++){
					 send_pulse(signal[i],pulse,1);
					 pulse=pulse*-1+1;
				  }
			  }
			  
		  }
	  }  
	  digitalWrite(0,LOW);
	  digitalWrite(1,LOW);
 
  return 0 ;
}