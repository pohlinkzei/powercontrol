/*
 * powercontrol on raspberry pi

 * Compile with:
 * gcc -lxdo -lwiringPi powercontrol.c -o powercontrol
 * https://github.com/jordansissel/xdotool
 */ 
//git clone git://git.drogon.net/wiringPi


 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringSerial.h>
//#include <xdo.h>

#define SERIAL_1  '1'
#define SERIAL_2  '2'
#define SERIAL_3  '3'
#define SERIAL_4  '4'
#define SERIAL_5  '5'
#define SERIAL_6  '6'
#define SERIALnext  'n'
#define SERIALprev  'p'
#define SERIALeject  'e'
#define SERIALtim  't'
#define SERIALinfo  'i'
#define SERIALas  'a'
#define SERIALscan  's'
#define SERIALflag  'f'
#define SERIALlight  'l'
#define SERIALnavi  'N'
#define SERIALback  'b'
#define SERIALaudio  'A'
#define SERIALtone  'T'
#define SERIALenter 'E'
#define SERIALtraffic 'v'
#define SERIALplus '+'
#define SERIALminus '-'

#define SHUTDOWN_PIN 17
#define ACTIVE_PIN 18
#define READY_PIN 27

int ser;
char radio_string[20] = {0,};
int ff_count = 0;
unsigned char rx_old = 0x00;
int backcount = 0;
int status;
int status_off;
int no_ser_count = 0;

long int winID = -1;
char window[24][24];
int numResults = 0;
int xdo_cnt = 0;

void send_key(char* key){
	int i=0;
	if (numResults > 0 && backcount == 0) { 
		for(i=0; i<numResults; i++){
			char command[100] = {0,};
			sprintf(command, "xdotool key --window %s %s", window[i], key);//--window %s
			system(command);
			//xdo_send_keysequence_window(xdo, window[i], key, 10); 
		}
	} 
}

void getNavitWindow(void){
	FILE *fp = popen("xdotool search --name Navit", "r");
	char response[32];
	char tmp;
	numResults = 0;
	if(fp == NULL){
		printf("Error: Failed to run command!");
		return;
	}
	while(fgets(response, sizeof(response), fp) != NULL){
		strtok(response, "\n");
		strcpy(window[numResults], response ); 
		numResults++;
	}
	
}


void serial_task(void){
	//*
	if(numResults < 1){
		getNavitWindow();
		printf("Found %d windows.\n", numResults); 
	}else{
		if(1000 == xdo_cnt++){
			xdo_cnt = 0;
			numResults = 0;
		}
	} 
	//*/
	if(serialDataAvail(ser)>0){
		unsigned char rx = serialGetchar(ser);
		//serialPutchar(ser, rx);
		/*if(rx){
			printf("Taste: %c,%i\n",rx,rx);
			return;
		}*/
		/*
		if(rx != '0'){
			printf("Taste: %c,%i\n",rx,rx);
		}
		//*/
		if(rx != rx_old  || (rx=='+' || rx=='-')){
			if(rx == 0x01){//rpimfdinterface startete den pi nach zv auf, radio ist aber noch nicht eingeschaltet - wir warten.
				send_key("F8");
				rx_old = rx;
				sleep(1);
			}
			if(rx == '0' && (rx_old == 0x01 || rx_old == 0x02)){// jetzt ist das radio an und der Modus is AUX - los
				
				send_key("F1");
			}
			if(rx == 0x02){//mfd hat AUX INFO TP gesendet (Verkehrsfunk) - wir warten.
				send_key("F8");
				rx_old = rx;
				sleep(1);
			}
			if(rx == 0x01){//String zum tacho - mp3player inaktiv
				rx_old = rx;
				int i;
				for(i = 0;i<17;i++){
					rx = serialGetchar(ser);
					if(rx==0){

						int j = 0;
						for(;j<17;j++){
							radio_string[j] = 0x00;
						}
					}else{
						radio_string[i] = rx;
						//printf("%c",rx);
					}
				}
				//printf("Radio: %s\n",radio_string);
				FILE *file = fopen("radio.txt","rw");
				if( file != NULL ){
					//printf("Radio: %s\n",radio_string);
					fprintf(file, radio_string);
					fclose(file);
				}
				rx = 0x01;
				serialFlush(ser); 
				return;
			}
			if(rx_old=='0' || (rx=='+' || rx=='-')){
				switch(rx){
					case SERIALaudio:
					case SERIALtone:{
						if(backcount>0){
							backcount = 0;
						}else{
							backcount = 3;
						}
						break;
					}
					case SERIALlight:{
						printf("SERIALlight\n");
						send_key("L");
						break;
					}
					case SERIALenter:{
						printf("SERIALenter\n");
						send_key("Return");
						break;
					}
					case SERIALnext:{
						printf("SERIALnext\n");
						send_key("F3");
						break;
					}
					case SERIALprev:{
						printf("SERIALprev\n");
						send_key("F2");
						break;
					}
					case SERIAL_1:{//prev album
						printf("SERIAL_1\n");
						send_key("F4");
						break;
					}
					case SERIAL_2:{//next album
						printf("SERIAL_2\n");
						send_key("F5");
						
						break;
					}
					case SERIAL_3:{//repeat
						printf("SERIAL_3\n");
						send_key("F1");
						break;
					}
					case SERIAL_4:{//prev artist
						printf("SERIAL_4\n");
						
						send_key("F6");
						break;
					}
					case SERIAL_5:{//next artist
						printf("SERIAL_5\n");
						send_key("F7");
						break;
					}
					case SERIAL_6:{ // mute navit speech output (i.e. toggle anouncer)
						printf("SERIAL_6\n");
						send_key("T"); 
						break;
					}
					case SERIALscan:{ 
						printf("SERIALscan\n");
						send_key("F12");
						break;
					}
					case SERIALas:{//cd mix
						printf("SERIALas\n");
						send_key("percent");
						break;
					}
					
					case SERIALinfo:{
						printf("SERIALinfo\n");
						break;
					}
					case SERIALback:{
						send_key("Escape");
						printf("SERIALback%i\n", backcount);
						if(backcount>0) backcount--;
						break;
					}
					case SERIALeject:{
						printf("SERIALeject\n");
						send_key("Delete");
					   
						break;
					}
					case SERIALflag:{
						printf("SERIALflag\n");
						//send_key("dollar");
						
						break;
					}
					case SERIALnavi:{
						printf("SERIALnavi\n");
						send_key("numbersign");
						break;
					}
					case SERIALtraffic:{
						printf("SERIALtraffic\n");
						//send_key("F1");
						
						break;
					}
					case SERIALtim:{
						printf("SERIALtim\n");
						//system("mpc toggle");
						break;
					}
					case SERIALplus:{
						int count = serialGetchar(ser);
						printf("SERIALplus\t%i\n", count);
						do{
							send_key("Page_Down");
						}while(--count);
						break;
					}
					case SERIALminus:{
						int count = serialGetchar(ser);
						printf("SERIALminus\t%i\n", count);
						do{
							send_key("Page_Up");
						}while(--count);
						break;
					}
					default:{
						//printf("default\n");
						serialFlush(ser);
						break;
					}
				}
			}
		}
		serialFlush(ser);
		rx_old = rx;
	}
}

int main (void){

	if (wiringPiSetupGpio () == -1)
		return 1 ;
	pinMode(READY_PIN, OUTPUT);
	//we use dtoverlay feature of the raspberry pi to drive this pin
	//dtoverlay=gpio-poweroff,gpiopin=18
	//so we do not need this anymore 
	pinMode(ACTIVE_PIN, OUTPUT);
	digitalWrite(ACTIVE_PIN, HIGH);
	
	// we use the shutdoun pin to catch the poweroff signal from the
	// interface board
	pinMode(SHUTDOWN_PIN, INPUT);
	
	// ready means that we finished
	digitalWrite(READY_PIN, HIGH);
	ser = serialOpen ("/dev/ttyAMA0", 38400);
	if(ser == -1){
		ser = serialOpen ("/dev/ttyS0", 38400); // maybe we are on the pi3
		if(ser == -1){
        		printf("error opening serial connection\nexiting....\n");
		}else{
			printf("we are on the pi3....\n");
		}
	}
        return 1;
    }
    
    system("xdotool mousemove 329 233");
	int cnt = 0;
	for (;;){
		cnt++;
		serial_task();
		if(cnt == 8){
			cnt = 0;
			if(digitalRead(SHUTDOWN_PIN)){
				system("mpc pause");
				delay (250);
				system("sudo shutdown -h now");
			}
		}
		delay (15);// mS
	}
	return -1;
}

