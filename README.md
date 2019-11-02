particle mqtt
===

- https://github.com/hirotakaster/MQTT.git
- https://docs.particle.io/reference/device-os/firmware/photon/#manual-mode
- https://github.com/jw3/conan-particle


```
if want to use IP address,
byte server[] = { XXX,XXX,XXX,XXX };
MQTT client(server, 1883, callback);

if want to use domain name,
exp) iot.eclipse.org is Eclipse Open MQTT Broker: https://iot.eclipse.org/getting-started
MQTT client("iot.eclipse.org", 1883, callback);
```
