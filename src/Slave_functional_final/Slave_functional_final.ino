#include <ESP8266WiFi.h>
#include <espnow.h>

#include <FirebaseESP8266.h>
#include <WiFiClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

long duration;
float distance;
int trigPin = 14;
int echoPin = 12;

float temp_sl;
float hum_sl;


// Variable to store if sending data was successful
String success;

//Provide the token generation process info.
#include <addons/TokenHelper.h>

//Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>


/* 1. Define the WiFi credentials */
#define WIFI_SSID "explorer"
#define WIFI_PASSWORD "87654321"

/* 2. Define the API Key */
#define API_KEY ""

/* 3. Define the RTDB URL */
#define DATABASE_URL ""

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL ""
#define USER_PASSWORD ""


//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

unsigned long count = 0;

// REPLACE WITH THE MAC Address of your receiver
uint8_t broadcastAddress[] = { 0x5C, 0xCF, 0x7F, 0x3D, 0x03, 0xD2 };

// Digital pin connected to the DHT sensor
#define DHTPIN 5

// Uncomment the type of sensor in use:
#define DHTTYPE DHT11  // DHT 11


DHT dht(DHTPIN, DHTTYPE);

// Define variables to store DHT readings to be sent
float temperature;
float humidity;

// Define variables to store incoming readings
float incomingTemp;
float incomingHum;
float incomingDist;
// Updates DHT readings every 10 seconds
const long interval = 10000;
unsigned long previousMillis = 0;  // will store last time DHT was updated


//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
  float temp;
  float hum;
  float dist;  
} struct_message;

// Create a struct_message called DHTReadings to hold sensor readings
struct_message DHTReadings;



// Create a struct_message to hold incoming sensor readings
struct_message incomingReadings;



// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0) {
    Serial.println("Delivery success");
  } else {
    Serial.println("Delivery fail");
  }
}



// Callback when data is received
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  Serial.print("Bytes received: ");
  Serial.println(len);
  incomingTemp = incomingReadings.temp;
  incomingHum = incomingReadings.hum;
  incomingDist = incomingReadings.dist;
}



void printIncomingReadings() {
 // Display Readings in Serial Monitor
 Serial.println("INCOMING READINGS");
 Serial.print("Temperature: ");
 Serial.print(incomingTemp);
 Serial.println(" ºC");
 Serial.print("Humidity: ");
 Serial.print(incomingHum);
 Serial.println(" %");
 Serial.print(incomingDist);
 Serial.println("cm");
}




//function to read values from the own dht sensor
void getReadings() {
  // Read Temperature
  temp_sl = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float t = dht.readTemperature(true);
  if (isnan(temperature)) {
    Serial.println("Failed to read from DHT");
    temp_sl = 0.0;
  }
  hum_sl = dht.readHumidity();
  if (isnan(humidity)) {
    Serial.println("Failed to read from DHT"); 
    hum_sl = 0.0;
  }
}




float getHeight() {
digitalWrite(trigPin, LOW);
delayMicroseconds(2);
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);
duration = pulseIn(echoPin, HIGH);
float d1 = duration * 0.034/2;
return d1;
}




void setup() {

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // Init Serial Monitor
  Serial.begin(115200);

  // Init DHT sensor
  dht.begin();

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  //WiFi.disconnect();
 
  //////////////////////////////////////////////////////
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  
  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Set ESP-NOW Role
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);//SLAVE ROLE

  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback;  //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);

  //Comment or pass false value when WiFi reconnection will control by your code or third party library
  Firebase.reconnectWiFi(true);

  Firebase.setDoubleDigits(5);
  /////////////////////////////////////////////////////
  
  
  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
}

void loop() {
  float d;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the DHT values
    previousMillis = currentMillis;

    getReadings();

    d = getHeight();
    Serial.print("Slave_Distance  ");
    Serial.println(d);

    // Print incoming readings
   printIncomingReadings();
    Serial.print("Slave_temperature  ");
    Serial.println(temp_sl);    
    Serial.print("Slave_humidity  ");
    Serial.println(hum_sl);

    float h = incomingTemp;
    float t = incomingHum;
    float d_mas = incomingDist;    

    if (Firebase.ready() && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
      sendDataPrevMillis = millis();
     Serial.printf("Set Temperature... %s\n", Firebase.setFloat(fbdo, F("/master/temperature"), t) ? "ok" : fbdo.errorReason().c_str());

     Serial.printf("Get Temperature... %s\n", Firebase.getFloat(fbdo, F("/master/temperature")) ? String(fbdo.to<float>()).c_str() : fbdo.errorReason().c_str());

     Serial.printf("Set Humidity... %s\n", Firebase.setDouble(fbdo, F("/master/humidity"), h) ? "ok" : fbdo.errorReason().c_str());

     Serial.printf("Get Humidity... %s\n", Firebase.getDouble(fbdo, F("/master/humidity")) ? String(fbdo.to<double>()).c_str() : fbdo.errorReason().c_str());

     Serial.printf("Set Humidity... %s\n", Firebase.setDouble(fbdo, F("/master/distance"), d_mas) ? "ok" : fbdo.errorReason().c_str());

     Serial.printf("Get Humidity... %s\n", Firebase.getDouble(fbdo, F("/master/distance")) ? String(fbdo.to<double>()).c_str() : fbdo.errorReason().c_str());


     
      
      Serial.printf("Set Temperature... %s\n", Firebase.setFloat(fbdo, F("/slave/temperature"), temp_sl) ? "ok" : fbdo.errorReason().c_str());

      Serial.printf("Get Temperature... %s\n", Firebase.getFloat(fbdo, F("/slave/temperature")) ? String(fbdo.to<float>()).c_str() : fbdo.errorReason().c_str());

      Serial.printf("Set Humidity... %s\n", Firebase.setDouble(fbdo, F("/slave/humidity"), hum_sl) ? "ok" : fbdo.errorReason().c_str());

      Serial.printf("Get Humidity... %s\n", Firebase.getDouble(fbdo, F("/slave/humidity")) ? String(fbdo.to<double>()).c_str() : fbdo.errorReason().c_str());


      Serial.printf("Set Distance... %s\n", Firebase.setDouble(fbdo, F("/slave/distance"), d) ? "ok" : fbdo.errorReason().c_str());

      Serial.printf("Get Distance... %s\n", Firebase.getDouble(fbdo, F("/slave/distance")) ? String(fbdo.to<double>()).c_str() : fbdo.errorReason().c_str());


      //For the usage of FirebaseJson, see examples/FirebaseJson/BasicUsage/Create_Parse_Edit.ino
      FirebaseJson json;

      if (count == 0) {
        json.set("value/round/" + String(count), F("cool!"));
        json.set(F("vaue/ts/.sv"), F("timestamp"));
        Serial.printf("Set json... %s\n", Firebase.set(fbdo, F("/test2/json"), json) ? "ok" : fbdo.errorReason().c_str());
      } else {
        json.add(String(count), "smart!");
        Serial.printf("Update node... %s\n", Firebase.updateNode(fbdo, F("/test2/json/value/round"), json) ? "ok" : fbdo.errorReason().c_str());
      }

      Serial.println();

      count++;
    }
  }
}
