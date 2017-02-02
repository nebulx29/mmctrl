/***
 * Program: mmctrl
 * Usage: mmctrl'
 * Description: run as daemon, listening on MagicMirror GPIO pins (wiringPi numbering)
 *              on defined signal the commands are executed, Ctrl+C to stop program
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <wiringPi.h>

// *** CONSTANTS ***
#define DEBUG 1
#define TIMESTAMP_LENGTH 25
#define PCF8574_ADDR 0x20
#define PIN_BASE 128
#define NR_PINS 8
#define LOOP_WAIT 50
#define PRESS_DELAY 1000
#define PIN_SIG_WAIT 50

// *** CONSTANTS ***
const int PINS[]    = { 128, 129, 130, 131, 132, 133, 134, 135 };
const char *CMDS_TEST[]   = {
				"date +\"%Y%m%d_%H%M%S GPIO 128 (Key 0) pressed\" >> /var/log/mmctrl/mmctrl.log",
				"date +\"%Y%m%d_%H%M%S GPIO 129 (Key 1) pressed\" >> /var/log/mmctrl/mmctrl.log",
				"date +\"%Y%m%d_%H%M%S GPIO 130 (Key 2) pressed\" >> /var/log/mmctrl/mmctrl.log",
				"date +\"%Y%m%d_%H%M%S GPIO 131 (Key 3) pressed\" >> /var/log/mmctrl/mmctrl.log",
				"date +\"%Y%m%d_%H%M%S GPIO 132 (Key 4) pressed\" >> /var/log/mmctrl/mmctrl.log",
				"date +\"%Y%m%d_%H%M%S GPIO 133 (Key 5) pressed\" >> /var/log/mmctrl/mmctrl.log",
				"date +\"%Y%m%d_%H%M%S GPIO 134 (Key 6) pressed\" >> /var/log/mmctrl/mmctrl.log",
				"date +\"%Y%m%d_%H%M%S GPIO 135 (Key 7) pressed\" >> /var/log/mmctrl/mmctrl.log"	
				};

const char *CMDS[]   = {
				"date +\"%Y%m%d_%H%M%S GPIO 128 (Key 0) pressed. No action assigned.\" >> /var/log/mmctrl/mmctrl.log",
				"date +\"%Y%m%d_%H%M%S GPIO 129 (Key 1) pressed. No action assigned.\" >> /var/log/mmctrl/mmctrl.log",
				"date +\"%Y%m%d_%H%M%S GPIO 130 (Key 2) pressed. Executing mm-start.sh \" ; /home/pi/bin/mm-start.sh >> /var/log/mmctrl/mmctrl.log",
				"date +\"%Y%m%d_%H%M%S GPIO 131 (Key 3) pressed. Executing mm-stop.sh \" ; /home/pi/bin/mm-stop.sh  >> /var/log/mmctrl/mmctrl.log",
				"date +\"%Y%m%d_%H%M%S GPIO 132 (Key 4) pressed. No action assigned.\" >> /var/log/mmctrl/mmctrl.log",
				"date +\"%Y%m%d_%H%M%S GPIO 133 (Key 5) pressed. No action assigned.\" >> /var/log/mmctrl/mmctrl.log",
				"date +\"%Y%m%d_%H%M%S GPIO 134 (Key 6) pressed. Executing reboot\" ; sudo shutdown -r now >> /var/log/mmctrl/mmctrl.log",
				"date +\"%Y%m%d_%H%M%S GPIO 135 (Key 7) pressed. Executing shutdown\" ; sudo shutdown -h now >> /var/log/mmctrl/mmctrl.log"	
				};				

// *** GLOBAL VARIABLES ***				
int SIG_RCVD[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
char timestamp[TIMESTAMP_LENGTH];

// *** FUNCTION DECLARATIONS ***
void printUsage(void);
int init(void);
int readPin(int pin);

// *** FUNCTION DEFINITONS ***
void printUsage() {
	fprintf(stderr, "Usage: mmctrl\n"
		            "  TODO\n"
		);
	exit(EXIT_FAILURE);
}

int init(void) {
	int i;
	if (wiringPiSetup()<0) {
		fprintf(stderr, "Unable to setup wiringPi: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
		return 1;
	}
	pcf8574Setup (PIN_BASE, PCF8574_ADDR);
	
	for (i=0; i<NR_PINS; i++) {
		pinMode(PINS[i], INPUT);
		pullUpDnControl(PINS[i], PUD_UP);
	}
	
    createTimestamp(timestamp);
	for (i=0; i<NR_PINS; i++) 
		fprintf(stdout,"%s init(GPIO=%d). Command='%s'\n", timestamp, PINS[i], CMDS[i]);
	fflush(stdout);

	return 0;
}

/* 
 * returns true if PIN is low for 
 */
int readPin(int pin) {
	int v = digitalRead(pin);
	if (digitalRead(pin)==LOW) {
		delay(PIN_SIG_WAIT);
		if (digitalRead(pin)==LOW) {
			return 1;
		}
	}
	return 0;
}

int createTimestamp(char* buffer) {
  int millisec;
  struct tm* tm_info;
  struct timeval tv;
  char buffer2[TIMESTAMP_LENGTH];
  
  gettimeofday(&tv, NULL);

  millisec = lrint(tv.tv_usec/1000.0); 
  if (millisec>=1000) { 
    millisec -=1000;
    tv.tv_sec++;
  }

  tm_info = localtime(&tv.tv_sec);
  strftime(buffer, TIMESTAMP_LENGTH, "%Y%m%d %H:%M:%S", tm_info); 
  sprintf(buffer, "%s.%d", buffer, millisec);
}

int main(int argc, char* argv[]) {
	int i;
	if (argc!=1) {
		printUsage();
		exit(1);
		return 1;
	}

	init();

	while (1) {
		delay(LOOP_WAIT);
		
		for (i=0; i<NR_PINS; i++) {
			if (readPin(PINS[i])) {
				createTimestamp(timestamp);
				fprintf(stdout,"%s SIG_RCVD(%d). Executing '%s'... start.\n", timestamp, PINS[i], CMDS[i]);
				fflush(stdout);
				system(CMDS[i]);
				delay(PRESS_DELAY);
			}
		}
	}
	exit(EXIT_SUCCESS);
	return 0;
}



