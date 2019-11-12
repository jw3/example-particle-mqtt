#include <Particle.h>
#include <MQTT.h>
#include <colormap.h>

SYSTEM_MODE(MANUAL)

#ifndef COLOR
#define COLOR "magicpink"
#endif

#ifndef BROKER
#define BROKER "raspberrypi"
#endif

void callback(char* topic, byte* payload, unsigned int length);

MQTT client(BROKER, 1883, callback);
SerialLogHandler logHandler;
constexpr const char* topic = "/E/color";
constexpr const char* color = COLOR;

void callback(char* t, byte* e, unsigned int length) {
   Log.info("callback %s", color);

   char p[length + 1];
   memcpy(p, e, length);
   p[length] = NULL;

   setRGB(p);
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
   setRGB("white");

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
   setRGB(color);

   System.on(button_status, button_handler);
}

void loop() {
   if(WasPressed()) {
      setRGB(color);
      client.publish(topic, color);
   }
   if(client.isConnected())
      client.loop();
}
