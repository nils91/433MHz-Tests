//this program shows traffic on 433MHz
#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#include <wiringPi.h>

/* This structure is used by main to communicate with parse_opt. */
struct arguments {
	int verbose; /* The -v flag */
	char *logfile; /* Argument for -f */
	int pin;					//-p
	int mirror_pin; //-m
	int nanoseconds; //-n
	int length; /* -l */
	int type; //-t
};

/*
 OPTIONS.  Field 1 in ARGP.
 Order of fields: {NAME, KEY, ARG, FLAGS, DOC}.
 */
static struct argp_option options[] =
		{ { "verbose", 'v', 0, 0, "Produce verbose output" },
				{ "pin", 'p', "PIN", 0,
						"The pin which is connected to the 433MHz receiving unit. wiringPi pin numbering." },
				{ "mirror", 'm', "PIN", OPTION_ARG_OPTIONAL,
						"The pin which will be used to mirror the input. wiringPi pin numbering." },
				{ "file", 'f', "FILE", OPTION_ARG_OPTIONAL,
						"Log the signal to a file" }, { "nanoseconds", 'n', 0,
						0, "Use nanoseconds instead of microseconds" },
				{ "length", 'l', "SECONDS", OPTION_ARG_OPTIONAL,
						"Specifies how long the program will log the signal. Requires time in seconds. Default is 10" },
				{ "type", 't', "TYPE", OPTION_ARG_OPTIONAL,
						"Type of logging. 0 to log HIGH and LOW pulses, 1 for only HIGH, 2 for only LOW. Default is 0" },
				{ 0 } };

/*
 PARSER. Field 2 in ARGP.
 Order of parameters: KEY, ARG, STATE.
 */
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
	struct arguments *arguments = state->input;

	switch (key) {
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
static struct argp argp = { options, parse_opt, args_doc, doc };
static unsigned long get_nanos(void) {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (long) ts.tv_sec * 1000000000L + ts.tv_nsec;
}
static unsigned long get_micros(void) {
	return get_nanos() / 1000;
}
static unsigned long get_millis(void) {
	return get_micros() / 1000;
}
static unsigned long get_seconds(void) {
	return get_millis() / 1000;
}
int main(int argc, char **argv) {
	struct arguments arguments; //argument structure
	//set defaults
	arguments.verbose = 0;
	arguments.logfile = 0;
	arguments.pin = -1;
	arguments.mirror_pin = -1;
	arguments.nanoseconds = 0;
	arguments.length = 10;
	arguments.type = 0;

	printf("Before parse\n");
	//Parse
	argp_parse(&argp, argc, argv, 0, 0, &arguments);
	printf("After parse\n");
	//Debug output
	printf("Verbose mode %i\n", arguments.verbose);
	if (arguments.logfile) {
		printf("Log to file ");
		printf(arguments.logfile);
		printf("\n");
	}
	printf("Input pin %i\n", arguments.pin);
	printf("Mirror pin %i\n", arguments.mirror_pin);
	if (arguments.nanoseconds) {
		printf("Use nanoseconds instead of microseconds\n");
	}
	if (arguments.length) {
		printf("Log for %i\n", arguments.length);
	}
	printf("Logging type %i\n", arguments.type);
	if (arguments.pin == -1) {
		exit(1);
	}
	//setup wiring pi
	wiringPiSetup();
	//setup pins
	if (arguments.mirror_pin >= 0) {
		pinMode(arguments.mirror_pin, OUTPUT);
	}

	pinMode(arguments.pin, INPUT);
	//pull-down on receiver input pin
	pullUpDnControl(arguments.pin, PUD_DOWN);
//sniffer vars setup
	unsigned long (*time_func)(void)=&get_micros;
	if (arguments.nanoseconds) {
		time_func = &get_nanos;
	}
	int signal_status = 0;
	unsigned long recording_start = time_func();
	unsigned long recording_start_seconds = get_seconds();
	unsigned long last_edge_detection = time_func();
//print table head
	char *head = "time, time_type, old, new, pulse_length\n";
	if (arguments.verbose) {
		printf(head);
	}
	while (arguments.length == 0
			|| recording_start_seconds - get_seconds() < arguments.length) {
		int r_status = digitalRead(arguments.pin);
		if (r_status != signal_status) {
			unsigned long edge_detection_time = time_func();
			char time_type = 'm';
			if (arguments.nanoseconds) {
				time_type = 'n';
			}
			unsigned long duration = last_edge_detection - edge_detection_time;
			unsigned long time_since_start = edge_detection_time
					- recording_start;
			char *line = malloc(
					sizeof(unsigned long) + 2 + 1 + 2 + (sizeof(int) + 2) * 2
							+ sizeof(unsigned long) + 1+2);
			sprintf(line, "%lu, %c, %i, %i, %lu\n", time_since_start, time_type,
					signal_status, r_status, duration);
			if (arguments.verbose)
				printf(line);
			free(line);

			last_edge_detection = edge_detection_time;
			signal_status = r_status;

		}
		if (arguments.mirror_pin >= 0) {
			digitalWrite(arguments.mirror_pin, r_status);
		}

	}
	//turn mirror pin off
	if (arguments.mirror_pin >= 0) {
		digitalWrite(arguments.mirror_pin, 0);
	}
	return 0;
}
