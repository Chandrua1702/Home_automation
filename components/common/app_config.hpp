#pragma once

/* ========= HiveMQ Cloud MQTT ========= */
#define MQTT_BROKER    "mqtts://12b3f8b0d36b46e1885d0944faa068c6.s1.eu.hivemq.cloud:8883"
#define MQTT_USERNAME  "esp32"
#define MQTT_PASSWORD  "Chandru@123"
#define MQTT_CLIENTID  "ESP32_HOME_1"

/* ========= Relay GPIOs ========= */
#define RELAY1_GPIO     14
#define RELAY2_GPIO     27
#define RELAY3_GPIO     26
#define RELAY4_GPIO     25

/*====== Test GPIO using HiveMQ Cloud in the terminal cmd ====*/

// mosquitto_pub -h 12b3f8b0d36b46e1885d0944faa068c6.s1.eu.hivemq.cloud 
//-p 8883 
//--cafile hivemq-ca.pem 
//-u esp32 -P Chandru@123 
//-t home/esp32_001/relay/1/cmd 
//-m OFF