#include <SoftwareSerial.h> 
#include <TinyGPS++.h>   
#include <Wire.h> 
#include <Adafruit_INA219.h> 
 
Adafruit_INA219 ina219; 
String  data, res, latitude, longitude; 
SoftwareSerial sim800l(12, 11); // RX, TX pins for SIM800L 

#define DEBUG true 
#define rxGPS 9 //Port TX pada Module GPS 
#define txGPS 8 //Port RX pada Module GPS 

SoftwareSerial neogps(rxGPS, txGPS); 
TinyGPSPlus gps; 

void gps2() 
{ 
  // Can take up to 60 seconds 
  boolean newData = false; 
  for (unsigned long start = millis(); millis() - start < 2000;) 
  { 
    while (neogps.available()) 
    { 
      if (gps.encode(neogps.read())) 
      { 
        newData = true; 
        break; 
      } 
    } 
  } 
  if (newData) 
  { 
    newData = false; 
    String latitude = String(gps.location.lat(), 6); 
    String longitude = String(gps.location.lng(), 6); 
 
  String alt = String(gps.altitude.meters()); 
String loc = latitude + ","+longitude+ ","+ alt ; 
 
  // Send HTTP GET request to the server 
  sendATCommand("AT+HTTPINIT"); 
  sendATCommand("AT+HTTPPARA=\"URL\",\"http://teleapigps.pythonanywhere.com/?lat="+latitude+"&long="+longitude+"&bat="+percentage+"&alt="+alt+ "\""); 
  sendATCommand("AT+HTTPACTION=0"); 
  delay(5000); 
 
  // Read and print the response from the server 
  sendATCommand("AT+HTTPREAD"); 
  delay(1000); 
 
  // Close the HTTP connection 
  sendATCommand("AT+HTTPTERM"); 
 
  delay(5000); // Wait before sending the next request 
  } 
} 
void setup() { 
  Serial.begin(9600); 
  sim800l.begin(9600); 
 
neogps.begin(9600); 
 if (!ina219.begin()) { 
    Serial.println("Gagal menemukan INA219"); 
    while (1); 
  } 
  Serial.println("NEO6M serial initialize"); 
 
 
  delay(2000); // Allow time for SIM800L to initialize 
 
  // Check if SIM800L is responding 
  sendATCommand("AT"); 
  delay(1000); 
 
  // Configure SIM800L for connecting to the internet 
  sendATCommand("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\""); 
  sendATCommand("AT+SAPBR=3,1,\"APN\",\"internet\""); 
  sendATCommand("AT+SAPBR=1,1"); 
  sendATCommand("AT+CIICR"); 
  sendATCommand("AT+CIFSR"); 
 
  delay(2000); // Wait before sending the GET request 
 
  String latitude = String(gps.location.lat(), 6); 
  String longitude = String(gps.location.lng(), 6); 
  String alt = String(gps.altitude.meters()); 
} 
 
void loop() { 
    float busVoltage = ina219.getBusVoltage_V(); 
    float batteryVoltage = 6.8; 
    float percentage = (batteryVoltage - busVoltage) / batteryVoltage * 100.0;  
gps2(); 
} 
 
void sendATCommand(String command) { 
  sim800l.println(command); 
  if (DEBUG) { 
    Serial.println(command); 
  } 
  delay(1000); 
   
  while (sim800l.available()) { 
    String response = sim800l.readString(); 
    if (DEBUG) { 
      Serial.println(response); 
    } 
  } 
}
