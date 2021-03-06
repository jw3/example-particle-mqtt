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

SerialLogHandler logHandler;
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

bool d4 = false;
int d4F(String arg) {
   d4 = !d4;
   digitalWrite(D4, d4);
   return 0;
}

void setup() {
#ifdef ETH
   NetworkClass& Net = Ethernet;
#else
   NetworkClass& Net = WiFi;
#endif

   RGB.control(true);
   setRGB("white");

   pinMode(D4, OUTPUT);
   digitalWrite(D4, LOW);

   Net.on();
   Net.connect();
   Log.info("Network connecting");
   while(!Ethernet.ready()) {
      if(!Ethernet.connecting()) {
         Ethernet.connect();
         Log.info("Network connecting...");
      }
   }
   Log.info("Network connected");

   Log.info("Cloud connecting");
   while(!Cloud.isConnected()) {
      Log.info("Cloud connecting...");
      Cloud.connect(color);
   }
   Log.info("Cloud connected!");

   Cloud.subscribe(topic, callback);
   setRGB(color);

   Cloud.function("d4", d4F);

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
