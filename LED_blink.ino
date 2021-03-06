/*********************** Heart beat LED setup *******************************/
#include <ESP8266WiFi.h> 
#include "Adafruit_MQTT.h" 
#include "Adafruit_MQTT_Client.h" 
/************************* WiFi Access Point *********************************/ 
#define WLAN_SSID       "[Your SSID]" 
#define WLAN_PASS       "[Password]" 
#define MQTT_SERVER     "[Host IP-adress]" // static ip address
#define MQTT_PORT       1883                    
#define MQTT_USERNAME   "" 
#define MQTT_PASSWORD        ""
// Where I define LED GPIO
#define LED_PIN         15
/************ Global State ******************/ 
// Create an ESP8266 WiFiClient class to connect to the MQTT server. 
WiFiClient client; 
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details. 
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_PORT, MQTT_USERNAME, MQTT_PASSWORD); 
/****************************** Feeds ***************************************/ 
// Setup a feed called 'esp8266_led' for subscribing to changes. 
Adafruit_MQTT_Subscribe esp8266_led = Adafruit_MQTT_Subscribe(&mqtt, MQTT_USERNAME "/led/esp8266"); 
/*************************** Sketch Code ************************************/ 

void MQTT_connect(); //connects to the server
void setup() { 
 WiFi.mode (WIFI_STA); // turn off AP (Access point) mode
 Serial.begin(115200); 
 delay(10);
 // setup for the LED
 pinMode(LED_PIN, OUTPUT); 
 digitalWrite(LED_PIN, LOW);
 /****************** Connect to WiFi access point. **************************/
 Serial.println(); Serial.println(); 
 Serial.print("Connecting to "); 
 Serial.println(WLAN_SSID); 
 WiFi.begin(WLAN_SSID, WLAN_PASS); 
 while (WiFi.status() != WL_CONNECTED) { 
   delay(500); 
   Serial.print("."); 
 } 
 Serial.println(); 
 Serial.println("WiFi connected"); 
 Serial.println("IP address: "); Serial.println(WiFi.localIP());
 // Setup MQTT subscription for esp8266_led feed. 
 mqtt.subscribe(&esp8266_led); 
}  
uint32_t period= 5000; // 5 seconds
/********************** This is where the magic happens *********************/
void loop() {
 // Ensure the connection to the MQTT server is alive (this will make the first 
 // connection and automatically reconnect when disconnected).  See the MQTT_connect.
 // MQTT_connect(); has to be in every code (void loop) you want to add to the system.
 MQTT_connect(); 
 // this is our 'wait for incoming subscription' busy subloop 
 // Here it reads the subscription 
 Adafruit_MQTT_Subscribe *subscription; 
 while ((subscription = mqtt.readSubscription())) { 
   if (subscription == &esp8266_led) { 
     char *message = (char *)esp8266_led.lastread; 
     Serial.print(F("Got: ")); 
     Serial.println(message); 
        // Checking for message 
     if (strncmp(message, "ON", 2) == 0) { 
       // Turn the LED on, and make sure the led blinks for the set period.
       for( uint32_t tStart = millis(); (millis()-tStart) < period; ){ 
       heartBeat(1.2);
       }
      } 
    } 
  } 
}
/*** Function to connect and reconnect as necessary to the MQTT server. ***/ 
void MQTT_connect() { 
 int8_t ret; 
 // Stop if already connected. 
 if (mqtt.connected()) { 
   return; 
 } 
 Serial.print("Connecting to MQTT... "); 
 uint8_t retries = 3; 
 while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected 
      Serial.println(mqtt.connectErrorString(ret)); 
      Serial.println("Retrying MQTT connection in 5 seconds..."); 
      mqtt.disconnect(); 
      delay(5000);  // wait 5 seconds 
      retries--; 
      if (retries == 0) { 
        // basically die and wait for WDT to reset me 
        while (1); 
      } 
 } 
 Serial.println("MQTT Connected!"); 
} 
