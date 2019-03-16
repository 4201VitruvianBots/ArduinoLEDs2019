#include <Adafruit_NeoPixel.h>

#define outPin 6
#define DIO12 12
#define DIO11 11
#define DIO10 10
#define DIO9 9 
unsigned long prevMillis = 0;

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(25, outPin, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  pinMode(DIO12, INPUT);  //initialises the DIO pins connected to the NavX as inputs
  pinMode(DIO11, INPUT);
  pinMode(DIO10, INPUT);
  pinMode(DIO9, INPUT);
  strip.begin();  //start communication with NeoPixel strip
  strip.setBrightness(50);  //set all pixels to 1/2 brightness
  strip.show(); // Initialize all pixels to 'off'
}

void loop(){
  bool ShifterState = digitalRead(DIO12); //constantly gets the values of all DIO inputs
  bool DIO11State = digitalRead(DIO11);
  bool DIO10State = digitalRead(DIO10);
  bool DIO9State = digitalRead(DIO9);
  int LEDColour = 0;  //Tracks what colour we want our LED to be, is defined by the above three pins as a number in binary.
  if(DIO11State){     //If the third binary digit is 1, that signifies a "4"
    LEDColour = LEDColour + 4; 
  }
  if(DIO10State){      //If the second binary digit is 1, that signifies a "2"
    LEDColour = LEDColour+2;
  }
  if(DIO9State){        // If the first binary digit is 1, that signifies "1"
    LEDColour = LEDColour+1;
  }
  if(ShifterState){ //If the shifters are in the lowest gear, we use the blinky lights.
    switch(LEDColour){
      case 0: //if the robot's disabled, we run the kittBounce animation green & with 30ms delay
        kittBounce(0,255,0,30);
      break;
      case 1:
        TwoPixelBlink(strip.Color(0,64,255),100);  //If the robot's in manual hatch mode, blink the lights blue
      break;
      case 2:
        TwoPixelBlink(strip.Color(255,8,0),100); //If the robot's in cargo mode, blink the lights red
      break;
      case 3:
        TwoPixelBlink(strip.Color(255,255,0),100); //If the robot's in hatch mode, blink the lights yellow
      break;
      case 4:
        TwoPixelBlink(strip.Color(0,255,0),100);  //If the robot's acquired a hatch, blink the lights green
      break;
      default:
        TwoPixelBlink(strip.Color(128,0,255),100);  //If there's something wrong, blink the lights purple. 
      break;
    }
  }
  else{ //if the robot's in the less low gear, we do basically the same thing but with solid lights
        switch(LEDColour){
      case 0: //if the robot's disabled, we run the kittBounce animation green & with 30ms delay
        kittBounce(0,255,0,30);
      break;
      case 1:
        fullStripSet(strip.Color(0,64,255));  //If the robot's in manual hatch mode, turn the lights blue
      break;
      case 2:
        fullStripSet(strip.Color(255,8,0)); //If the robot's in cargo mode, turn the lights red
      break;
      case 3:
        fullStripSet(strip.Color(255,255,0)); //If the robot's in hatch mode, turn the lights yellow
      break;
      case 4:
        fullStripSet(strip.Color(0,255,0));  //If the robot's acquired a hatch, turn the lights green
      break;
      default:
        fullStripSet(strip.Color(128,0,255));  //If there's something wrong, turn the lights purple. 
      break;
    }
  }
}
//Just makes the entire strip one colour. Why even buy NeoPixels at this point?
void fullStripSet(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) { //repeats the below command for every pixel on the strip
    strip.setPixelColor(i, c);
  }
  strip.show();     //sets the whole strip in one go from the changes in memory
}

//Do the colorWipe thing in reverse
void revColorWipe(uint32_t c, uint8_t wait){    //c is the color, wait is the number of millisecodns in between cycles
  for(uint16_t i=strip.numPixels(); i>0; i--){  //i starts at the top, and until it's 0, subtracts 1 every cycle
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);      //delay, ewwww
  }
}
//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}
//Flashing for use as a near-instant indicator
void TwoPixelBlink(uint32_t c, uint8_t wait){
  static unsigned long lastUpdate = 0;  //stores how many milliseconds the last cycle was at
  unsigned long now = millis();  //not really sure why I have to use a variable instead of just millis(). Best practices?
  uint16_t numOfSegments = strip.numPixels()/2; //stores how many segments there're supposed to be
  static bool even = 0; //allows the strip to remember if it was on the odd or even segments.
  if(now > lastUpdate + wait){     //checks if the perscribed # of ms has passes since last go
    if(even){ //checks if we're on the odd or even pixels
      for(uint16_t i=0; i<((strip.numPixels()/4)+1); i++){//divies the strip up into four-led segments, plus a bit b/c not everything is divisible
        strip.setPixelColor((i*4), c);    //turns the first two pixels in each segment on to the color you picked
        strip.setPixelColor((i*4)+1, c);  
        strip.setPixelColor((i*4)+2, 0);  //and the second two off
        strip.setPixelColor((i*4)+3, 0); 
        strip.show();
      }
     even = 0; //sets even to off so the next cycle will turn the odd set of pixels on & off 
    }
    else{ //if we're on the odd pixels
      for(uint16_t i=0; i<((strip.numPixels()/4)+1); i++){
        strip.setPixelColor((i*4), 0);    //does the even thing, but with the other two pixels.
        strip.setPixelColor((i*4)+1, 0);  
        strip.setPixelColor((i*4)+2, c); 
        strip.setPixelColor((i*4)+3, c); 
        strip.show();
      }
     even = 1; 
    }
    lastUpdate = now; //registers when the cycle occured
  }
}
//"Like Kitt from Knight Rider"
void kittBounce(int r, int g, int b, uint8_t wait){ //uses RGB as arguments instead of C because I don't know how to separate them
  static unsigned long lastUpdate = 0;
  unsigned long now = millis();
  static uint16_t currentLED = 0;       //this is because I'm not using for loops for the LED state anymroe, is the LED number we're on
  static bool reverse = 0;              //allows the strip to "bounce" instead of just go one way
  if(now > lastUpdate + wait){
    if(reverse){                   //checks if we're supposed to be going from top to bottom
      strip.setPixelColor(currentLED, strip.Color(r,g,b));
      strip.setPixelColor(currentLED + 1, strip.Color(((r / 3) * 2),((g / 3) * 2),((b / 3) * 2)));  //set the one in front to 5/6 of that color, etc.
      strip.setPixelColor(currentLED + 2, strip.Color((r / 2),(g / 2),(b / 2)));
      strip.setPixelColor(currentLED + 3, strip.Color((r / 5),(g / 5),(b / 5)));
      strip.setPixelColor(currentLED + 4, strip.Color((r / 10),(g / 10),(b / 10)));
      strip.setPixelColor(currentLED + 5, strip.Color((r / 25),(g / 25),(b / 25)));
      strip.setPixelColor(currentLED + 6, strip.Color(0,0,0));  //turns the one 6 ahead to off so it follows down the strip
      strip.show();
      if(currentLED <= 0){    //if we reach the end of the strip
        reverse = false;      //make the leds go from bottom to top again
      }
      else{
        currentLED = currentLED -1;   //otherwise make the led we're affecting one lower on the strip
      }
      lastUpdate = now;     //log the time of the last update to the strip
    }
    else{     //if we're in forward mode (nested ifs by the way, also eww)
     strip.setPixelColor(currentLED, strip.Color(r,g,b)); 
     strip.setPixelColor(currentLED - 1, strip.Color(((r / 3) * 2),((g / 3) * 2),((b / 3) * 2)));  //set the one behind to 5/6 of the color.
     strip.setPixelColor(currentLED - 2, strip.Color((r / 2),(g / 2),(b / 2)));  //the one behind that to 2/3
     strip.setPixelColor(currentLED - 3, strip.Color((r / 5),(g / 5),(b / 5))); //to 1/2
     strip.setPixelColor(currentLED - 4, strip.Color((r / 10),(g / 10),(b / 10))); //and so on
     strip.setPixelColor(currentLED - 5, strip.Color((r / 25),(g / 25),(b / 25)));
     strip.setPixelColor(currentLED - 6, strip.Color(0,0,0));   //set the LED 6 behind us to off to complete the cycle
     strip.show();
     if(currentLED >= strip.numPixels()){   //if we've reached the top of the strip
       reverse = true;  //run the cyce in reverse
     }
     else{
      currentLED = currentLED + 1;    //otherwise move to the next LED up
      lastUpdate = now;     //log the last cycle time 
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
