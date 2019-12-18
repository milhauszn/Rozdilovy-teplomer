
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define ONE_WIRE_BUS 4
#define TEMPERATURE_PRECISION 11
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress tep1 = { 0x28, 0x70, 0x75, 0x79, 0xA2, 0x00, 0x03, 0x59 };
DeviceAddress tep2   = { 0x28, 0xD2, 0x3A, 0x79, 0xA2, 0x00, 0x03, 0xED };

float temp1;
float temp2;
float rozdil;
int graf[41] = {};
int cas_rozdilu[41] = {};
unsigned long cas_akt;
unsigned long cas_predch;

//unsigned long cas;

void setup() {
  sensors.begin();
  sensors.setResolution(tep1, TEMPERATURE_PRECISION);
  sensors.setResolution(tep2, TEMPERATURE_PRECISION);
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  display.display();
  delay(1000); 
  
  // Clear the buffer
  display.clearDisplay();
   // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);

  cas_predch = 0;
 
}

void loop() {
  display.clearDisplay();
  sensors.requestTemperatures();
  temp1 = sensors.getTempC(tep1);
  temp2 = sensors.getTempC(tep2);
  rozdil = temp1-temp2;
  cas_akt = millis();
  if (cas_akt >= cas_predch + 2000) {
    cas_predch = cas_akt;
    cas_rozdilu[40] = cas_akt/1000;
    graf[40] = 2* rozdil;
    for (int i =0; i<=39; i=i+1){
      cas_rozdilu[i]=cas_rozdilu[i+1];
      graf[i]=graf[i+1];
      
    }
  
  }
  for (int i = 0; i <=39; i= i+1){
    display.drawPixel(128-i, 32-graf[i+1], SSD1306_WHITE);
  }
  display.drawLine(87, 22, 87, 42, SSD1306_WHITE);
  display.drawLine(82, 32, 86, 32, SSD1306_WHITE);
  display.drawPixel(86, 30, SSD1306_WHITE);
  display.drawPixel(86, 28, SSD1306_WHITE);
  display.drawPixel(86, 26, SSD1306_WHITE);
  display.drawPixel(86, 24, SSD1306_WHITE);
  display.drawPixel(86, 22, SSD1306_WHITE);

  display.drawPixel(86, 34, SSD1306_WHITE);
  display.drawPixel(86, 36, SSD1306_WHITE);
  display.drawPixel(86, 38, SSD1306_WHITE);
  display.drawPixel(86, 40, SSD1306_WHITE);
  display.drawPixel(86, 42 , SSD1306_WHITE);

  display.setTextSize(2);
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
  display.display();      // Show initial text
  //delay(200);

}
