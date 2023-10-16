#include <Arduino.h>
#include <Bounce2.h>
#include <elapsedMillis.h>
#include <Adafruit_NeoPixel.h>

#define MAXIMUM_NUM_NEOPIXELS 5

#define LED_PIN 6

Adafruit_NeoPixel strip(MAXIMUM_NUM_NEOPIXELS, LED_PIN, NEO_RGB + NEO_KHZ800);

#define SGO 2 //button 1
#define SUP 3 //button 4
#define SDOWN 4 //button 3

Bounce Sgo = Bounce();
Bounce Sup = Bounce();
Bounce Sdown = Bounce();

elapsedMillis timer;
elapsedMillis timer_blink;
elapsedMillis timer2;
elapsedMillis timer3;


#define blink_interval 1000
int ledState = HIGH;
int ledState2 = HIGH;


typedef enum{
  Entry_Configuration,
  Inicio,
  Led_count,
  Pause,
  Configuration,
  Exit_Configuration
} state_t;

typedef enum{
  Wait,
  Timer_time,
  Counting_effect,
  Countin_Color
} state_confi;

typedef enum{
  time_1,
  time_2,
  time_5,
  time_10
}state_mode1;

typedef enum{
  Switch_off,
  Blink_half,
  Fade_out
}state_mode2;

typedef enum{
  Violet,
  Blue,
  Cyan,
  Green,
  Yellow,
  Orange,
  White
}state_mode3;

//Estado atual da maquina
state_t currentState = Inicio;
state_confi currentState_C = Wait;
state_mode1 currentState_C1 = time_1;
state_mode2 currentState_C2 = Switch_off;
state_mode3 currentState_C3 = Violet;

unsigned long previousMillis = 0;

unsigned long t = 10;
int r = 255, g = 255, b = 255;

void updateTimer( unsigned long interval){
  if (millis() - previousMillis >= interval) {
    previousMillis = millis();
    if(t > 0){
      t -= 2;
    }
  }
}

void updateLed(int r, int g, int b){
  for (int i = 0; i < MAXIMUM_NUM_NEOPIXELS; i++) {
    if(i < t/2)
      strip.setPixelColor(i, strip.Color(r, g, b));
    else
      strip.setPixelColor(i, 0);
  }
  if(t > 0)
  strip.show();
}

void Time_up(){
  if (timer_blink > 100) {
    timer_blink = 0;

    if (ledState) 
      strip.fill(strip.Color(255, 0, 0), 0, 5);
    else 
      strip.clear();

    strip.show();
    ledState = !ledState;
  }

}

void LedBlink(int pin, unsigned long interval){
  if (timer_blink > interval) {
    timer_blink = 0;

    if (ledState) 
      strip.setPixelColor(pin, strip.Color(20, 20, 20));
    else 
      strip.setPixelColor(pin, 0);

    strip.show();
    ledState = !ledState;
  }
}

void LedTime(int pin, unsigned long interval, int r, int g, int b){
  if(timer > interval){
    strip.setPixelColor(pin, 0);
    strip.show();
    timer = 0;
  }
  else if(timer > 500){
    strip.setPixelColor(pin, strip.Color(r, g, b));
    strip.show();
  }
}

void HalfBlink(int pin, unsigned long interval, int r, int g, int b){
  if(timer2 < interval/2){
    //Serial.println("fmdkfm");
    strip.setPixelColor(4, strip.Color(r, g, b));
    strip.show();
  }
  if(timer2 > interval/2 && timer2 <= interval){
    if (timer3 > 250) {
      timer3 = 0;
      //Serial.println("ioijiofh");
      if (ledState2) 
        strip.setPixelColor(pin, strip.Color(r, g, b));
      else 
        strip.setPixelColor(pin, 0);

      strip.show();
      ledState2 = !ledState2;
    }
  }
  if(timer2 > interval){
    //Serial.println("aqui");
    strip.setPixelColor(pin, 0);
    strip.show();
    //if(timer2 > interval+500)
      timer2 = 0;
  }
}

void fade(unsigned long duration, unsigned long startTime, int r, int g, int b){  
  // Calculate the elapsed time since the start
  unsigned long elapsedTime = millis() - startTime;
 
  // Calculate the new brightness based on the elapsed time
  int currentBrightness = map(elapsedTime, 0, duration, 255, 0);
  
  // Set the LED color with the current brightness
  strip.setPixelColor(4, strip.Color(r * currentBrightness, g * currentBrightness, b * currentBrightness));
  strip.show();
  
  // Check if the fade is complete
  if (elapsedTime >= duration) {
    // Optional: Turn off the LED or perform any other action
    strip.setPixelColor(4, strip.Color(0, 0, 0)); // Turn off the LED
    strip.show();
  }
}

void setup(){

  Sgo.attach(SGO, INPUT_PULLUP);
  Sup.attach(SUP, INPUT_PULLUP);
  Sdown.attach(SDOWN, INPUT_PULLUP);

  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);

  strip.begin();
  strip.show(); 
}

bool blink = false;

void INICIO()
{
  switch (currentState){
    case Inicio:
      Serial.println("Inicio");

      if(blink ==true)
        Time_up();

      if(blink == false){
        strip.clear();
        strip.show();
      }


      if(Sgo.rose() && blink == false){                ////////////////mexi aqui
        t=10 +2;
        currentState = Led_count;
      }

      if(Sgo.rose() && blink == true){
        blink = false;
        currentState = Inicio;
      }

      if(Sup.read() == LOW && Sup.currentDuration() >= 3000){
        currentState = Entry_Configuration;
        strip.clear();
        strip.show();
      }

    break;

    case Led_count:
      //Serial.println("Led_count");
      if(Sgo.rose()){                  //////////////////////////////////////////alterei
        //previousMillis = millis();
        blink = false;
        currentState = Inicio;
      }                              //////////////////////////////////////////alterei
      updateTimer(2000);
      updateLed(r, g, b);

      Serial.println(t);

      if ( t <= 8 && Sup.rose()){
        t += 2;
        //previousMillis = millis();
        Serial.println(t);
      }
      
      if(Sdown.rose()){
        currentState = Pause;///////////////////////////////////////////////////amimir
      }

      if(Sup.read() == LOW && Sup.currentDuration() >= 3000){
        strip.clear();
        strip.show();
        currentState = Entry_Configuration;
      }


      if(t <= 0){
        Serial.println("Fim");
        strip.clear();
        strip.show();
        blink = true;
        currentState = Inicio;
      }
    break;

    case Pause:
      Serial.println("Pause");
      updateLed(r, g, b); 

      if(Sup.rose() && t <= 8)
        t += 2;

      if(Sup.read() == LOW && Sup.currentDuration() >= 3000){
        strip.clear();
        strip.show();
        currentState = Entry_Configuration;
      }

      if(Sdown.rose()){
        currentState = Led_count;
      }

      if(Sgo.rose()){      //////////////////////////////////////////adicionei
        t=10 +2;
        strip.clear();
        strip.show();
        blink = false;
        currentState = Inicio;
      }                                   /////////////////////////////adicionei
    break;

    case Entry_Configuration:
      Serial.println("Entry_Configuration");
      LedBlink(0, 500);
      if(Sup.rose()){
        currentState = Configuration;
      }
    break;

    case Configuration:
      Serial.println("Configuration");
      if(Sup.read() == LOW && Sup.currentDuration() >= 3000){
        currentState = Exit_Configuration;
      }
    break;

    case Exit_Configuration:
        strip.clear();
        strip.show();
        if(Sup.rose()){
          currentState = Inicio;
        }
    break;
  }
  
}

void CONFI_MODE()
{
    switch (currentState_C){
      case Wait:
        if(currentState == Configuration)
          currentState_C = Timer_time;
      break;

      case Timer_time:
        //Serial.println("Timer_time");
        LedBlink(0, 500); //Led 1 blink

        if(Sup.rose()){
          strip.setPixelColor(0, 0);
          currentState_C = Counting_effect;
        }
        
        if(currentState != Configuration)
          currentState_C = Wait;
      break;

      case Counting_effect:
        //Serial.println("Counting_effect");
        LedBlink(1, 500); //LED 2 blink

        if(Sup.rose()){
          strip.setPixelColor(1, 0);
          currentState_C = Countin_Color;
        }

        if(currentState != Configuration)
          currentState_C = Wait;

      break;

      case Countin_Color:
        //Serial.println("Countin_Color");
        LedBlink(2, 500); //Led 3 blink

        if(Sup.rose()){
          strip.setPixelColor(2, 0);
          currentState_C = Timer_time;
        }

        if(currentState != Configuration)
          currentState_C = Wait;

      break;
    }
}

void MODE1()
{
  if(currentState_C == Timer_time){
    switch (currentState_C1){
      case time_1:
        Serial.println("time_1");
        t = 5*1;
        LedTime(4, 1000, r, g, b);

        if(Sdown.rose()){
          strip.setPixelColor(4, 0);
          currentState_C1 = time_2;
        }
      break;

      case time_2:
        Serial.println("time_2");
        t=5*2;
        LedTime(4, 2000, r, g, b);

        if(Sdown.rose()){
          strip.setPixelColor(4, 0);
          currentState_C1 = time_5;
        }
      break;

      case time_5:
        Serial.println("time_5");
        t=5*5;
        LedTime(4, 5000, r, g, b);

        if(Sdown.rose()){
          strip.setPixelColor(4, 0);
          currentState_C1 = time_10;
        }
      break;

      case time_10:
        Serial.println("time_10");
        t=5*10;
        LedTime(4, 10000, r, g, b);

        if(Sdown.rose()){
          strip.setPixelColor(4, 0);
          currentState_C1 = time_1;
        }
      break;
    }
  }
}

void MODE2()
{
  if(currentState_C == Counting_effect){
    switch (currentState_C2){

      case Switch_off:
        LedTime(4, 2000, r, g, b);
        Serial.println("Switch_off");
        if(Sdown.rose()){
          strip.setPixelColor(4, 0);
          currentState_C2 = Blink_half;
        }
      break;

      case Blink_half:
        HalfBlink(4, 4000, r, g, b);
        Serial.println("Blink_half");
        if(Sdown.rose()){
          strip.setPixelColor(4, 0);  
          currentState_C2 = Fade_out;
        }
      break;

      case Fade_out:
      Serial.println("Fade_out");
        unsigned long startTime = millis();
        while(currentState_C2 == Fade_out){
          fade(5000, startTime, 1, 0, 0);
        }
        if(Sdown.rose()){
          strip.setPixelColor(4, 0);
          currentState_C2 = Switch_off;
        }
      break;
    } 
  }
}

void MODE3()
{
  if(currentState_C == Countin_Color){
    switch (currentState_C3){
      case Violet:
        Serial.println("Violet");
        strip.setPixelColor(4, strip.Color(179, 0, 255));
        r=179;
        g=0;
        b=255;
        strip.show();

        if(Sdown.rose()){
          strip.setPixelColor(4, 0);
          currentState_C3 = Blue;
        }
      break;

      case Blue:
        Serial.println("Blue");
        strip.setPixelColor(4, strip.Color(0, 0, 255));
        r=0;
        g=0;
        b=255;
        strip.show();

        if(Sdown.rose()){
          strip.setPixelColor(4, 0);
          currentState_C3 = Cyan;
        }
      break;

      case Cyan:
        Serial.println("Cyan");
        strip.setPixelColor(4, strip.Color(0, 255, 255));
        r=0;
        g=255;
        b=255;
        strip.show();

        if(Sdown.rose()){
          strip.setPixelColor(4, 0);
          currentState_C3 = Green;
        }
      break;

      case Green:
        Serial.println("Green");
        strip.setPixelColor(4, strip.Color( 0, 255, 0));
        r=0;
        g=255;
        b=0;
        strip.show();

        if(Sdown.rose()){
          strip.setPixelColor(4, 0);
          currentState_C3 = Yellow;
        }
      break;

      case Yellow:
        Serial.println("Yellow");
        strip.setPixelColor(4, strip.Color(255, 165, 0));
        r=255;
        g=165;
        b=0;
        strip.show();

        if(Sdown.rose()){
          strip.setPixelColor(4, 0);
          currentState_C3 = Orange;
        }
      break;

      case Orange:
        Serial.println("Orange");
        strip.setPixelColor(4, strip.Color(255, 50, 0));
        r=255;
        g=50;
        b=0;
        strip.show();

        if(Sdown.rose()){
          strip.setPixelColor(4, 0);
          currentState_C3 = White;
        }
      break;

      case White:
        Serial.println("White");
        strip.setPixelColor(4, strip.Color( 255, 255, 255));
        r=255;
        g=255;
        b=255;
        strip.show();

        if(Sdown.rose()){
          strip.setPixelColor(4, 0);
          currentState_C3 = Violet;
        }
      break;
    }
  }
}

void loop() 
{
  Sgo.update();
  Sup.update();
  Sdown.update();

 
  INICIO();

  //Configuration mode
  CONFI_MODE();
  MODE1();
  MODE2();
  MODE3();  
}