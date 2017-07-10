
///put pull down resistor on Res.Scan INPUT GND
/// consider putting pullup/down R on Galvo OUT
///solder an input to teensy from the galvo out, which can allow me to read the galvo time
//reference Timer Three Lib examples
///reference =  http://www.pjrc.com/teensy/td_libs_FreqCount.html ~~~~~~~~PUT INPUT PIN ON 13~~~~~!!!!!!!

//15.75Khz == 0.063492063492063 millis   // 15.75Khz == 15750hz // 15.75kHz == 63.492 in microseconds 
///freqCOUNT to X millis == X = freqCount * 1000

// --------Math For output vars == 
//KHZ to HZ == * 1000 --&&-- HZ to KHZ == / 1000
//Millis to Micros == *1000 --&&--  Micros to Millis == /1000
//KHz to Hertz == X Khz * 1000  // Hertz to KHz == X Hz / 1000
//Hz to Millis == 1 / X Hz * 1000


#include <TimerThree.h> //timer three functions in microseconds
#include <FreqCount.h> //freq count, counts in milliseconds?

//UNIVERSAL VARS//////
int millisCounter = 0;
int previousMillisCounter = 0;

//GET INPUT FREQUENCY VARS
int durationOfFreqCount = 1000;  ///library recs 1000 so that there is no scaling involved / may tweak this if ness.
boolean  resScanDelayedStart;  //there is signal on the output, and has been counted
unsigned long miliResFreqCOUNT = 0; //IN HERTZ

//WRITE OUTPUT FREQUENCY SIGNAL

//translation vars
float FRAME_RATE = 29.97;
float millisFrameRate;
float microsFrameRate;
volatile unsigned long  microResFreqCount = 0; ///holds freqCOUNT at micros Time


const int galvoPulsePin = A22;
byte GalvoReadPin = A23;
volatile float galvoFreq;  //var for galvo running freq. in millis?

boolean GalvoFrameRate;
boolean resFreqCounted;


void initalizeResScannerFrequency(){ 
FreqCount.begin(durationOfFreqCount);
///FREQUENCY COUNT IS COUNTING HERTZ?
if (FreqCount.available()){
   miliResFreqCOUNT = FreqCount.read();
   delay(durationOfFreqCount  * 6);
   resScanDelayedStart = true;   
}
}

//get the scanner freq.
void checkResScanFreq(){
  if (FreqCount.available() && GalvoFrameRate == false){ 
  miliResFreqCOUNT = FreqCount.read(); //freqCOUNT is in Hertz
   //do I need to count how long this measurement has taken and then stop it? if so inialize below code:
 // millisCounter = millis();
 //if (millisCounter >= durationOfFreqCountinMillis +1){
   FreqCount.end(); //PWM avail NOW
   resFreqCounted = true;               
  }  
}



//TIMER LIBRARY USES MICROSECONDS /
//translate incoming variable freCOUNT from millis to micro


void runGalvos(){
  if (GalvoFrameRate == false && resFreqCounted == true) {

    microResFreqCount = miliResFreqCOUNT * 1000;                  //make var INTO MICROS from milliseconds
    galvoFreq=  microResFreqCount / microsFrameRate;       // make micros

    Timer3.setPeriod( galvoFreq);
    GalvoFrameRate = true;
    resFreqCounted = false;
    Timer3.start();
  }
  
//this is waiting for 1 hz output and then signaling another read, and then resetting.  The code below is where 
///the duration of checking back in with the scanner can be reset to longer periods.

  while (GalvoFrameRate == true) {
    Timer3.pwm(galvoPulsePin, 50);
    galvoFreq = pulseIn(GalvoReadPin, HIGH);
    if ( galvoFreq >= GalvoFrameRate) {
      Timer3.stop();///may fuck up///look at whether frequecy will change?
      GalvoFrameRate = false;
    }
  }
}



void setup() {
pinMode(GalvoReadPin, INPUT);
initalizeResScannerFrequency(); 
millisFrameRate =  1 / FRAME_RATE * 1000;
microsFrameRate = millisFrameRate * 1000;
Timer3.initialize(millisFrameRate);
//setup 13 as an input here? check code examples
///setup pin galvoPulsePin as an output pin?
//set up input pin reading the output pin
}

void loop() {
  
///if resonant scanner not on, keep checking and waiting for it to resonate
if (resFreqCounted == false){
initalizeResScannerFrequency();
}

//if resonantor has started and had time to begin outputting proper frequencies
if (resFreqCounted == true){
checkResScanFreq();
runGalvos();
}
}

