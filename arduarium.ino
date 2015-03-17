
/******
 * This code is all protoype for getting the LEDs and colour sequence correct
 * but is using DELAY which will not be compatible with the OLED or temp probe
 */


#include "LPD8806.h"
#include "SPI.h" // Comment out this line if using Trinket or Gemma
#ifdef __AVR_ATtiny85__
 #include <avr/power.h>
#endif

// Example to control LPD8806-based RGB LED Modules in a strip

/*****************************************************************************/

// Number of RGB LEDs in strand:
int nLEDs = 100;

// Chose 2 pins for output; can be any valid output pins:
int dataPin  = 4;
int clockPin = 3;


// First parameter is the number of LEDs in the strand.  The LED strips
// are 32 LEDs per meter but you can extend or cut the strip.  Next two
// parameters are SPI data and clock pins:
LPD8806 strip = LPD8806(nLEDs, dataPin, clockPin);

// You can optionally use hardware SPI for faster writes, just leave out
// the data and clock pin parameters.  But this does limit use to very
// specific pins on the Arduino.  For "classic" Arduinos (Uno, Duemilanove,
// etc.), data = pin 11, clock = pin 13.  For Arduino Mega, data = pin 51,
// clock = pin 52.  For 32u4 Breakout Board+ and Teensy, data = pin B2,
// clock = pin B1.  For Leonardo, this can ONLY be done on the ICSP pins.
//LPD8806 strip = LPD8806(nLEDs);




// Colour config, in percentage of RGB channels (0-100)
int dawn[3]    = { 15, 0, 20 };
int sunrise[3] = { 5, 0, 40 };
int day[3]     = { 100, 100, 100 };
int sunset[3]  = { 80, 80, 0 };
int dusk[3]    = { 51, 0, 35 };
int moon[3]    = { 0, 0, 8 };
int dark[3]    = { 0, 0, 0 };


int DEBUG = 1;      // DEBUG counter; if set to 1, will write values back via serial
int loopCount = 1; // How often should DEBUG report?



// Set up the LED outputs
void setup()
{
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
  clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
#endif

  // Start up the LED strip
  strip.begin();

  // Update the strip, to start they are all 'off'
  strip.show();

//  pinMode(redPin, OUTPUT);   // sets the pins as output
//  pinMode(grnPin, OUTPUT);   
//  pinMode(bluPin, OUTPUT); 

  if (DEBUG) {           // If we want to see values for debugging...
    Serial.begin(9600);  // ...set up the serial ouput 
  }
}

// Main program: list the order of crossfades
void loop() {
  Serial.println( "Dark > Dawn" );
  colorBlend( dark, dawn, 10, 0 ); // 10 min fade, 0 hold
  Serial.println( "Dawn > Sunrise" );
  colorBlend( dawn, sunrise, 50, 0 ); // 50 min fade, 0 hold
  Serial.println( "Sunrise > Day" );
  colorBlend( sunrise, day, 60, 720 ); // 720 // 1 hr fade, 12 hold
  Serial.println( "Day > Sunset" );
  colorBlend( day, sunset, 60, 0 ); // 1 hr fade, 0 hold
  Serial.println( "Sunset > Dusk" );
  colorBlend( sunset, dusk, 60, 0 ); // 1 hr fade, 0 hold
  Serial.println( "Dusk > Moon" );
  colorBlend( dusk, moon, 60, 0 ); // 1 hr fade, 0 hold
  Serial.println( "Moon > Dark" );
  colorBlend( moon, dark, 10, 410 ); // 410 // 10 min fade, 6h50m hold
  // total 24 hours
return;
//  colorBlend( dawn );
//  colorBlend( sunrise );
//  colorBlend( day );
//  colorBlend( sunset );
//  colorBlend( dusk );
//  colorBlend( night );
}


/* crossFade() converts the percentage colors to a 
*  0-255 range, then loops 1020 times, checking to see if  
*  the value needs to be updated each time, then writing
*  the color values to the correct pins.
*/

void colorBlend( int oldColor[3], int newColor[3], int fade, int hold ) {
  // 10 color changes, meaning wait (fadeMins*60sec)/10steps
  int fadeStep = 20;
  unsigned long waitMin = ( fade * 60 ) / fadeStep;
  unsigned long holdMin = hold * 60;

  // debug some stuff
  Serial.print( "fade: " );
  Serial.print( fade );
  Serial.print( "mins | fadeStep: " );
  Serial.print( fadeStep );
  Serial.print( "steps | waitMin: " );
  Serial.print( waitMin );
  Serial.print( "secs | hold: " );
  Serial.print( hold );
  Serial.print( "mins | holdMin: " );
  Serial.print( holdMin );
  Serial.println( "secs" );

  // Convert to 0-255
  int oldColorR = (oldColor[0] * 255) / 100;
  int oldColorG = (oldColor[1] * 255) / 100;
  int oldColorB = (oldColor[2] * 255) / 100;
  int newColorR = (newColor[0] * 255) / 100;
  int newColorG = (newColor[1] * 255) / 100;
  int newColorB = (newColor[2] * 255) / 100;
  int currentColorR = 0;
  int currentColorG = 0;
  int currentColorB = 0;

  for (int i = 0; i <= fadeStep; i++) {
    currentColorR = ( ((oldColorR * (fadeStep - i)) + (newColorR * i)) / (fadeStep*2) ) ;
    currentColorG = ( ((oldColorG * (fadeStep - i)) + (newColorG * i)) / (fadeStep*2) ) ;
    currentColorB = ( ((oldColorB * (fadeStep - i)) + (newColorB * i)) / (fadeStep*2) ) ;

    colorWipe(strip.Color(currentColorB, currentColorR, currentColorG), 0);


    if (DEBUG) { // If we want serial output, print it at the 
      if (i == 0 or i % loopCount == 0) { // beginning, and every loopCount times
        Serial.print("Loop/RGB: #");
        Serial.print(i);
        Serial.print(" | ");
        Serial.print(oldColorR);
        Serial.print(" / ");
        Serial.print(oldColorG);
        Serial.print(" / ");
        Serial.print(oldColorB);
        Serial.print(" | ");
        Serial.print(currentColorR*2);
        Serial.print(" / ");
        Serial.print(currentColorG*2);
        Serial.print(" / ");
        Serial.print(currentColorB*2);
        Serial.print(" | ");
        Serial.print(newColorR);
        Serial.print(" / ");
        Serial.print(newColorG);
        Serial.print(" / ");
        Serial.println(newColorB);
      } 
      DEBUG += 1;
    }

    for ( int y = 1; y <= waitMin; y++ ) {
//      delay( 1000 );
      delay( 10 );
    }

  }

int glimmer = false;
  if ( glimmer ) {
//    glimmer( newColor, hold );
  } else {
    for ( int z = 1; z <= holdMin; z++ ) {
      delay( 10 );
//      delay( 1000 );
    }
  }
}


// Fill the dots progressively along the strip.
void colorWipe(uint32_t c, uint8_t waiting) {
  int i;

  for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
//      delay(waiting);
  }
}
