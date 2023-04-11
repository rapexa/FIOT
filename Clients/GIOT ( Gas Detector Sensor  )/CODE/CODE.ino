#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

const char *ssid = "mmd";         // WiFi Name
const char *pass = "mgstudio884"; // WiFi Password

const int httpPort = 5550;
const char *host = "192.168.43.228";
const char *DeviceKey  = "100";

const int ledpin = 2;
const int sensorPin = 0;
int lpg;

void send_event();

WiFiClient client;

void setup() 
{
       Serial.begin(115200); 
       delay(10);
       
       pinMode(ledpin, OUTPUT);
       pinMode(sensorPin, INPUT);
       digitalWrite(ledpin, HIGH);
       
       Serial.println("[+] Connecting to ");
       Serial.println(ssid);
       
       WiFi.begin(ssid, pass);
        
       while (WiFi.status() != WL_CONNECTED) 
     {
            Serial.print(".");
            digitalWrite(ledpin, LOW);
            delay(300);
            digitalWrite(ledpin, HIGH);
            delay(300);
            digitalWrite(ledpin, LOW);
            delay(300);
            digitalWrite(ledpin, HIGH);
     }
      Serial.println("");
      Serial.println("[+] WiFi connected");
      Serial.print("[+] local ip: ");
      Serial.println(WiFi.localIP());
 }
void loop() 
{
     lpg = !digitalRead(sensorPin);
     
     Serial.print("[+] Air: ");
     Serial.println(lpg);
     
     if (lpg == 1)
     {
     
     digitalWrite(ledpin, HIGH);
     delay(2000);
     digitalWrite(ledpin, LOW);
     send_event();    
     
     }
     delay(1000);
}

void send_event(){
  
  Serial.print("[+] Connecting to ");
  Serial.println(host);

  WiFiClient client;
  
  if (!client.connect(host, httpPort)) {
    
    Serial.println("[+] Connection failed");
    digitalWrite(ledpin, LOW);
    delay(500);
    digitalWrite(ledpin, HIGH);
    delay(500);
    digitalWrite(ledpin, LOW);
    delay(500);
    digitalWrite(ledpin, HIGH);
    return;
    
  }
    
  String url = "/trigger/";
  url += "LPG_event";
  url += "/with/key/";
  url += DeviceKey;  
  
  Serial.print("[+] Requesting URL: ");
  Serial.println(url);  

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
               
  while(client.connected())
  {
    
    if(client.available())
    {
      
      digitalWrite(ledpin, LOW);
      delay(500);
      digitalWrite(ledpin, HIGH);
      delay(500);
      digitalWrite(ledpin, LOW);
      delay(500);
      digitalWrite(ledpin, HIGH);

    } else {
    
      delay(50);
    
    };  
  }

  String line = client.readStringUntil('}');
  line = line + "}";
  line = line.substring(145);
  line.trim();
  
  //Serial.println(line);
  
  StaticJsonDocument<200> jsonBuffer;
  //JsonObject root = jsonBuffer.parseObject(line);
  auto error = deserializeJson(jsonBuffer, line);
  
  if (error) {
    Serial.println("[+] Can not parse json data!");
    digitalWrite(ledpin, LOW);
    delay(300);
    digitalWrite(ledpin, HIGH);
    delay(300);
    digitalWrite(ledpin, LOW);
    delay(300);
    digitalWrite(ledpin, HIGH);
    delay(300);
    digitalWrite(ledpin, LOW);
    delay(300);
    digitalWrite(ledpin, HIGH);
  }
  String code = jsonBuffer["code"];
  String data = jsonBuffer["data"]; 
  Serial.print("[-] Code : ");
  Serial.println(code);
  Serial.print("[-] Date: ");
  Serial.println(data);
  Serial.println("[+] Connection closed!");
  Serial.println();
  client.stop();
}
