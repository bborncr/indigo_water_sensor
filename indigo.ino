
//  UNO/MEGA/ESP w ethernet/WIFI sketch  by karl wachs July 26 2016 v 1.10.4
//  v1.11.6 
//  added soft reset function if no wifi request for 300 seconds (chnage that number or disable in section: """#define softReset   """ )
//  v1.10.5
//   fixed analog read pin was hardwired as A0 
//  v1.10.4
//   changed types for time variables:  long --> unsigned long 
//  v1.10.3
//  changed the onewire included  setup etc, 
//   fixed and tested  reboot option watchdog timer function
//  v1.10.2
//   fixed and tested  reboot option watchdog timer function
//  v1.10.1
//   added reboot option watchdog timer function
//  v1.9.2.
//   fixed debug --> DEBUG and removed debug print statement
//  v1.9.1.
//   added support MKR1000
//  fixed some smaller things
//  v1.8.3  
//  replaced min() max() with if(){}, some compiler versions did not like it
//  v1.8.2  
//  fixed removed additional & in analog write For S variables
// added: v 1.8.1:
//  1.added DS18B20 temp sensors example, will support several senors on one pin.
//     see http://milesburton.com/Dallas_Temperature_Control_Library#The_Library
//  2. added rise and fall time for ramp up/down/continuous, requires plugin V >= 2.6.1 
// 
//  3. added FREEdevice: this device has no limts from the plugin point of view. All pins (50xD,16xA,20xS) are available. YOU will need to manage in code. also IP# mac# wifi  etc.
//
//
//  command to be send:  curl 192.168.1.155"?--command-string--?" 
//  command string has the form command:pin=value&  eg:  command:pin=value1,value2&command:pin&command:pin=value1& 
//  
//  commands implemented:  
//  rd wr aw pg mU mD pD pU rU rD pC
//  rd:A1&rd:D3&         ==> will read analog pins 1 and digital pin 3     
//  wr:D1=0&wr:D4=0&     ==> set digital pins 1 to 0 and digital pin4 to 1
//  aw:D3=55&            ==> analogWrite(3,55)  will set digital pin3 to 55
//  pg:D5=O&pg:D4=I&pg:D7=U&   
//                       ==> will reprogram digital pin 5 to Output and digital pin 4 to Input and digital pin 7 to input_pullUp with pull up resistor
//  mU:D5=100&           ==> will program   digital pin 5 to do: 500msec up then down (digital write  set to 0/1)
//  mD:D2=5555&          ==> will program   digital pin 2 to do: 5555 msec down  then up (digital write  set to 0/1)
//  pU:D5=100,5555&      ==> will program   digital pin 5 to do: 100msec up ; then 5555 msec down and repeat (digital write  set to 0/1)
//  pD:D5=2000,3000&     ==> will program   digital pin 5 to do: 3000msec down ; then 2000 msec up and repeat (digital write  set to 0/1)
//  rC:D5=2000,3000,10,255&=> will program  digital pin 5 to do: set v=10; ramp up in 2 secs to 255; ramp down in 3 seconds to v=10 and repeat (analog write to set to 10/255)
//  rU:D5=2000,10,255&   ==> will program   digital pin 5 to do: set v=10; ramp up 2 seconds to v=255  (analog write  set to 10/255)
//  rD:D5=2000,10,255&   ==> will program   digital pin 5 to do: set v=255; ramp down 2 seconds to v=10  (analog write  set to 10/255)
//  rd:S1&rd:S2&         ==> will read contents of variable S1 and S2
//  aw:S1=abc -=; def&   ==> will write "abc -=; def" to varibale S1
//
//  pins can be D0..Dxx  A0.. Axx, S0..S4
//  Dx are digital i/o pins they can be read ==> 0/1; digital write 0/1 or analogWrite 0..255
//  Ax are anlog input pins delivery values 0..1023           
//  Sx are  String variables can be written to and read from  eg "231234 9p8137598734 kajshdlkjh=-:"  but NOT a "&" that will end the string
//
//  here a more complex example: 
//  command   curl http://192.168.1.155/"?rd:a1&wr:d5=1&pg:d7=O&wr:d7=1&?"    note the "
//  you can combine multiple commands, each command must end with a &
//  in python it is    
//  ret= urllib2.urlopen("http://ipnumber/?rd:a1&wr:d5=1&pg:d7=O&wr:d7=1&?").read()
//  would send  ret =">>rd:A1=311&wr:D5=1&pg:D7=O&wr:D7=0&wr:D7=1&"
//
//  excution times:  for mega reading 70 pins  ... use this for time out setting, default is 5 seconds
//   debug on ethernet: 0.4 secs
//   debug off          0.2 secs
//   debug on   wifi  : 0.9 secs
//   debug off        : 0.6 secs

//                      UNO reading 15 pins
//   debug off ethernet: 0.05 secs
//   debug on          : 0.2  secs
//   debug off wifi    : 0.3  
//   debug on          : 0.5 secs
//
//  this needs to be set :  1. #define DEBUG only for testing
//                          2. #define ETHERNET / WIFI    only for uno or mega
//                          3. #define UNO  MEGA ESP1 ESP16    --- in the tools menue in ARDUINO IDE you have to select the proper board also
//  for ethernet you need to set the IP number and the MAC number, WIFI gets it those from the router, but you have to set the SID and wifi password, You have to figure out the IP number either in debug mode or on the router
//
//           byte mac[] = { 0x55, 0x55, 0x55, 0x55, 0x55, 0x53 }; // pick one thats not on your network
//           IPAddress ip(192,168,1, 183);
//  some of the pins are disabled / enabled automatically: pin D7 D10,11,12,13 50,..are used to communicate to the ethernet/wifi boards
// 
//                                                        pins d0 d1 are used for usb communication, so in debug mode they are diabled, debug off they can used as all other pins
//
////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////!!!!!!!!!!!!!!
/// you need to adapt/change parameters in lines indicated with :    //<-----------------
////////////////////////////////////////////////////////////////////////////////////!!!!!!!!!!!!!!



// enable serial monitor for debugging, if in production do not send data to serial port
//#define DEBUG         //<-----------------
////




// enable watchdogtimer 
//#define WATCHDOGenabled         //<----------------- put // infront if you want to switch off
///////////////////////////// for watchdog timer, to enable: set maxWatchDogTimer >0 in millisecs  eg 1 minute =60000
#if defined(WATCHDOGenabled)
long watchDogTimer    = 0; // initialize  
long maxWatchDogTimer = 125000; // reboot if 2 minutes 5 secs w/o http request,  you could make it higher
int watchDogTimerPin  = 7; // set to pin that is connected to reset pin MAKE sure your do not use that pin in the plugin, for D0 use 0, here D7
                           // during  program load the cable must not be connected to reset pin 
#endif
////



//  select which board you use  --------------------------------
/// -------->  #include <xx>   does not work in 
//             #if defined(zz)   
//             #endif  
//             it is always included, have to do it manually



//////////////////////////////////////////////// ESP
//#define ESP1            //<-----------------
//#define ESP16             //<-----------------

// for both:
//#include <ESP8266WiFi.h>  //<----------------- for ESP1 and ESP16
////////////////////////////////////////////////


/////////////////////////////////////////////// FREEdevice
//#define FREEdevice       // <-- freely defined device. no restrictions from plugin YOU must define everything yourself
                         // if etehrnet, wifi ip numebr etc..
                         // comes with 50 defined D pins; 16 A pins; 20 S pins, like MEGA
//#include <SPI.h>       // needed for WIFI and ETHERNET, not for ESP

// for uno and mega:
//#define WIFI             //<-----------------  EITHER WIFI
//#include <WiFi.h>        //<-----------------

//#define ETHERNET       //<----------------- or ETHERNET
//#include <Ethernet.h>  //<-----------------
////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


/////////////////////////////////////////////// mkr1000
//#define mkr1000       // 
                         // if etehrnet, wifi ip numebr etc..
                         // comes with 8 defined D pins; 7 A pins; 20 S pins, 
//#include <SPI.h>       // needed for WIFI

//#define WIFI101
//#include <WiFi101.h>
////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>



//////////////////////////////////////////////// MEGA and UNO
//#define MEGA           //<----------either
#define UNO            //<--------or

#include <SPI.h>       // needed for WIFI and ETHERNET, not for ESP

// for uno and mega:
//#define WIFI             //<-----------------  EITHER WIFI
//#include <WiFi.h>        //<-----------------

#define ETHERNET       //<----------------- or ETHERNET
#include <Ethernet.h>  //<-----------------
////////////////////////////////////////////////
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
////////////  here YOU MUST ENTER YOUR NETWORK PARAMETERS !!
//////////////////////////////// configure network here





//////////
//#define softReset       //<-----------------   add// if you want to disable it.
#if defined(softReset) 
  unsigned longsoftResetTime = 300000;  // in milliseconds
  unsigned longsoftResetLast = 0;
#endif
//////////





#if defined(WIFI) || defined(ESP1) || defined(ESP16) || defined(mkr1000)
    char ssid[]     = "";                                  //<-----------------  your network SSID (name) 
    char pass[]     = "";                          //<----------------- your network password
    int keyIndex    = 0;                                    // your network key Index number (needed only for WEP)
    WiFiServer server(80);
#endif  
  
#if defined(ETHERNET) // this is for ethernet
    byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x56 };    //<----------------- pick one thats not on your network
    IPAddress ip(192,168,10, 48);                           //<----------------- pick one that works for you
    EthernetServer server(80);
#endif

////////////  here YOU MUST ENTER YOUR NETWORK PARAMETERS  END    !!
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

#if defined(WIFI) || defined(mkr1000)
  int status = WL_IDLE_STATUS;
#endif



// for DHT sensors///////////////////////////////////////
//#define DHTSensor
#if defined(DHTSensor)
  #include "DHT.h"
  #define DHTPIN 2     // what pin we're connected to
  #define DHTTYPE DHT22   // DHT 22  (AM2302)
  DHT dht(DHTPIN, DHTTYPE);
#endif

// for ONEWIRE ///////////////////////////////////////
//#define ONEWIRE
#if defined(ONEWIRE)
  #include <OneWire.h>
  #include <DallasTemperature.h>
// Data wire is plugged into pin x on the Arduino
  #define ONE_WIRE_PIN 2
   OneWire oneWire(ONE_WIRE_PIN);
   DallasTemperature sensors(&oneWire);

#endif  


// 
#if defined(UNO) || defined(MEGA) 
     #define maxAnalogValue 255
     #define serialBaudRate 57600
#endif  
// 
#if defined(mkr1000)  
     #define maxAnalogValue 1023
     #define serialBaudRate 57600
#endif  

// 
#if defined(FREEdevice) 
     #define maxAnalogValue 10000
     #define serialBaudRate 57600
#endif  


// 
#if defined(WIFI)
    #define WiFiDelay 2000
#endif  

#if defined(mkr1000)
    #define WiFiDelay 1000
#endif  

//
#if defined(ESP1) ||  defined(ESP16)
    #define WiFiDelay 1000
    #define maxAnalogValue   1023
    #define serialBaudRate 115200
#endif
    
char cmd[20];
char command[3];
int pin = 0 ;
int value1 = -1;
int value2 = -1;
int value3 = -1;
int value4 = -1;

String valueS="";
int cmdL = 0;
char c=' ';
char mode = 'I';
int kk=1;
int eIndex=-1;
int kIndex=-1;   // : index
int cIndex1=-1;  // comma index 1,2,3
int cIndex2=-1;
int cIndex3=-1;
int cc =0; // used for counter in doMyStuff
int dd =0; // used for counter in doMyStuff
String configured   ="no";

// free values to read from and write to
String S[20]={"","","","","","","","","","","","","","","","","","","",""};

String   out ;   // this can get: 68* 12 = 800+ for mega and 14*12 = 170 for uno 

#if defined(FREEdevice)
  int activeDPins =50;
  int activeAPins =16;
  char           dConfig[50] ; 
  char           pulseCMD[50] ;
  unsigned long  pulseEND[50] ;
  unsigned long  pulseV1[50] ;
  unsigned long  pulseV2[50] ;
  unsigned long  pulseMAX[50] ;
  unsigned long  pulseMIN[50] ;
  bool doNotUsePin[50]; 
#endif

#if defined(mkr1000)
  int activeDPins =21;
  int activeAPins =7;
  char           dConfig[21] ; 
  char           pulseCMD[21] ;
  unsigned long  pulseEND[21] ;
  unsigned long  pulseV1[21] ;
  unsigned long  pulseV2[21] ;
  unsigned long  pulseMAX[21] ;
  unsigned long  pulseMIN[21] ;
  bool doNotUsePin[21];
#endif


#if defined(MEGA)
  int activeDPins =50;
  int activeAPins =16;
  char           dConfig[50] ; 
  char           pulseCMD[50] ;
  unsigned long  pulseEND[50] ;
  unsigned long  pulseV1[50] ;
  unsigned long  pulseV2[50] ;
  unsigned long  pulseMAX[50] ;
  unsigned long  pulseMIN[50] ;
  bool doNotUsePin[50];
#endif

#if defined(UNO)
  int activeDPins =10;
  int activeAPins =6;
  char           dConfig[10] ; 
  char           pulseCMD[10] ;
   unsigned long  pulseEND[10] ;
   unsigned long  pulseV1[10] ;
   unsigned long  pulseV2[10] ;
   unsigned long  pulseMAX[10] ;
   unsigned long  pulseMIN[10] ;
  bool doNotUsePin[20];
#endif

#if defined(ESP16)
  int activeDPins =17;
  int activeAPins =1;
  char           dConfig[17] ; 
  char           pulseCMD[17] ;
   unsigned long  pulseEND[17] ;
   unsigned long  pulseV1[17] ;
   unsigned long  pulseV2[17] ;
   unsigned long  pulseMAX[17] ;
   unsigned long  pulseMIN[17] ;
  bool doNotUsePin[20];
#endif

#if defined(ESP1)
  int activeDPins =3;
  int activeAPins =1;
  char           dConfig[3] ; 
  char           pulseCMD[3] ;
   unsigned long  pulseEND[3] ;
   unsigned long  pulseV1[3] ;
   unsigned long  pulseV2[3] ;
   unsigned long  pulseMAX[3] ;
   unsigned long  pulseMIN[3] ;
  bool doNotUsePin[3];
#endif

int activeSPins =20;



////////////////////////////     put your stuff here, is called once per loop, make sure you add a delay at the end if you use an ESP module  

void doMyStuff(){ // example for DHT,  2 methods to get data to indigo
    configured   ="yes";
    S[1] = "7";
    if (false){   // switch on off: use true or false
            #if defined(DHTSensor)
                    // method  1:  just define S1 and S2 as input in indigo plugin , will get current stored value
                    cc +=1;
                    if (cc > 5000 ){ //  get sensor data every ~10 seconds - there is a delay of 2-3 msec every loop * 5000 ~ 10-15 seconds, this can be changed to faster or slower depending on your needs
                          cc =0 ;
                          S[1] = String(dht.readTemperature());
                          S[2] = String(dht.readHumidity());
                          S[0] = "";
                          S[3] = String(dht.readHumidity());
                    }
                     // method  2  only read sensor when asked, store in S1 and S2 and pick up next time ==> do a write S0=DHT, a read S1 and S2 (define S1 and S2 as input) will be executed every xx seconds
                    //if (S[0]="DHT" ){ // do it only when asked to do so
                          //S[1] = String(dht.readTemperature());
                          //S[2] = String(dht.readHumidity());
                      //S[0] = "";
                    //}
                    #if defined(ESP1) || defined(ESP16)
                      delay(1); 
                    #endif  
          #endif 

          #if defined(ONEWIRE)
            dd +=1;
            if (dd > 5000 ){  // do it every xx seconds only 
              dd=0; 
              sensors.requestTemperatures();
              S[1] = String(sensors.getTempCByIndex(0));
              // S[2] = String(sensors.getTempCByIndex(1)); // if you have a sensors attached to pin x
            }
          #endif 
          
      
    }
}

#if defined(WATCHDOGenabled)
void watchDog(){ 
          if (  millis() > 125000 ){ // dont to anything in the first 2 minutes
                  //Serial.print("millisec       = "); Serial.print(millis());Serial.print("watchDogTimer  = "); Serial.println(watchDogTimer);
                  //delay(500);
                  if (  maxWatchDogTimer > 0 and  ( millis() - watchDogTimer) > maxWatchDogTimer ){
                        if (watchDogTimerPin >= 0){
                              #if defined(DEBUG)  
                              Serial.print("rebooting timer= "); Serial.println(watchDogTimer);
                              Serial.print("millisec       = "); Serial.println(millis());
                              delay(1000);
                              #endif
                              pinMode(watchDogTimerPin, OUTPUT);
                              digitalWrite(watchDogTimerPin,LOW); // this will reset the board immediately 
                        }
                  }
          }         
            
}
#endif




/////////////////  dont change anything after this, should be the same for each device and program /////////////////////


void setup() {

              for (int i =0;i< activeDPins;i++){
                      dConfig[i]   ='0';
                      pulseCMD[i]  ='0';
                      pulseEND[i]  =0;
                      pulseV1[i]   =0;
                      pulseV2[i]   =0;
                      doNotUsePin[i]=false;
              }

              #if defined(WATCHDOGenabled)
                   digitalWrite(watchDogTimerPin, HIGH);
                   pinMode(watchDogTimerPin, OUTPUT);
              #endif
              
              #if defined(DHT)
                  dht.begin();   // start DHT
              #endif
              
              #if defined(ONEWIRE)
                  sensors.begin();
              #endif      
 
       
              #if defined(DEBUG)  
                  doNotUsePin[0] = true;
                  doNotUsePin[1] = true;
                  Serial.begin(serialBaudRate);
                  Serial.println("server starting");
              #endif
         
              #if defined(ETHERNET)  || defined(WIFI)   // these pins are used for communication to network board
                  doNotUsePin[7] = true;
                  doNotUsePin[10] = true;
                  doNotUsePin[11] = true;
                  doNotUsePin[12] = true;
                  doNotUsePin[13] = true;
               #endif
              #if defined(mkr1000) // these pins are used for communication to network board
                  //doNotUsePin[9] = true;
                  //doNotUsePin[11] = true;
                  //doNotUsePin[12] = true;
                  //doNotUsePin[13] = true;
                  //doNotUsePin[14] = true;
                  //doNotUsePin[15] = true;
                  doNotUsePin[16] = true;
                  doNotUsePin[17] = true;
                  doNotUsePin[20] = true;
                  doNotUsePin[21] = true;
                analogWriteResolution(10);
                analogReadResolution(10);

               #endif
                  
              // start the Ethernet connection and the server:
              #if defined(ETHERNET)
                  Ethernet.begin(mac, ip);
                  server.begin();
                  delay(1000);
                  // Open serial communications and wait for port to open:
                  #if defined(DEBUG)  
                    Serial.print("server is at ");
                    Serial.println(Ethernet.localIP());
                  #endif
              #endif  // ETHERNET
        
              // start wifi
              #if defined(WIFI) || defined (mkr1000)
                      // check for the presence of the shield:
                if (WiFi.status() == WL_NO_SHIELD) {
                        #if defined(DEBUG)  
                          Serial.println("WiFi shield not present"); 
                        #endif  
                        while(true);// don't continue:
                } 
                  // attempt to connect to Wifi network:
                  while ( status != WL_CONNECTED) { 
                        #if defined(DEBUG)  
                            Serial.print("Attempting to connect to SSID: ");
                            Serial.println(ssid);
                        #endif
                        status = WiFi.begin(ssid, pass);
                        // wait xx seconds for connection:
                        delay(WiFiDelay);
                  } 
                  server.begin();
                  printWifiStatus();
              #endif  // WIFI
        
              #if  defined(ESP1) || defined(ESP16)
                  // attempt to connect to Wifi network:
                  WiFi.begin(ssid, pass);
                  while (WiFi.status() != WL_CONNECTED) { delay(1000);} // wait 1 seconds for connection:
                  server.begin();
                  printWifiStatus();
             #endif  

              #if defined(softReset)
                   longsoftResetLast =  millis();
              #endif
 

}



void SpecialPredefinedOutputs() {
        for ( int j = 0; j< activeDPins; j++){  // check for all digital pins
            if (pulseCMD[j] != '0') {
                  //Serial.println("in pulse ");
                 //////  moment up or down 
                    if (pulseCMD[j] =='M'){  // UP  
                           if (millis() > pulseEND[j]) { pulseCMD[j] ='0'; digitalWrite(j,0);}
                    }
                    if (pulseCMD[j] =='m'){ // down
                           if (millis() > pulseEND[j]) { pulseCMD[j] ='0'; digitalWrite(j,1);}
                    }

                ///////  pulses up and down 
                    if (pulseCMD[j] =='P'){ // UP
                           if (millis() > pulseEND[j]) { pulseCMD[j] ='p'; digitalWrite(j,0); pulseEND[j] = millis()+ pulseV2[j];}
                    }
                    if (pulseCMD[j] =='p' ){ //down
                           if (millis() > pulseEND[j]) { pulseCMD[j] ='P'; digitalWrite(j,1); pulseEND[j] = millis()+ pulseV1[j];}
                    }   
                         
               /////// ramp up or down
                    if (pulseCMD[j] =='R'){ // UP
                            if (millis() > pulseEND[j] ) { pulseCMD[j]='0';analogWrite(j, pulseMAX[j]);}
                            else                         { analogWrite(j, (int) ( pulseMIN[j] + (pulseMAX[j] - pulseMIN[j]) * (1.-      ((float)pulseEND[j] - (float) millis()) / ((float)pulseV1[j])  ) ))  ;  }
                    } 
                    if (pulseCMD[j] =='r'){ // down
                            if (millis() > pulseEND[j] ) { pulseCMD[j]='0';analogWrite(j, pulseMIN[j]);}
                            else                         { analogWrite(j, (int) ( pulseMIN[j] + (pulseMAX[j] - pulseMIN[j]) * (         ((float)pulseEND[j] - (float) millis()) / ((float)pulseV1[j])  ) ))  ;  }
                    } 
                    
                 //////  ramp up AND down continously
                    if (pulseCMD[j] =='C'){  // UP  
                            if (millis() > pulseEND[j] ) { pulseCMD[j]='c';analogWrite(j, pulseMAX[j]);pulseEND[j] = millis()+ pulseV2[j];}
                            else                         { analogWrite(j, (int) ( pulseMIN[j] + (pulseMAX[j] - pulseMIN[j]) * (1.-      ((float)pulseEND[j] - (float) millis()) / ((float)pulseV1[j])  ) ))  ;  }
                    }
                    if (pulseCMD[j] =='c'){ // down
                            if (millis() > pulseEND[j] ) { pulseCMD[j]='C';analogWrite(j, pulseMIN[j]);pulseEND[j] = millis()+ pulseV1[j];}
                            else                         { //Serial.print(  pulseEND[j] - millis() );  Serial.print("  "); Serial.println(          (int) ( pulseMIN[j] + (pulseMAX[j]-pulseMIN[j])   * (    ((float)pulseEND[j] - (float) millis()) / ((float)pulseV2[j])  ) ));
                                                           analogWrite(j, (int) ( pulseMIN[j] + (pulseMAX[j]-pulseMIN[j])   * (         ((float)pulseEND[j] - (float) millis()) / ((float)pulseV2[j])  ) ));} 
                    }
            }       
      }
}



void doNextCMD(){
           
                if ( (configured == "no")  and (strcmp(command,"pg") !=0 )  ) {
                    out=">>notConfigured";
                    #if defined(DEBUG)
                      Serial.println(">>notConfigured");
                    #endif
                    return;
                }
                if (    strcmp(command,"st") ==0  and configured =="yes" ) { out=">>Configured"; return; }
                if (    strcmp(command,"st") ==0  and configured =="no"  ) { out=">>notConfigured"; return; }
                
                if (    cmd[3]=='I'  and pin >=activeAPins ) { out+=command;out+=":I";out+=String(pin);out+="=outOfRange&"; return;}     // for I2C  not implemeted yet 
                if (    cmd[3]=='A'  and pin >=activeAPins ) { out+=command;out+=":A";out+=String(pin);out+="=outOfRange&"; return;}     // analog in  
                if (    cmd[3]=='D'  and pin >=activeDPins ) { out+=command;out+=":D";out+=String(pin);out+="=outOfRange&"; return; }    // digital 
                if (    cmd[3]=='S'  and pin >=activeSPins ) { out+=command;out+=":S";out+=String(pin);out+="=outOfRange&"; return; }    // String
                if (    cmd[3]=='D'  and doNotUsePin[pin]  ) { out+=command;out+=":D";out+=String(pin);out+="=notUseable&"; return; }    // digital  
                configured="yes";   
           
                if (    strcmp(command,"cf") ==0   or  strcmp(command,"lc")==0    ) {
                           if (  strcmp(command,"cf") ==0   ) {
                                    #if defined(DEBUG)
                                    Serial.println("command = cf - ");
                                    #endif
                                    for (int i=0;i<activeDPins;i++){out+="cf:D"+String(i)+'='+dConfig[i]+'&';}
                           }
                           return ;
                } 
                if (  strcmp(command, "i2") ==0  ){  // not implemented yet, dummy function
                               if (cmd[3]=='I') {    //= read i2c devices 
                                    return;
                               }
                }
                if (strcmp(command, "pg")==0 and (cmd[3]=='D' or cmd[3] == 'S' )){
                                    #if defined(DEBUG) 
                                    Serial.println("command =pg  "+ String(pin) +" to mode=" + mode +", cmd="+ cmd);
                                    #endif
                                    if (cmd[3]=='D'){
                                        if (doNotUsePin[pin] ) { return; }        
                                        if (mode =='I'){ pinMode(pin, INPUT); dConfig[pin] ='I';out+=cmd ;}
                                        if (mode =='O'){ pinMode(pin, OUTPUT);dConfig[pin] ='O';out+=cmd ;}
                                        if (mode =='U'){ pinMode(pin, INPUT_PULLUP);dConfig[pin] ='U';out+=cmd ;}
                                        pulseCMD[pin]='0';
                                        pulseEND[pin]=0;
                                        return;
                                    }    
                                    if (cmd[3]=='S'){return;}
                }
                if (  strcmp(command, "rd")==0  ){
                             if ( cmd[3]=='A') {  // ANALOG
                                    int val = analogRead(pin); //  for ESP has only analog one pin and it must be address as A0
                                    #if defined(DEBUG) 
                                    Serial.print("command = A-read - "); Serial.println(val); 
                                    #endif
                                    out+="rd:A"+String(pin)+'='+String(val)+'&';
                                    return;
                             }
                             if (cmd[3]=='D'){  // Digital
                                    int val = digitalRead(pin);
                                    #if defined(DEBUG) 
                                    Serial.print("command = D-read - ");Serial.println(val);
                                    #endif
                                    out+="rd:D"+String(pin)+'='+String(val)+'&';
                                    pulseCMD[pin]='0';
                                    pulseEND[pin]=0;
                                    return;
                            } 
                            if (cmd[3]=='S'){  // this will return the S[pin] string
                                    #if defined(DEBUG) 
                                    Serial.print("command = S-read - ");Serial.println(S[pin]);
                                    #endif
                                    out+="rd:S"+String(pin)+'='+S[pin]+'&';
                                    return;
                            } 
                }
  

                           
                if (strcmp(command, "aw") ==0 and cmd[3]=='S'){
                                    S[pin]= valueS;  
                                    Serial.print("command = S-write - ");Serial.print(S[pin]);  Serial.println(valueS);                                                  
                                    out+=cmd; 
                                    return;
                }  

                if ( cmd[3]=='D'){
                  
                        if (strcmp(command, "wr")==0 ){
                                    //#if defined(DEBUG) 
                                    Serial.print("command = wr -");Serial.println(value1);
                                    //#endif
                                    digitalWrite(pin,value1);                                                       
                                    pulseCMD[pin]='0';
                                    out+=cmd; 
                                    return;
                        }   
                  
                        if (strcmp(command, "aw") ==0 and cmd[3]=='D'){
                                    #if defined(DEBUG) 
                                    Serial.print("command = aw, value=" );Serial.println(value1);  
                                    #endif
                                    analogWrite(pin,value1);                                                       
                                    pulseCMD[pin]='0';
                                    out+=cmd; 
                                    return;
                        }  
                           
                        if (strcmp(command, "pU")==0 ){   // pulse Down / pulse Up
                                    pulseEND[pin]= millis()+ value1;
                                    pulseV1[pin]=value1;
                                    pulseV2[pin]=value2;
                                    pulseCMD[pin]='P';
                                    //analogWrite(pin,maxAnalogValue);
                                    digitalWrite(pin,1);
                                    #if defined(DEBUG) 
                                    Serial.println("command = pU"); 
                                    #endif
                                    out+=cmd;
                                   return;
                        }
                               
                        if (strcmp(command, "pD")==0 ){   // pulse up / pulse down
                                    pulseEND[pin]= millis()+ value2;
                                    pulseV1[pin]=value1;
                                    pulseV2[pin]=value2;
                                    pulseCMD[pin]='p';
                                    //analogWrite(pin,0);
                                    digitalWrite(pin,0);
                                    #if defined(DEBUG) 
                                    Serial.println("command = pD"); 
                                    #endif
                                    out+=cmd; 
                                    return;
                        }
                               
                        if (strcmp(command, "mU")==0 ){  // moment UP
                                    pulseEND[pin]= millis()+ value1;
                                    pulseCMD[pin]='M';
                                    #if defined(DEBUG) 
                                    Serial.println("command = mU");Serial.println(value1);
                                    #endif
                                    digitalWrite(pin,1);
                                    out+=cmd; 
                                    return;
                        }
                               
                        if (strcmp(command, "mD")==0 ){  // moment Down
                                    pulseEND[pin]= millis()+ value1;
                                    pulseCMD[pin]='m';
                                    #if defined(DEBUG) 
                                    Serial.println("command = mD");Serial.println(value1); 
                                    #endif
                                    digitalWrite(pin,0);
                                    out+=cmd; 
                                    return;
                        }                  
                        if (strcmp(command, "rD")==0 ){  // ramp down
                                    pulseEND[pin] = millis()+ value1;
                                    pulseV1[pin]  = value1;
                                    if (value3 == -1){ value3 = maxAnalogValue;}
                                    if (value2 <0){value2 =0;};if (value2> maxAnalogValue){ value2 = maxAnalogValue;}; pulseMIN[pin] =value2;
                                    if (value3 <0){value3 =0;};if (value3> maxAnalogValue){ value3 = maxAnalogValue;}; pulseMAX[pin] =value3;
                                    pulseCMD[pin] = 'r';
                                    #if defined(DEBUG) 
                                    Serial.print("command = rD  v1 " );Serial.print(value1); Serial.print(" MIN "); Serial.print(pulseMIN[pin]); Serial.print(" max "); Serial.println(pulseMAX[pin]); 
                                    #endif
                                    analogWrite(pin,pulseMAX[pin]);
                                    out+=cmd; 
                                    return;
                        }                  
                        if (strcmp(command, "rU")==0 ){  // ramp up
                                    pulseEND[pin] = millis()+ value1;
                                    pulseV1[pin]  = value1;
                                    if (value3 == -1){ value3 = maxAnalogValue;}
                                    if (value2 <0){value2 =0;};if (value2> maxAnalogValue){ value2 = maxAnalogValue;}; pulseMIN[pin] =value2;
                                    if (value3 <0){value3 =0;};if (value3> maxAnalogValue){ value3 = maxAnalogValue;}; pulseMAX[pin] =value3;
                                    pulseCMD[pin] = 'R';
                                    #if defined(DEBUG) 
                                    Serial.print("command = rU  v1 " );Serial.print(value1); Serial.print(" MIN "); Serial.print(pulseMIN[pin]); Serial.print(" max "); Serial.println(pulseMAX[pin]); 
                                    #endif
                                    analogWrite(pin,pulseMIN[pin]);
                                    out+=cmd; 
                                    return;
                       }
                        if (strcmp(command, "rC")==0 ){   // rampup AND down continuously
                                    pulseEND[pin]= millis()+ value1;
                                    pulseV1[pin]  = value1;
                                    pulseV2[pin]  = value2;
                                    if (value4 == -1){ value4 = maxAnalogValue;}
                                    if (value4 <0){value4 =0;};if (value4> maxAnalogValue){ value4 = maxAnalogValue;}; pulseMAX[pin] =value4;
                                    if (value3 <0){value3 =0;};if (value3> maxAnalogValue){ value3 = maxAnalogValue;}; pulseMIN[pin] =value3;
                                    pulseCMD[pin]='C';
                                    analogWrite(pin,pulseMIN[pin]);
                                    #if defined(DEBUG) 
                                    Serial.print("command = rC  v1 " );Serial.print(pulseV1[pin]); Serial.print(" V2 "); Serial.print(pulseV2[pin]); Serial.print(" MIN "); Serial.print(pulseMIN[pin]); Serial.print(" MAX "); Serial.println(pulseMAX[pin]); 
                                    #endif
                                    out+=cmd; 
                                   return;
                        }
                        
                }
           
}


 
int findCinCMD( char x){
         for ( int i=0;i<cmdL;i++){ if (cmd[i] == x){ return i;}}
         return -1;
}

void parseCMD() {  // structure: rd:D09&  or wr:D55=1234& or mU:d3=1234,4321& or pg:d33=I&
         int i;  String xx=""; int i1; int i2;
         value1 = -1; value2 = -1; value3 = -1; value4 = -1; mode=' ';

         command[0]=cmd[0]; command[1]= cmd[1];command[2]='\0';
         
         //Serial.print("\n ki: ");Serial.print(kIndex);Serial.print(" ei: ");Serial.print(eIndex);Serial.print(" c: ");Serial.print(cIndex);

          if (eIndex==-1){eIndex=cmdL;}; if (cIndex1==-1){cIndex1=cmdL;} // if no "=" or "," signs
         
         // get pin #  is integer 0.. 99 allowed                    
         if (eIndex >  0) { xx=""; for (i = kIndex+2; i< eIndex; i++ )  {  xx+=cmd[i]; };      pin=xx.toInt();  }

         // now get values
         if (eIndex>3){ // any "=" sign?
                     if (eIndex == cmdL-1){return;} // no values given
                     
                     // this is for programming to Input or Output
                     if (cmd[eIndex+1]  =='I'){mode = 'I';return;}    
                     if (cmd[eIndex+1]  =='O'){mode = 'O';return;}
                     if (cmd[eIndex+1]  =='U'){mode = 'U';return;}

                     // get integer numbers may by one or 2 , if "," present then it has 2 numbers
                     if (cmd[3]=='S'){
                              xx=""; for ( i = eIndex+1; i< cmdL;      i++ ){ xx+=cmd[i];}; valueS=xx;           // only one value for strings
                              return;
                     }

                     if ( eIndex < cmdL-1){ 
                          if (cIndex1>0 ){ // we have 2,3,4 numbers separated by a ","
                                  xx=""; for ( i = eIndex+1; i< cIndex1;                 i++ ){ xx+=cmd[i];}; value1=xx.toInt(); // at least 2
                                  i1=cmdL; if (cIndex2 !=-1) { i1= cIndex2; }  
                                  xx=""; for ( i = cIndex1+1; i< i1;                     i++ ){ xx+=cmd[i];}; value2=xx.toInt(); // 
                                  
                                  if (cIndex2>0){ 
                                          i2=cmdL; if (cIndex3 !=-1){i2=cIndex3;}
                                          xx=""; for ( i = cIndex2+1; i< i2;             i++ ){ xx+=cmd[i];}; value3=xx.toInt(); 
                                          
                                          if (cIndex3>0){
                                                xx=""; for ( i = cIndex3+1; i< cmdL;     i++ ){ xx+=cmd[i];}; value4=xx.toInt(); 
                                          }      
                                  }                                 
                                  return;
                          }
                     }
                     
                     // just one number/ string, no "," present
                    xx=""; for ( i = eIndex+1; i< cmdL;                                 i++ ){ xx+=cmd[i];}; value1=xx.toInt();   
                    return; 
         }    
         return ;   
}


void loop(){
       #if defined(ESP1) || defined(ESP16)
       delay(5); 
       #endif   
       
       #if defined(WATCHDOGenabled)
            watchDog();
       #endif
      
       SpecialPredefinedOutputs();
       
       #if defined(ESP1) || defined(ESP16)
       delay(5); 
       #endif     
       
      // special stuff if you want to implement your things 
      doMyStuff();
      
      // listen for incoming clients
      #if defined(WIFI)  || defined(ESP1) || defined(ESP16) || defined(mkr1000)
        WiFiClient client = server.available();
      #endif        
      
      #if defined(ETHERNET)           
       EthernetClient client = server.available();
      #endif
 
      #if defined(softReset)
         if (millis() - longsoftResetLast  > longsoftResetTime) {asm volatile ("  jmp 0");}
     #endif
     
      //Serial.print("a") ;       
      if (client) {

                  #if defined(softReset)
                      longsoftResetLast =  millis();
                  #endif
 
                  #if defined(DEBUG) 
                    Serial.println("new client"); 
                  #endif
                  char c4 =' '; char c3=' ' ; char c2=' '; char c1=' ';  c=' ';int GETfound =0;int newLine=0;cIndex1=-1;cIndex2=-1;cIndex3=-1;kIndex=-1;eIndex=-1;
                  while (client.connected()) {
                            if (client.available()) {
                                      c4=c3;c3=c2;c2=c1;c1=c;
                                      c = client.read();
                                      #if defined(DEBUG)
                                       Serial.write(c);
                                      #endif
                                      #if defined(WATCHDOGenabled)
                                      watchDogTimer = millis();
                                      #endif

                                      if (GETfound ==0){ if ( c4=='G' and c3=='E' and c2=='T' and c1 ==' ' and c=='/'){    //   string  "GET /"  found, now get data
                                                              #if defined(DEBUG)
                                                                Serial.println(">>GET /<<  Found");
                                                              #endif
                                                              GETfound =1; c = client.read();// skip "?" in "GET /?"
                                                              #if defined(DEBUG)
                                                                Serial.write(c);
                                                              #endif
                                                              cmdL=-1; c = client.read(); out =">>"; // get first character
                                                              #if defined(DEBUG)
                                                                Serial.write(c);  
                                                              #endif
                                                         } // skip "?" and read first char (avoid another loop)
                                      } 
                                      if (GETfound ==1) { 
                                               if (c != '?' and out !=">>notConfigured" ) { // skip if not configured, otherwise read until ending "?"
                                                    cmdL+=1;
                                                    cmd[cmdL]=c; 
                                                    
                                                    if (c==':' and kIndex  ==-1) {kIndex=cmdL;} //  use the first occurence
                                                    if (c=='=' and eIndex  ==-1) {eIndex=cmdL;}
                                                    if ( c==',' ){
                                                        if ( cIndex1  ==-1) {cIndex1=cmdL;}
                                                        else{
                                                              if (cIndex2 ==-1) {cIndex2=cmdL;}
                                                              else {
                                                                     if (cIndex3 ==-1) {cIndex3=cmdL;}
                                                              }
                                                        }
                                                    }
                                                    
                                                    if (c=='&'){
                                                      cmd[cmdL+1]= '\0';
                                                      parseCMD(); 
                                                            #if defined(DEBUG)
                                                                Serial.print("next cmd: ");Serial.println(command);
                                                              #endif
                                                       
                                                      doNextCMD();
                                                      cmdL=-1;cIndex1=-1;cIndex2=-1;cIndex3=-1;eIndex=-1;kIndex=-1;}
                                               }
                                               else{ GETfound=2;
                                                  #if defined(DEBUG)
                                                      Serial.println(" all data read" );  
                                                  #endif
                                               }
                                      }
                            }           
                            if (GETfound ==2) {  // here we send the header and the composed data
                                      GETfound=3;
                                      client.println("HTTP/1.1 200 OK");
                                      client.print("Content-Length: "); client.println(out.length());
                                      client.println("Content-Type: text/html");
                                      client.println();
                                      client.print(out);
                                      
                                      delay(5); 
                                      #if defined(DEBUG)
                                          Serial.println(out.length());
                                          Serial.println(out);
                                          Serial.println("finished processing"); 
                                      #endif
                             } 
                            if (GETfound ==3) {  // read the rest of the message until \n\r thats the end of it
                                      if (c=='\n'){newLine+=1;}
                                      else if (c== '\r' && newLine==1){client.stop();break;}
                                      if (c != '\n'){newLine=0;}      
                            }          
                 }
           #if defined(ESP1) || defined(ESP16)
                  yield();// give ESP time to do its background stuff
           #endif   
           #if defined(DEBUG) 
                  Serial.println("client disconnected"); 
         #endif
 
        }
}


#if defined(WIFI) || defined(ESP1) || defined(ESP16)
void printWifiStatus() {
          // print the SSID of the network you're attached to:
          #if defined(DEBUG)
                Serial.print("SSID: ");
                Serial.println(WiFi.SSID());
              
              
                // print your WiFi shield's IP address:
                IPAddress ip = WiFi.localIP();
                Serial.print("IP Address: ");
                Serial.println(ip);
              
                // print the received signal strength:
                long rssi = WiFi.RSSI();
                Serial.print("signal strength (RSSI):");
                Serial.print(rssi);
                Serial.println(" dBm");
          #endif
}
#endif




