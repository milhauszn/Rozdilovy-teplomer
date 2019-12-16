
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "EncoderStepCounter.h"

#define ENCODER_PIN1 2
#define ENCODER_PIN2 3

#define ONE_WIRE_BUS 4
#define TEMPERATURE_PRECISION 11
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
EncoderStepCounter encoder(ENCODER_PIN1, ENCODER_PIN2);

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress tep1 = { 0x28, 0x70, 0x75, 0x79, 0xA2, 0x00, 0x03, 0x59 };
DeviceAddress tep2   = { 0x28, 0xD2, 0x3A, 0x79, 0xA2, 0x00, 0x03, 0xED };

float temp1;
float temp2;
float rozdil;
int  graf[41] = {};
int cas_rozdilu[41] = {};
unsigned long cas_akt;
unsigned long cas_predch;
unsigned long cas_predch_r;
signed char lastpos = 0;
signed char pos;
unsigned long cas_predch_o;
bool ref;

//unsigned long cas;

void setup() {
  sensors.begin();
  sensors.setResolution(tep1, TEMPERATURE_PRECISION);
  sensors.setResolution(tep2, TEMPERATURE_PRECISION);
  Serial.begin(9600);
  encoder.begin();
  encoder.setPosition(0);
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  display.display();
  delay(1000); 
  
  // Clear the buffer
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  cas_predch = 0;
  cas_predch_r = 0;
  cas_predch_o = 0;
  ref = false;
}

void interrupt() {
  encoder.tick();
}


void loop() {
  
  encoder.tick();
  pos = encoder.getPosition(); //osetreni enkoderu do 40 od 0
  if (pos != lastpos) {
    if (pos <= 0 ) {
      pos =0;
      encoder.setPosition(0);
      hlavniobr();
      ref = true;
    }
    if (pos > 40 ) {
      pos =40;
      encoder.setPosition(40);
    }
    if (pos >0){
      vedlobr();
      ref = true;
    }
    lastpos = pos;
 //   Serial.println(pos);
  }
  
  //sensors.requestTemperatures(); //cteni teplot
  //encoder.tick();
  //temp1 = sensors.getTempC(tep1);
  //encoder.tick();
  //temp2 = sensors.getTempC(tep2);
  //encoder.tick();
 // rozdil = temp1-temp2;

  cas_akt = millis();    //ziskani teplot teploty 
  if (cas_akt >= cas_predch_r + 2000) {
    cas_predch_r = cas_akt;
    zisktepl();
    if (!ref & pos ==0){
      hlavniobr();
    }
    if (!ref & pos>0){
      vedlobr();
    }
   }
    
     //zapis teploty do grafu jen kazdy cas za pluskem
  if (cas_akt >= cas_predch + 10000) {
    cas_predch = cas_akt;
    cas_rozdilu[40] = cas_akt/1000;
    graf[40] =20* rozdil;
    for (int i =0; i<=39; i=i+1){
      cas_rozdilu[i]=cas_rozdilu[i+1];
      graf[i]=graf[i+1];
      }
    }
 // encoder.tick();
 //if (cas_akt >= cas_predch_o  + 1000) {
//    cas_predch_o = cas_akt;
//    if (pos == 0){
//    hlavniobr();
//    }
//    else {
//    vedlobr();
//    }
//   }
   ref= false; 
}

void zisktepl() {
  sensors.requestTemperatures(); //cteni teplot
  temp1 = sensors.getTempC(tep1);
  temp2 = sensors.getTempC(tep2);
  rozdil = temp1-temp2;
}

void hlavniobr() {
  display.setTextSize(2);
  display.clearDisplay(); 
  for (int i = 0; i <=39; i= i+1){                              //kresleni grafu maleho a zobrazeni teplot
    display.drawPixel(127-i, 32-graf[i]/10, SSD1306_WHITE);
  }
  display.drawLine(86, 22, 86, 42, SSD1306_WHITE);
  display.drawLine(82, 32, 86, 32, SSD1306_WHITE);
  display.drawPixel(85, 30, SSD1306_WHITE);
  display.drawPixel(85, 28, SSD1306_WHITE);
  display.drawPixel(85, 26, SSD1306_WHITE);
  display.drawPixel(85, 24, SSD1306_WHITE);
  display.drawPixel(85, 22, SSD1306_WHITE);
  display.drawPixel(85, 34, SSD1306_WHITE);
  display.drawPixel(85, 36, SSD1306_WHITE);
  display.drawPixel(85, 38, SSD1306_WHITE);
  display.drawPixel(85, 40, SSD1306_WHITE);
  display.drawPixel(85, 42 , SSD1306_WHITE);
  
  display.setCursor(0, 0);
  display.print("T1:");
  display.print(temp1);
  display.print(" C");
  display.setCursor(0, 25);
  display.print("R:");
  display.print(round(rozdil*10.0)/10.0,1);
  display.setCursor(0, 50);
  display.print("T2:");
  display.print(temp2);
  display.print(" C");
  display.display();
}

void vedlobr() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("1:");
  display.print(temp1);
  display.print(" R");
  display.print(round(rozdil*10.0)/10.0,1);
  display.print(" 2:");
  display.print(temp2);
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);
  display.setCursor(0, 12);
  display.print("c: ");
  display.print(cas_rozdilu[41-pos]/60.0,1);
  display.print(" roz: ");
  display.print(graf[41-pos]/20.0);
  

  for (int i = 0; i <=39; i= i+1){                              //kresleni grafu velkeho
   // display.drawPixel(128-3*i, 44-graf[i+1]/5, SSD1306_WHITE);
   display.drawLine(127-3*i, 43-graf[i]/5 , 127-3*(i+1), 43-graf[i+1]/5, SSD1306_WHITE);
  }

  display.drawPixel(8+3*(pos-1), 21, SSD1306_WHITE); //kurzor
  display.drawLine(7+3*(pos-1), 20, 9+3*(pos-1), 20, SSD1306_WHITE);
 
  
  display.drawLine(7, 23, 7, 63, SSD1306_WHITE);
  display.drawLine(0, 43, 6, 43, SSD1306_WHITE);
  display.drawPixel(6, 39, SSD1306_WHITE);
  display.drawPixel(6, 35, SSD1306_WHITE);
  display.drawPixel(6, 31, SSD1306_WHITE);
  display.drawPixel(6, 27, SSD1306_WHITE);
  display.drawPixel(6, 23, SSD1306_WHITE);
  display.drawPixel(6, 47, SSD1306_WHITE);
  display.drawPixel(6, 51, SSD1306_WHITE);
  display.drawPixel(6, 55, SSD1306_WHITE);
  display.drawPixel(6, 59, SSD1306_WHITE);
  display.drawPixel(6, 63 , SSD1306_WHITE);
  display.display();
}
