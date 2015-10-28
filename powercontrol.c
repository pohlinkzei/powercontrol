/*
 * powercontrol on raspberry pi

 * Compile with:
 * gcc -lxdo -lwiringpi powercontrol.c
 * https://github.com/jordansissel/xdotool
 */ 
//git clone git://git.drogon.net/wiringPi


 
#include <stdio.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <xdo.h>

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
//#define ACTIVE_PIN 18
#define READY_PIN 27

int ser;
char radio_string[17] = {0,};
int ff_count = 0;
unsigned char rx_old = 0x00;
int backcount = 0;
int status;
int status_off;
int no_ser_count = 0;

Window *window; 
xdo_t *xdo; 
xdo_search_t search = {0};
int numResults = 0;

void send_key(char* key){
	if (numResults > 0) { 
		xdo_send_keysequence_window(xdo, *window, key, 10); 
	} 
}

void serial_task(void){
	if(numResults < 1){
		xdo_search_windows(xdo, &search, &window, &numResults);
		printf("Found %d windows.\n", numResults); 
	}
   
    if(serialDataAvail(ser)>0){
        unsigned char rx = serialGetchar(ser);
        serialPutchar(ser, rx);
		//printf("Taste: %x\n",rx);
        if(rx != '0'){
            printf("Taste: %c,%i\n",rx,rx);
        }
        if(rx == 0xFF){// radio ausgeschaltet?
            ff_count++;
            printf("FF_count: %i\n",ff_count);
        }else{
            ff_count = 0;

            if(rx != rx_old){


                if(rx == 0x01){//rpimfdinterface startete den pi nach zv auf, radio ist aber noch nicht eingeschaltet - wir warten.
                    system("mpc pause");
                    sleep(1);
                }
                if(rx == '0' && rx_old == 0x01){// jetzt ist das radio an - los
                    system("mpc play");
                }
                if(rx == 0x02){//mfd hat AUX INFO TP gesendet (Verkehrsfunk) - wir warten.
                    system("mpc pause");
                    sleep(1);
                }else if(rx == '0' && rx_old == 0x02){// jetzt ist das radio an - los
                    system("mpc play");
                }


                switch(rx){
                    //*
                    int i;
                    case 0x01:{//String zum tacho - mp3player inaktiv
                        for(i = 0;i<17;i++){
							int pos = 0;
                            rx = serialGetchar(ser);
                            if(rx==0){
                                printf("%s\n",radio_string);
								int j = 0;
                                for(;j<17;j++){
									radio_string[i] = 0x00;
								}
                            }else{
                                radio_string[pos++] = rx;
                            }

                        }
						// TODO: an Navit übergeben, damit radio_string an MFA gesendet werden kann
                        rx = 0x01;
                        serialFlush(ser);
                        //return;
                        break;
                    }
                    //*/
                    case SERIALlight:{
						printf("SERIALlight\n");
						send_key("l");
						break;
					}
                    case SERIALenter:{
						printf("SERIALenter\n");
						send_key("Return");
                        break;
                    }

                    case SERIALnext:{
						printf("SERIALnext\n");
                        send_key("F2");
                        break;
                    }
                    case SERIALprev:{
						printf("SERIALprev\n");
                        send_key("F3");
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
                        //system("mpc repeat");
                        
                        send_key("F1");
                        break;
                    }
                    case SERIAL_4:{//prev artist
						printf("SERIAL_4\n");
						
                        send_key("F6");
                        //system("mpc clear");
                        //CurrentPlayList = prev_artist(CurrentPlayList);
                        break;
                    }
                    case SERIAL_5:{//next artist
						printf("SERIAL_5\n");
                        send_key("F7");
                        //system("mpc clear");
                        //CurrentPlayList = next_artist(CurrentPlayList);
                        break;
                    }
                    case SERIAL_6:{ // mute navit speech output (i.e. toggle anouncer)
						printf("SERIAL_6\n");
                        send_key("T"); 
                        //system("mpc single");
                        break;
                    }
                    case SERIALscan:{ //
                    
                       
						printf("SERIALscan\n");
                        send_key("F12");
                        break;
                    }
                    case SERIALas:{//cd mix
						printf("SERIALas\n");
						
                        send_key("%");
                        //system("mpc random");
                        break;
                    }
                    case SERIALaudio:{
						printf("SERIALaudio %i\n", backcount);
                        if(backcount>0){
                            backcount = 0;
                        }else{
                            backcount = 3;
                        }
                        break;
                    }
                    case SERIALtone:{
						printf("SERIALtone%i\n", backcount);
                        if(backcount>0){
                            backcount = 0;
                        }else{
                            backcount = 3;
                        }
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
                        send_key("del");
                       
                        break;
                    }
                    case SERIALflag:{
						printf("SERIALflag\n");
						send_key("$");
                        
                        break;
                    }
                    case SERIALnavi:{
						printf("SERIALnavi\n");
                        send_key("§");
                        break;
                    }
                    case SERIALtraffic:{
						printf("SERIALtraffic\n");
                        send_key("F1");
                        
                        break;
                    }
                    case SERIALtim:{
						printf("SERIALtim\n");
                        system("mpc toggle");
                        break;
                    }
                    case SERIALplus:{
						printf("SERIALplus\n");
                        send_key("Page_up");

                        break;
                    }
                    case SERIALminus:{
						printf("SERIALminus\n");
                        send_key("Page_down");

                        break;
                    }
                    default:{
						printf("default\n");
                        serialFlush(ser);
                        break;
                    }
                }
            }
            serialFlush(ser);
            rx_old = rx;
        }
        if(ff_count > 10){
            // save playlist!
            system("mpc pause");
            //we use dtoverlay feature of the raspberry pi to drive this pin
            //so we do not need this anymore 
            //digitalWrite(ACTIVE_PIN, LOW);
            digitalWrite(READY_PIN, HIGH);
            status = status_off;
            printf("Status: OFF!\n");
            return;
        }
        no_ser_count = 0;
    }else{
        no_ser_count++;
        if(no_ser_count == 400){
            //we are idle? no info from navi? try to reactivate it.
            //serialPutchar(ser, 0xFF);
        }
    }
}




int main (void){

	xdo = xdo_new(":0.0"); 
	search.winname = "Navit"; 
	search.winclassname = "Navit"; 
	search.winclass = "Navit"; 
	search.require = SEARCH_ANY; 
	search.searchmask = SEARCH_NAME | SEARCH_CLASS | SEARCH_CLASSNAME; 
	search.max_depth = -1; 
	
	if (wiringPiSetupGpio () == -1)
		return 1 ;
	
	pinMode(READY_PIN, OUTPUT);
	//we use dtoverlay feature of the raspberry pi to drive this pin
	//dtoverlay=gpio-poweroff,gpiopin=18
	//so we do not need this anymore 
	//pinMode(ACTIVE_PIN, OUTPUT);
	//digitalWrite(ACTIVE_PIN, HIGH);
	
	// we use the shutdoun pin to catch the poweroff signal from the
	// interface board
	pinMode(SHUTDOWN_PIN, INPUT);
	
	// ready means that we finished
	digitalWrite(READY_PIN, HIGH);
	ser = serialOpen ("/dev/ttyAMA0", 38400);
	if(ser == -1){
        printf("error opening /dev/ttyAMA0!\nexiting....");
        return 1;
    }
    
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
		delay (75);// mS
	}
  return -1;
}

