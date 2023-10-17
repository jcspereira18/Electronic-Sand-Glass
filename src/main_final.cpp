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
elapsedMillis timer4;
elapsedMillis timer5;


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

unsigned long t_interval = 2000;
unsigned long fast_blink = 125;
unsigned long t_count = 10;
unsigned long t_max = 10;
int r = 100, g = 100, b = 100;
bool blink = false;
int effect_count = 0;  //flag para saber qual o efeito a ser executado

void updateTimer( unsigned long interval){
  if (millis() - previousMillis >= interval) {
    previousMillis = millis();
    if(t_count > 0){
      t_count -= interval/1000;
    }
  }
}

void updateLed(int r, int g, int b){
  for (unsigned long i = 0; i < MAXIMUM_NUM_NEOPIXELS; i++) {
    if(i < t_count/(t_interval/1000))
      strip.setPixelColor(i, strip.Color(r, g, b));
    else
      strip.setPixelColor(i, 0);
  }
  if(t_count > 0)
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
  if(timer >= interval + 500){
    strip.setPixelColor(pin, 0);
    strip.show();
    timer = 0;
  }
  else if(timer > 500){
    strip.setPixelColor(pin, strip.Color(r, g, b));
    strip.show();
  }
}

void HalfBlink(int pin, int r, int g, int b, unsigned long t_interval, int fast_blink){

  if(timer2 < t_interval/2){
    strip.setPixelColor(pin, strip.Color(r, g, b));
    strip.show();
  }
  if(timer2 > t_interval/2 && timer2 <= t_interval){
    if (timer3 > fast_blink) {
    timer3 = 0;
    if (ledState2) 
      strip.setPixelColor(pin, strip.Color(r, g, b));
    else 
      strip.setPixelColor(pin, 0);
    strip.show();
    ledState2 = !ledState2;
    }
  }
  if(timer2 > t_interval){
      timer2 = 0;
  }  
}

void fade(int pin, unsigned long duration, int r, int g, int b){  
 
  // Calculate the new brightness based on the elapsed time
  int currentBrightness = map(timer4, 0, duration, 255, 0);
  
  // Set the LED color with the current brightness
  strip.setPixelColor(pin, strip.Color(r*currentBrightness/255, g*currentBrightness/255, b*currentBrightness/255));
  strip.show();
  
  // Check if the fade is complete
  if (timer4 > duration) {
    strip.setPixelColor(pin, 0); // Turn off the LED
    strip.show();
    timer4 = 0; //restart fade
  }
}

void LedColor(int pin, int red, int green, int blue) {
    strip.setPixelColor(pin, strip.Color( red, green, blue));
    r = red;
    g = green;
    b = blue;
    strip.show();
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

void INICIO()
{
  switch (currentState){
    case Inicio:
      Serial.println("Inicio");

      if(blink == true)
        Time_up();

      if(blink == false){
        strip.clear();
        strip.show();
      }


      if(Sgo.rose() && blink == false){
        t_count=MAXIMUM_NUM_NEOPIXELS*t_interval/1000;
        t_max=t_count;
        previousMillis = millis();
        ledState2 = HIGH;
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
      if(Sgo.rose()){
        t_count=MAXIMUM_NUM_NEOPIXELS*t_interval/1000;
        blink = false;
        currentState = Inicio;
      }
      
      if(effect_count == 0){
        updateTimer(t_interval);
        updateLed(r, g, b);
      }
      
      if(effect_count == 1){
        updateTimer(t_interval);
        for(unsigned long i = 0; i < MAXIMUM_NUM_NEOPIXELS; i++) {
          if(i < t_count/(t_interval/1000)-1){
            strip.setPixelColor(i, strip.Color(r, g, b));
            strip.show();
          }    
          else{
            if(i > t_count/(t_interval/1000)-1){
              strip.setPixelColor(i, 0);
              strip.show();
            }
            else
              HalfBlink(i, r, g, b, t_interval, fast_blink);
          }
        }
      }

      if(effect_count == 2){
        updateTimer(t_interval);
        for(unsigned long i = 0; i < MAXIMUM_NUM_NEOPIXELS; i++) {
          if(i < t_count/(t_interval/1000)-1){
            strip.setPixelColor(i, strip.Color(r, g, b));
            strip.show();
          }    
          else{
            if(i > t_count/(t_interval/1000)-1){
              strip.setPixelColor(i, 0);
              strip.show();
            }
            else
              fade(i, t_interval, r, g, b);
          }
        }
      }

      Serial.println(t_count);

      if ( (t_count <= t_max - t_interval/1000) && Sup.rose()){
        t_count += t_interval/1000;
        Serial.println(t_count);
      }
      
      if(Sdown.rose()){
        currentState = Pause;///////////////////////////////////////////////////amimir
      }

      if(Sup.read() == LOW && Sup.currentDuration() >= 3000){
        strip.clear();
        strip.show();
        currentState = Entry_Configuration;
      }


      if(t_count <= 0){
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

      if(Sup.rose() && (t_count <= t_max - t_interval/1000))
        t_count += t_interval/1000;

      if(Sup.read() == LOW && Sup.currentDuration() >= 3000){
        strip.clear();
        strip.show();
        currentState = Entry_Configuration;
      }

      if(Sdown.rose()){
        currentState = Led_count;
      }

      if(Sgo.rose()){      //////////////////////////////////////////adicionei
        t_count=MAXIMUM_NUM_NEOPIXELS*t_interval/1000;
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
        blink = false;
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
        t_interval = 1000;
        fast_blink = 63;
        LedTime(4, t_interval, r, g, b);

        if(Sdown.rose()){
          strip.setPixelColor(4, 0);
          currentState_C1 = time_2;
        }
      break;

      case time_2:
        Serial.println("time_2");
        t_interval=2000;
        fast_blink = 125;
        LedTime(4, t_interval, r, g, b);

        if(Sdown.rose()){
          strip.setPixelColor(4, 0);
          currentState_C1 = time_5;
        }
      break;

      case time_5:
        Serial.println("time_5");
        t_interval=5000;
        fast_blink = 313;
        LedTime(4, t_interval, r, g, b);

        if(Sdown.rose()){
          strip.setPixelColor(4, 0);
          currentState_C1 = time_10;
        }
      break;

      case time_10:
        Serial.println("time_10");
        t_interval=10000;
        fast_blink = 625;
        LedTime(4, t_interval, r, g, b);

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
        effect_count = 0;
        Serial.println("Switch_off");
        if(Sdown.rose()){
          strip.setPixelColor(4, 0);
          currentState_C2 = Blink_half;
        }
      break;

      case Blink_half:
        HalfBlink(4, r, g, b, 2000, 125);
        effect_count = 1;
        Serial.println("Blink_half");
        if(Sdown.rose()){
          strip.setPixelColor(4, 0);  
          currentState_C2 = Fade_out;
        }
      break;

      case Fade_out:
        Serial.println("Fade_out");
        effect_count = 2;
        fade(4, 2000, r, g, b);

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
        LedColor(4, 179, 0, 255);

        if(Sdown.rose()){
          strip.setPixelColor(4, 0);
          currentState_C3 = Blue;
        }
      break;

      case Blue:
        Serial.println("Blue");
        LedColor(4, 0, 0, 255);

        if(Sdown.rose()){
          strip.setPixelColor(4, 0);
          currentState_C3 = Cyan;
        }
      break;

      case Cyan:
        Serial.println("Cyan");
        LedColor(4, 0, 255, 255);

        if(Sdown.rose()){
          strip.setPixelColor(4, 0);
          currentState_C3 = Green;
        }
      break;

      case Green:
        Serial.println("Green");
        LedColor(4, 0, 255, 0);

        if(Sdown.rose()){
          strip.setPixelColor(4, 0);
          currentState_C3 = Yellow;
        }
      break;

      case Yellow:
        Serial.println("Yellow");
        LedColor(4, 255, 165, 0);

        if(Sdown.rose()){
          strip.setPixelColor(4, 0);
          currentState_C3 = Orange;
        }
      break;

      case Orange:
        Serial.println("Orange");
        LedColor(4, 255, 50, 0);

        if(Sdown.rose()){
          strip.setPixelColor(4, 0);
          currentState_C3 = White;
        }
      break;

      case White:
        Serial.println("White");
        LedColor(4, 255, 255, 255);

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