#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <stdio.h>
#include <stdbool.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <driver/adc.h>

// Set to true to define Relay as Normally Open (NO)
#define RELAY_NO false

// Set number of relays
#define NUM_RELAYS 5

// Assign each GPIO to a relay
int relayGPIOs[NUM_RELAYS] = {25, 27, 26, 5, 18};

// Assign each GPIO to a relay
// Define the GPIO pins for the relays
#define MAIN_RELAY_PIN GPIO_NUM_25
#define GENERATOR1_RELAY_PIN GPIO_NUM_27
#define GENERATOR2_RELAY_PIN GPIO_NUM_26
#define SAFETY_RELAY1_PIN GPIO_NUM_5

#define SAFETY_RELAY2_PIN GPIO_NUM_18
// Define the ADC channels for voltage sensors
#define VOLTAGE_SENSOR1_CHANNEL ADC1_CHANNEL_5
#define VOLTAGE_SENSOR2_CHANNEL ADC1_CHANNEL_4
#define VOLTAGE_SENSOR3_CHANNEL ADC1_CHANNEL_7
// Define the voltage threshold for triggering relays
#define VOLTAGE_THRESHOLD 1000
// Define the delay for checking voltage after power loss (in milliseconds)
#define VOLTAGE_CHECK_DELAY 20000
// Define the states for the power sources
bool mainPowerOn = true;  // Assuming main power is initially available
bool generator1On = false;
bool generator2On = false;
bool loadPowerOn = true;  // Assuming load power is initially available
bool safety1On = false;
bool safety2On = false;

// Other definitions and global variables for power control (as in the second code)

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 34px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 68px}
    input:checked+.slider {background-color: #2196F3}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
  </style>
</head>
<body>
  <h2>ESP Web Server</h2>
  %BUTTONPLACEHOLDER%
<script>function toggleCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ xhr.open("GET", "/update?relay="+element.id+"&state=1", true); }
  else { xhr.open("GET", "/update?relay="+element.id+"&state=0", true); }
  xhr.send();
}</script>
</body>
</html>
)rawliteral";

// Replaces placeholder with button section in your web page
String processor(const String& var){
  //Serial.println(var);
  if(var == "BUTTONPLACEHOLDER"){
    String buttons ="";
    for(int i=1; i<=NUM_RELAYS; i++){
      String relayStateValue = relayState(i);
      buttons+= "<h4>Relay #" + String(i) + " - GPIO " + relayGPIOs[i-1] + "</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"" + String(i) + "\" "+ relayStateValue +"><span class=\"slider\"></span></label>";
    }
    return buttons;
  }
  return String();
}

String relayState(int numRelay){
  if(RELAY_NO){
    if(digitalRead(relayGPIOs[numRelay-1])){
      return "";
    }
    else {
      return "checked";
    }
  }
  else {
    if(digitalRead(relayGPIOs[numRelay-1])){
      return "checked";
    }
    else {
      return "";
    }
  }
  return "";
}// Add the index_html[] and processor() functions from the first code

// Replace with your network credentials
const char* ssid = "Tpj";
const char* password = "tpj_7803";

// Rest of the second code (main logic and functions)

void setup() {
  // Rest of the setup code from the second code
  adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(VOLTAGE_SENSOR1_CHANNEL, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(VOLTAGE_SENSOR2_CHANNEL, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(VOLTAGE_SENSOR3_CHANNEL, ADC_ATTEN_DB_11);
    // Initialize GPIO pins for relays
    gpio_set_direction(MAIN_RELAY_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(GENERATOR1_RELAY_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(GENERATOR2_RELAY_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(SAFETY_RELAY1_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(SAFETY_RELAY2_PIN, GPIO_MODE_OUTPUT);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Add other routes as needed for the web server

  // Handle relay control requests
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String relayNumStr = request->getParam("relay")->value();
    int relayNum = relayNumStr.toInt();
    String relayStateStr = request->getParam("state")->value();
    int relayState = relayStateStr.toInt();

    // Check if the relay number is valid
    if (relayNum > 0 && relayNum <= NUM_RELAYS) {
      // Control the relay based on the relayState value
      digitalWrite(relayGPIOs[relayNum - 1], RELAY_NO ? !relayState : relayState);
      // Respond with "OK" status
      request->send(200, "text/plain", "OK");
    } else {
      // Invalid relay number, respond with "Bad Request" status
      request->send(400, "text/plain", "Bad Request");
    }
  });

  server.begin();
}

void loop() {
  // Read voltage values from sensors
    uint32_t voltage1 = adc1_get_raw(VOLTAGE_SENSOR1_CHANNEL);
    uint32_t voltage2 = adc1_get_raw(VOLTAGE_SENSOR2_CHANNEL);
    uint32_t voltage3 = adc1_get_raw(VOLTAGE_SENSOR3_CHANNEL);
     // Check if main power is on
    if (voltage1 != 0) {
        mainPowerOn = true;
        loadPowerOn = true;
        // Main power is available
        generator1On = false;
        generator2On = false;
        safety1On = false;
        safety2On = false;
        gpio_set_level(MAIN_RELAY_PIN, 0);          // open main relay
        gpio_set_level(GENERATOR1_RELAY_PIN, 0);    // Turn off generator 1 relay
        gpio_set_level(GENERATOR2_RELAY_PIN, 0);    // Turn off generator 2 relay
        gpio_set_level(SAFETY_RELAY1_PIN, 0);       // Close safety relay 1
        gpio_set_level(SAFETY_RELAY2_PIN, 0);       // Close safety relay 2
    } else if (voltage1 == 0 && voltage3 == 0) {
        // Main power is lost
        mainPowerOn = false;
        generator1On = true;
        loadPowerOn = false;
        gpio_set_level(GENERATOR1_RELAY_PIN, 1);
        gpio_set_level(MAIN_RELAY_PIN, 1);          // close main relay
        gpio_set_level(SAFETY_RELAY1_PIN, 0);       // Open safety relay 1
        gpio_set_level(SAFETY_RELAY2_PIN, 0);       // Open safety relay 2
      
        // Check if generator 1 power is on
        if (voltage2 != 0) {
      
            generator2On = false;
            safety1On = true;
            safety2On = false;
            loadPowerOn = true;
            gpio_set_level(GENERATOR2_RELAY_PIN, 0);  // Turn off generator 2 relay
            gpio_set_level(SAFETY_RELAY1_PIN, 1);     // Close safety relay 1
            gpio_set_level(SAFETY_RELAY2_PIN, 0);     // Close safety relay 2
        } 
    }
    if (voltage2 == 0 && safety1On == true ) {
            generator1On = false;
            generator2On = true;
            loadPowerOn = false;
            //safety2On = true;
            gpio_set_level(GENERATOR2_RELAY_PIN, 1);
            gpio_set_level(GENERATOR1_RELAY_PIN, 0);  // Open generator 1 relay
            gpio_set_level(SAFETY_RELAY1_PIN, 0);
             gpio_set_level(SAFETY_RELAY2_PIN, 0);
              
             if (voltage3 != 0 ) {
      
            generator2On = true;
            safety1On = false;
            safety2On = true;
            generator1On= false;
            loadPowerOn = true;
            gpio_set_level(GENERATOR2_RELAY_PIN, 1);
            gpio_set_level(GENERATOR1_RELAY_PIN, 0);  // Turn off generator 2 relay
            gpio_set_level(SAFETY_RELAY1_PIN, 0);     // Close safety relay 1
            gpio_set_level(SAFETY_RELAY2_PIN, 1);     // Close safety relay 2
        }
   
    }

    // Print the status to the Serial Monitor
    printf("---- Power Status ----\n");
    printf("Main Power: %s\n", mainPowerOn ? "ON" : "OFF");
    printf("Generator 1: %s\n", generator1On ? "ON" : "OFF");
    printf("Generator 2: %s\n", generator2On ? "ON" : "OFF");
    printf("Load Power: %s\n", loadPowerOn ? "ON" : "OFF");
    printf("----------------------\n");
    printf("Voltage 1: %u\n", voltage1);
    printf("Voltage 2: %u\n", voltage2);
    printf("Voltage 3: %u\n", voltage3);
    vTaskDelay(VOLTAGE_CHECK_DELAY / portTICK_PERIOD_MS);
}

