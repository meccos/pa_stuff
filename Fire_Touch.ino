#include <Adafruit_NeoPixel.h>
#include "Adafruit_FreeTouch.h"
#define PIN A1
#define CAPTOUCH_PIN A0
#define NUM_LEDS 180
// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRBW + NEO_KHZ800);

Adafruit_FreeTouch qt_1 = Adafruit_FreeTouch(CAPTOUCH_PIN, OVERSAMPLE_4, RESISTOR_50K, FREQ_MODE_NONE);

int InitFail=0;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("JP Test test");
  
  
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  InitFail = qt_1.begin();
  
    if (InitFail != 0)
	{		 
      Serial.println("Failed to begin qt on pin A0");
	}

}

bool checkpress();

void loop() 
{
  static int iCase=0;
  
  if(checkpress() == true)
  {
    iCase ++;
  }
  if( iCase == 5)
    iCase = 0;
  
  switch (iCase)
  {
    case 0:
      Fire(50,120,10);
      break;
    case 1:
      SnowSparkle(0x10, 0x10, 0x10, 20, 200);
      break;
	case 2:
		for( int j = 0; j < NUM_LEDS; j++) 
		{
		  setPixelHeatColor(j, 0 );
		}
		showStrip();
	break;
	case 3:
		for( int j = 0; j < NUM_LEDS; j++) 
		{
		  setPixelHeatColor(j, 100 );
		}
		showStrip();
	break;
	case 4:
		for( int j = 0; j < NUM_LEDS; j++) 
		{
		  setPixelHeatColor(j, 255 );
		}
		showStrip();
      break;
    default:
    break;
  }
  delay(20);
}

bool checkpress() {

    int iReturn=false;
	
	static int iLow=1000;
	static int iHigh=0;

// Get current button state.
  
    static int iDebounce=0;
    int iResult =  qt_1.measure(); 
	iResult = qt_1.measure(); 
    Serial.print("QT 1: "); Serial.print(iResult);
	
	if(iLow > iResult)
	{
		iLow = iResult;
	}
	if(iHigh < iResult)
	{
		iHigh = iResult;
	}
 
	if (iResult > ((iHigh + iLow)/2 )) 
	{  
	 iDebounce ++;
	}
	else 
	{
	  iDebounce = 0;
	  iReturn=false;
	}

	if( iDebounce == 10)
	{
	   iReturn=true;
	}
		
  return iReturn;
} 

void Fire(int Cooling, int Sparking, int SpeedDelay) 
  { 
  static byte heat[NUM_LEDS];
  int cooldown;

  
  // Step 1.  Cool down every cell a little
  for( int i = 0; i < NUM_LEDS; i++) {
    cooldown = random(0, ((Cooling * 10) / NUM_LEDS) + 2);
    
    if(cooldown>heat[i]) {
      heat[i]=0;
    } else {
      heat[i]=heat[i]-cooldown;
    }
  }
  
  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }
    
  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160,255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for( int j = 0; j < NUM_LEDS; j++) {
    setPixelHeatColor(j, heat[j] );
  }

  showStrip();
}

void setPixelHeatColor (int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature/255.0)*191);
 
  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252
 
  // figure out which third of the spectrum we're in:
  if( t192 > 0x80) {                     // hottest
    setPixel(Pixel, 255, 255, heatramp);
  } else if( t192 > 0x40 ) {             // middle
    setPixel(Pixel, 255, heatramp, 0);
  } else {                               // coolest
    setPixel(Pixel, heatramp, 0, 0);
  }
}

void showStrip() {
 #ifdef ADAFRUIT_NEOPIXEL_H 
   // NeoPixel
   strip.show();
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   FastLED.show();
 #endif
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
 #ifdef ADAFRUIT_NEOPIXEL_H 
   // NeoPixel
   strip.setPixelColor(Pixel, strip.Color(red, green, blue));
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H 
   // FastLED
   leds[Pixel].r = red;
   leds[Pixel].g = green;
   leds[Pixel].b = blue;
 #endif
}

void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue); 
  }
  showStrip();
}

void SnowSparkle(byte red, byte green, byte blue, int SparkleDelay, int SpeedDelay) {
  setAll(red,green,blue);
  
  int Pixel = random(NUM_LEDS);
  setPixel(Pixel,0xff,0xff,0xff);
  showStrip();
  delay(SparkleDelay);
  setPixel(Pixel,red,green,blue);
  showStrip();
}
