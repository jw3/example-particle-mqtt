#include <Particle.h>
SYSTEM_MODE(MANUAL);

//
// https://github.com/hirotakaster/MQTT/blob/master/examples/mqtttest/mqtttest.ino

#include "MQTT.h"

void callback(char* topic, byte* payload, unsigned int length);

/**
 * if want to use IP address,
 * byte server[] = { XXX,XXX,XXX,XXX };
 * MQTT client(server, 1883, callback);
 * want to use domain name,
 * exp) iot.eclipse.org is Eclipse Open MQTT Broker: https://iot.eclipse.org/getting-started
 * MQTT client("iot.eclipse.org", 1883, callback);
 **/
byte server[] = {192, 168, 0, 10};
MQTT client(server, 1883, callback);
SerialLogHandler logHandler;

// recieve message
void callback(char* topic, byte* payload, unsigned int length) {
   char p[length + 1];
   memcpy(p, payload, length);
   p[length] = NULL;

   if (!strcmp(p, "RED"))
      RGB.color(255, 0, 0);
   else if (!strcmp(p, "GREEN"))
      RGB.color(0, 255, 0);
   else if (!strcmp(p, "BLUE"))
      RGB.color(0, 0, 255);
   else
      RGB.color(255, 255, 255);
   delay(1000);
}


void setup() {
   RGB.control(true);
   RGB.color(128, 128, 0);

   WiFi.on();
   WiFi.connect();
   Log.info("WiFi connecting");
   while(!WiFi.ready()) {
      if(!WiFi.connecting()) {
         WiFi.connect();
         Log.info("WiFi connecting...");
      }
   }
   Log.info("MQTT connected!");

   ;

   Log.info("WiFi connected");

   Log.info("MQTT connecting");
   while(!client.connect("sparkclient")) {
      Log.info("MQTT connecting...");
   }
   Log.info("MQTT connected!");

   if(client.isConnected()) {
      RGB.color(0, 255, 0);
      client.publish("outTopic/message", "hello world");
      client.subscribe("inTopic/message");
   }
   else
      RGB.color(255, 0, 0);
}

void loop() {
   if (client.isConnected()){
      client.loop();
      client.publish("outTopic/message", "tic");
   }
}
