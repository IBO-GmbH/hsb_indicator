#include "mqtt_publisher.hpp"

#include <stdexcept>

mqtt_publisher::mqtt_publisher(const mqtt_settings& settings_)
    : client(wifi_client), settings(settings_) {}

void mqtt_publisher::set_mqtt_settings(const mqtt_settings& new_settings) {
  if (new_settings == settings) return;
  if ((new_settings.broker_ip != settings.broker_ip ||
       new_settings.broker_port != settings.broker_port) &&
      new_settings.broker_ip.has_value() &&
      !new_settings.broker_ip.value().empty() && new_settings.broker_port != 0)
    client.setServer(new_settings.broker_ip->c_str(), new_settings.broker_port);
  settings = new_settings;
  if (settings.keep_alive_time.has_value())
    client.setKeepAlive(settings.keep_alive_time.value());
}

void mqtt_publisher::connect_to_broker() {
  if (!settings.mqtt_enabled) return;
  if (!settings.broker_ip.has_value() || settings.broker_ip.value().empty() ||
      settings.broker_port == 0)
    return;
  if (connected_to_broker()) return;
  if (!connect_with_settings()) {
    std::string error_string("Could not connect to MQTT Broker! rc=");
    error_string += std::to_string(client.state());
    throw std::runtime_error(error_string.c_str());
  }
}

void mqtt_publisher::publish_value(const double& value) {
  if (!settings.mqtt_enabled) return;
  if (!settings.averaging_count.has_value()) return;
  if (!connected_to_broker()) return;
  std::string publish_base = settings.client_id + "/" + settings.room_name;
  boolean success = true;
  success &= client.publish((publish_base + "/start").c_str(), "start");
  success &= client.publish((publish_base + "/temperature").c_str(),
                            String(value).c_str());
  success &= client.publish((publish_base + "/end").c_str(), "end");
  delay(150);
}

bool mqtt_publisher::connected_to_broker() {
  return client.connected() && client.loop();
}

bool mqtt_publisher::connect_with_settings() {
  if (!settings.last_will_topic.has_value() ||
      !settings.last_will_qos.has_value() ||
      !settings.last_will_retain.has_value() ||
      !settings.last_will_message.has_value() ||
      settings.last_will_topic.value().empty() ||
      settings.last_will_message.value().empty())
    return client.connect(settings.client_id.c_str());
  return client.connect(
      settings.client_id.c_str(), settings.last_will_topic->c_str(),
      settings.last_will_qos.value(), settings.last_will_retain.value(),
      settings.last_will_message->c_str());
}