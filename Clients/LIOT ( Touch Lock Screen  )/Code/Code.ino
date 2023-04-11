#include <Arduino.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include "Adafruit_ILI9341esp.h"
#include "Adafruit_GFX.h"
#include "XPT2046.h"

const char* ssid = "mmd";
const char* password = "12345678";
String url = "http://rapexa.ir:5511";

#define TFT_DC 2
#define TFT_CS 15
#define BUZZER D0

/******************* UI details */
#define BUTTON_X 40
#define BUTTON_Y 100
#define BUTTON_W 60
#define BUTTON_H 30
#define BUTTON_SPACING_X 20
#define BUTTON_SPACING_Y 20
#define BUTTON_TEXTSIZE 2

// text box where numbers go
#define TEXT_X 10
#define TEXT_Y 10
#define TEXT_W 220
#define TEXT_H 50
#define TEXT_TSIZE 3
#define TEXT_TCOLOR ILI9341_MAGENTA

// the data (phone #) we store in the textfield
#define TEXT_LEN 12
char textfield[TEXT_LEN+1] = "";
uint8_t textfield_i=0;

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
XPT2046 touch(/*cs=*/ 4, /*irq=*/ 5);

/* create 15 buttons, in classic candybar phone style */
char buttonlabels[15][5] = {"Buy", "", "Clr", "1", "2", "3", "4", "5", "6", "7", "8", "9", ".", "0" , ""};
uint16_t buttoncolors[15] = {ILI9341_DARKGREEN, ILI9341_BLACK, ILI9341_RED, 
                             ILI9341_BLUE, ILI9341_BLUE, ILI9341_BLUE, 
                             ILI9341_BLUE, ILI9341_BLUE, ILI9341_BLUE, 
                             ILI9341_BLUE, ILI9341_BLUE, ILI9341_BLUE, 
                             ILI9341_ORANGE, ILI9341_BLUE, ILI9341_BLACK };
Adafruit_GFX_Button buttons[15];

void setup() {
  // put your setup code here, to run once:
  pinMode(BUZZER, OUTPUT);
  Serial.begin(115200);
  SPI.setFrequency(ESP_SPI_FREQ);

  WiFi.begin(ssid, password);
  Serial.print("[+] Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("[+] Connected to : " + String(ssid));
  Serial.println();
  Serial.print("[+] IP address : ");
  Serial.println(WiFi.localIP());

  tft.begin();
  touch.begin(tft.width(), tft.height());  // Must be done before setting rotation
  tft.fillScreen(ILI9341_BLACK);
  
  // Replace these for your screen module
  touch.setCalibration(1832, 262, 264, 1782);

  // create buttons
  for (uint8_t row=0; row<5; row++) {
    for (uint8_t col=0; col<3; col++) {
      buttons[col + row*3].initButton(&tft, BUTTON_X+col*(BUTTON_W+BUTTON_SPACING_X), 
                 BUTTON_Y+row*(BUTTON_H+BUTTON_SPACING_Y),    // x, y, w, h, outline, fill, text
                  BUTTON_W, BUTTON_H, ILI9341_BLACK, buttoncolors[col+row*3], ILI9341_WHITE,
                  buttonlabels[col + row*3], BUTTON_TEXTSIZE); 
      buttons[col + row*3].drawButton();
    }
  }

  // create 'text field'
  tft.drawRect(TEXT_X, TEXT_Y, TEXT_W, TEXT_H, ILI9341_WHITE);

}

void sound_error(){
  digitalWrite(BUZZER,HIGH);
  delay(2000);
  digitalWrite(BUZZER,LOW);
}

void sound_acpet() {
  digitalWrite(BUZZER,HIGH);
  delay(500);
  digitalWrite(BUZZER,LOW); 
}

void loop() {
  // put your main code here, to run repeatedly:
  uint16_t x, y;
  if (touch.isTouching()) 
    touch.getPosition(x, y);

  // go thru all the buttons, checking if they were pressed
  for (uint8_t b=0; b<15; b++) {
    if (buttons[b].contains(x, y)) {
      buttons[b].press(true);  // tell the button it is pressed
    } else {
      buttons[b].press(false);  // tell the button it is NOT pressed
    }
  }

    // now we can ask the buttons if their state has changed
  for (uint8_t b=0; b<15; b++) {
    if (buttons[b].justReleased()) {
      buttons[b].drawButton();  // draw normal
    }
    
    if (buttons[b].justPressed()) {
        buttons[b].drawButton(true);  // draw invert!
        
        // if a numberpad button, append the relevant # to the textfield
        if (b >= 3) {
          if (textfield_i < TEXT_LEN) {
            textfield[textfield_i] = buttonlabels[b][0];
            textfield_i++;
      textfield[textfield_i] = 0; // zero terminate
            
            //fona.playDTMF(buttonlabels[b][0]);
          }
        }

        // clr button! delete char
        if (b == 2) {
          
          textfield[textfield_i] = 0;
          if (textfield > 0) {
            textfield_i--;
            textfield[textfield_i] = ' ';
          }
        }

        if (b == 0) {
          if (WiFi.status() == WL_CONNECTED) {
            WiFiClient client;
            HTTPClient http;
            String fullUrl = url;
            Serial.println("[+] Requesting : " + fullUrl);
            if (http.begin(client, fullUrl)) {
              // Specify content-type header
              http.addHeader("Content-Type", "application/x-www-form-urlencoded");
              // Data to send with HTTP POST
              String httpRequestData = "token=123456&data="+String(textfield);
              int httpCode = http.POST(httpRequestData);
              Serial.println("[+] Response code : " + String(httpCode));
              if (httpCode > 0) {
                Serial.println("[+] Response : " + http.getString());
              }
              http.end();
              sound_acpet();
            } else {
              Serial.printf("[-] Unable to connect! \n");
              sound_error();
            }
          }
        }

        
        // update the current text field
        tft.setCursor(TEXT_X + 2, TEXT_Y+10);
        tft.setTextColor(TEXT_TCOLOR, ILI9341_BLACK);
        tft.setTextSize(TEXT_TSIZE);
        tft.print(textfield);
        
      delay(100); // UI debouncing
    }
  }
}
