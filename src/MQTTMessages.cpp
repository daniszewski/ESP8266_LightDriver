#include "MQTTMessages.h"
#include <WiFiClient.h>
#include "PubSubClient.h"
#include <cstring>
#include <queue>
#include "Commands.h"

#define MAX_QUEUE_SIZE 100

bool mqtt_enabled = false;
char *mqtt_server = nullptr;
uint16_t mqtt_server_port = 1883;
char *mqtt_user = nullptr;
char *mqtt_pwd = nullptr;

WiFiClient wifi;
PubSubClient subclient(wifi);
std::queue<String> topics;
std::queue<String> messages;

MQTTMessagesClass::MQTTMessagesClass() {
    queueSize = 0;
}

void MQTTMessagesClass::enable() {
    mqtt_enabled = true;
    connectTry = 10000;
}

void MQTTMessagesClass::disable() {
    mqtt_enabled = false;
}

bool MQTTMessagesClass::isEnabled() {
    return mqtt_enabled;
}

void MQTTMessagesClass::setServer(const char *server, int port, const char *user, const char *pwd) {
    if (mqtt_server) free(mqtt_server);
    if (mqtt_user) free(mqtt_user);
    if (mqtt_pwd) free(mqtt_pwd);

    mqtt_server = strdup(server);
    mqtt_user = strdup(user);
    mqtt_pwd = strdup(pwd);

    mqtt_server_port = port;

    DEBUGMQTT("[MQTT] service enabled: %s:%d\n", mqtt_server, mqtt_server_port);
    enable();
}

void MQTTMessagesClass::subscribe(const String topic) {
    DEBUGMQTT("[MQTT] subscribe: %s\n", topic.c_str());
    subclient.subscribe(topic.c_str());
}

void MQTTMessagesClass::messageReceived(char *topic, unsigned char *payload, unsigned int len) {
    char message[len+1];
    strcpy(message, (char*)payload);
    message[len] = 0;
    DEBUGMQTT("[MQTT] message received: %s: [%d chars] %s\n", topic, len, message);
    if (String(topic).endsWith("/cmd")) {
        execute(String(message));
    } else if (String(topic).endsWith("/script")) {
        executeScript(String(message));
    }
}

void MQTTMessagesClass::begin() {
    subclient.setCallback([=](char *topic, uint8_t *payload, unsigned int len) { this->messageReceived(topic, payload, len); });
}

void MQTTMessagesClass::handle() {
    if (!mqtt_enabled && subclient.connected()) {
        DEBUGMQTT("[MQTT] disabling\n");
        subclient.disconnect();
        wifi.stopAll();
    } else if (mqtt_enabled) {
        if (!subclient.loop()) {
            if (++connectTry > 10000) {//20 secs?
                subclient.setServer(mqtt_server, mqtt_server_port);
                DEBUGMQTT("[MQTT] connecting\n");
                if (subclient.connect("esp8266", mqtt_user, mqtt_pwd)) {
                    DEBUGMQTT("[MQTT] connected\n");
                } else {
                    DEBUGMQTT("[MQTT] connection error (%d)\n", subclient.state());
                }
                connectTry = 0;
            }
        } else {
            connectTry = 0;
            while (queueSize) {
                subclient.publish(topics.front().c_str(), messages.front().c_str());
                queueSize--;
            }
        }
    }
}

void MQTTMessagesClass::sendMessage(const String topic, const String message) {
    if (mqtt_enabled) {
        if (queueSize < MAX_QUEUE_SIZE) {
            DEBUGMQTT("[MQTT] sending message %s: %s\n", topic.c_str(), message.c_str());
            topics.push(topic);
            messages.push(message);
            queueSize++;
        } else {
            DEBUGMQTT("[MQTT] queue full\n");
        }
    }
}

MQTTMessagesClass MQTTMessages;
