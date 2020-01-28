#include <Particle.h>
#include <MQTT.h>
#include <colormap.h>

SYSTEM_MODE(MANUAL)

#ifndef BROKER
#define BROKER "raspberrypi"
#endif

void callback(char* topic, byte* payload, unsigned int length);

MQTT client(BROKER, 1883, callback);
SerialLogHandler logHandler;
constexpr const char* topic = "/E/color";
constexpr const char* color = "blue";

void callback(char* t, byte* e, unsigned int length) {
   Log.info("callback %s", color);

   char p[length + 1];
   memcpy(p, e, length);
   p[length] = NULL;

   Log.info("Set color %s", p);
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

bool connected = false;
void setup() {
   NetworkClass& Net = Ethernet;

   RGB.control(true);
   setRGB("red");

   Net.on();
   Net.connect();
   Log.info("Net connecting");
   while(!Net.ready()) {
      if(!Net.connecting()) {
         Net.connect();
         Log.info("Net connecting...");
      }
   }
   Log.info("Net connected");

   setRGB("orange");

   while(!client.isConnected()) {
      Log.info("MQTT connecting...");
      client.connect(color);
   }
   Log.info("MQTT connected!");

   client.subscribe(topic);
   setRGB("green");

   System.on(button_status, button_handler);
   connected = true;
}

void loop() {
   if(WasPressed()) {
      if(connected){
         Log.info("MQTT disconnected");
         client.disconnect();
         connected = false;
      }
      else {
         while(!client.isConnected()) {
            Log.info("MQTT connecting...");
            client.connect(color, nullptr, nullptr, 0, MQTT::EMQTT_QOS::QOS0, 0, 0, false);
         }
//         client.subscribe(topic);
         Log.info("MQTT connected!");
         connected = true;
      }
   }
   if(client.isConnected())
      client.loop();
}
