//this program shows traffic on 433MHz
#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#include <wiringPi.h>

/* This structure is used by main to communicate with parse_opt. */
struct arguments
{
  int verbose;              /* The -v flag */
  char *logfile;            /* Argument for -f */
  int pin;					//-p
  int mirror_pin; //-m
  int nanoseconds; //-n
  int length;  /* -l */
  int type; //-t
};

/*
   OPTIONS.  Field 1 in ARGP.
   Order of fields: {NAME, KEY, ARG, FLAGS, DOC}.
*/
static struct argp_option options[] =
{
  {"verbose", 'v', 0, 0, "Produce verbose output"},
  {"pin",   'p', "PIN", 0,
   "The pin which is connected to the 433MHz receiving unit. wiringPi pin numbering."},
  {"mirror",   'm', "PIN", OPTION_ARG_OPTIONAL ,
   "The pin which will be used to mirror the input. wiringPi pin numbering."},
  {"file",  'f', "FILE", OPTION_ARG_OPTIONAL ,
   "Log the signal to a file"},
   {"nanoseconds",  'n', 0, 0,
   "Use nanoseconds instead of microseconds"},
   {"length",  'l', "SECONDS", OPTION_ARG_OPTIONAL ,
   "Specifies how long the program will log the signal. Requires time in seconds. Default is 10"},
   {"type",  't', "TYPE", OPTION_ARG_OPTIONAL ,
   "Type of logging. 0 to log HIGH and LOW pulses, 1 for only HIGH, 2 for only LOW. Default is 0"},
  {0}
};


/*
   PARSER. Field 2 in ARGP.
   Order of parameters: KEY, ARG, STATE.
*/
static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = state->input;

  switch (key)
    {
    case 'v':
      arguments->verbose = 1;
      break;
	 case 'f':
      arguments->logfile = arg;
      break;
	case 'p':
      arguments->pin = atoi(arg);
      break;
	case 'm':
      arguments->mirror_pin = atoi(arg);
      break;
    case 'n':
      arguments->nanoseconds = 1;
      break;
    case 'l':
      arguments->length = atoi(arg);
      break;
    case 't':
      arguments->type = atoi(arg);
      break;
    case ARGP_KEY_ARG:
      if (state->arg_num >= 2)
	{
	  argp_usage(state);
	}
      //arguments->args[state->arg_num] = arg;
      break;
    case ARGP_KEY_END:
      if (state->arg_num < 2)
	{
	  argp_usage (state);
	}
      break;
    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}

/*
   ARGS_DOC. Field 3 in ARGP.
   A description of the non-option command-line arguments
     that we accept.
*/
static char args_doc[] = "";

/*
  DOC.  Field 4 in ARGP.
  Program documentation.
*/
static char doc[] =
"sniffer -- A program to record a signal from a sensor or wireless signal receiver.";

/*
   The ARGP structure itself.
*/
static struct argp argp = {options, parse_opt, args_doc, doc};
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
	struct arguments arguments; //argument structure
	//set defaults
	arguments.verbose=0;
	arguments.logfile=0;
	arguments.pin=-1;
	arguments.mirror_pin=-1;
	arguments.nanoseconds=0;
	arguments.length=10;
	arguments.type=0;
	
	//Parse
  argp_parse (&argp, argc, argv, 0, 0, &arguments);
  
  //Debug output
  printf("Verbose mode %i\n",arguments.verbose);
  if(arguments.logfile){
	  printf("Log to file ");
	  printf(arguments.logfile);
	  printf("\n");
  }
  printf("Input pin %i\n",arguments.pin);
   printf("Mirror pin %i\n",arguments.mirror_pin);
   if(arguments.nanoseconds){
	   printf("Use nanoseconds instead of microseconds\n");
   }
   if(arguments.length){
	   printf("Log for %i\n",arguments.length);
   }
    printf("Logging type %i\n",arguments.type);
	return;
	 
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