#include <Particle.h>
#include <MQTT.h>

#include <mqttcloud.h>
#include <colormap.h>

SYSTEM_MODE(MANUAL)

#ifndef COLOR
#define COLOR "magicpink"
#endif

#ifndef BROKER
#define BROKER "raspberrypi"
#endif

particle::MQTTCloud Cloud(BROKER);
constexpr const char* topic = "color";
constexpr const char* color = COLOR;

void callback(const char* t, const char* e) {
   setRGB(e);
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

   Log.info("Cloud connecting");
   while(!Cloud.isConnected()) {
      Log.info("Cloud connecting...");
      Cloud.connect(color);
   }
   Log.info("Cloud connected!");

   Cloud.subscribe(topic, callback);
   setRGB(color);

   System.on(button_status, button_handler);
}

void loop() {
   if(WasPressed()) {
      setRGB(color);
      Cloud.publish(topic, color, PRIVATE);
   }
   if(Cloud.isConnected())
      Cloud.loop();
}
