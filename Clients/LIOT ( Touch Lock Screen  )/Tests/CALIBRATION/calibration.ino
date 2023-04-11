#include <Arduino.h>
#include <SPI.h>

#include <Adafruit_ILI9341esp.h>
#include <Adafruit_GFX.h>
#include <XPT2046.h>

// For the Esp connection of touch
#define TFT_DC 2
#define TFT_CS 15

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
XPT2046 touch(/*cs=*/ 4, /*irq=*/ 5);

static void calibratePoint(uint16_t x, uint16_t y, uint16_t &vi, uint16_t &vj) {
  // Draw cross
  
  
  tft.drawFastHLine(x - 8, y, 16,0xff);
  tft.drawFastVLine(x, y - 8, 16,0xff);
  while (!touch.isTouching()) {
    delay(10);
  }
  touch.getRaw(vi, vj);
  // Erase by overwriting with black
  tft.drawFastHLine(x - 8, y, 16, 0);
  tft.drawFastVLine(x, y - 8, 16, 0);
}

void calibrate() {
  uint16_t x1, y1, x2, y2;
  uint16_t vi1, vj1, vi2, vj2;
  touch.getCalibrationPoints(x1, y1, x2, y2);
  calibratePoint(x1, y1, vi1, vj1);
  delay(1000);
  calibratePoint(x2, y2, vi2, vj2);
  touch.setCalibration(vi1, vj1, vi2, vj2);
  
  
  tft.setTextColor(ILI9341_CYAN);
  tft.setTextSize(2);
  tft.println("Calibration Params");
  tft.println("");
  tft.setTextSize(3);
  tft.println(vi1);
  tft.println(vj1);
  tft.println(vi2);
  tft.println(vj2);
}

void setup() {
  delay(1000);  
  SPI.setFrequency(ESP_SPI_FREQ);
  tft.begin();
  touch.begin(tft.width(), tft.height());  // Must be done before setting rotation
  tft.fillScreen(ILI9341_BLACK);  
  calibrate();  // No rotation!!
}

void loop() {
  // Do nothing
  delay(1000);
}
