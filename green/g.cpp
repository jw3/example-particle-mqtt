#include <Particle.h>
#include <MQTT.h>

SYSTEM_MODE(MANUAL)

#ifndef BROKER
#define BROKER "raspberrypi"
#endif

void callback(char* topic, byte* payload, unsigned int length);

MQTT client(BROKER, 1883, callback);
SerialLogHandler logHandler;
constexpr const char* topic = "/E/color";
constexpr const char* color = "GREEN";

void callback(char* topic, byte* payload, unsigned int length) {
   Log.info("callback", color);

   char p[length + 1];
   memcpy(p, payload, length);
   p[length] = NULL;

   if(!strcmp(p, "RED"))
      RGB.color(255, 0, 0);
   else if(!strcmp(p, "BLUE"))
      RGB.color(0, 0, 255);
   else if(!strcmp(p, "ORANGE"))
      RGB.color(255, 128, 0);
}

bool ButtonPressed = false;
bool WasPressed() {
   if(ButtonPressed) {
      ButtonPressed = false;
      return true;
   }
   return false;
}
void button_handler(system_event_t event, int duration) {
   if(!duration && !ButtonPressed) ButtonPressed = true;
}

void setup() {
   RGB.control(true);
   RGB.color(255, 255, 255);

   WiFi.on();
   WiFi.connect();
   Log.info("WiFi connecting");
   while(!WiFi.ready()) {
      if(!WiFi.connecting()) {
         WiFi.connect();
         Log.info("WiFi connecting...");
      }
   }
   Log.info("WiFi connected");

   Log.info("MQTT connecting");
   while(!client.isConnected()) {
      Log.info("MQTT connecting...");
      client.connect(color);
   }
   Log.info("MQTT connected!");

   client.subscribe(topic);
   RGB.color(0, 255, 0);

   System.on(button_status, button_handler);
}

void loop() {
   if(WasPressed()) {
      RGB.color(0, 255, 0);
      client.publish(topic, color);
   }
   if(client.isConnected())
      client.loop();
}
