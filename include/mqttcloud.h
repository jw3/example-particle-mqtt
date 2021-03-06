#pragma once

#include <Particle.h>
#include <MQTT.h>
#include <map>

auto mqttcloudid = System.deviceID().substring(0, 6);

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

typedef user_std_function_int_str_t CloudFunc;
typedef std::shared_ptr<CloudFunc> CloudFuncPtr;

typedef std::function<void(const char*, const char*)> CloudEventHandler;
typedef std::shared_ptr<CloudEventHandler> CloudEventHandlerPtr;

const std::string E("/E/");
const std::string Fn(String::format("/F/%s/", mqttcloudid.c_str()));

namespace particle
{
   struct MQTTCloud
   {
      explicit MQTTCloud(const char* broker, uint16_t port = 1883)
            : client(const_cast<char*>(broker), port, 5, std::bind(&MQTTCloud::callback, this, _1, _2, _3)) {
      }

      ~MQTTCloud() {
         if(client.isConnected()) client.disconnect();
      }

      bool publish(const char* eventName, const char* data, PublishFlags flags) {
         const auto e = E + eventName;
         return client.publish(e.c_str(), data);
      }

      bool subscribe(const char* eventName, EventHandler handler, Spark_Subscription_Scope_TypeDef scope = MY_DEVICES) {
         const auto e = E + eventName;
         handlers[e] = std::make_shared<CloudEventHandler>(handler);
         return client.subscribe(e.c_str());
      }

      bool function(const char* funcKey, user_function_int_str_t func, void* reserved = nullptr) {
         auto f = std::function<user_function_int_str_t>(func);
         return function(funcKey, f);
      }

      bool function(const char* funcKey, user_std_function_int_str_t& func, void* reserved = nullptr) {
         const auto e = Fn + funcKey;
         functions[e] = std::make_shared<CloudFunc>(func);
         return client.subscribe(e.c_str());
      }

      bool loop() { return client.loop(); }
      bool connect(const char* id) { return client.connect(id); }
      bool isConnected() { return client.isConnected(); }

   private:
      MQTT client;
      std::map<std::string, CloudFuncPtr> functions;
      std::map<std::string, CloudEventHandlerPtr> handlers;

      void callback(char* t, uint8_t* d, uint32_t c) {
         const std::string e(t);
         const std::string p(reinterpret_cast< char const* >(d), c);

         if(e.rfind(Fn, 0) == 0) {     // Function
            const auto v = functions.find(e);
            if(v != functions.end()) {
               (*v->second)(p.c_str());
            }
         }
         else if(e.rfind(E, 0) == 0) { // Event
            const auto v = handlers.find(e);
            if(v != handlers.end()) {
               (*v->second)(e.substr(E.length()).c_str(), p.c_str());
            }
         }
      }
   };
}
