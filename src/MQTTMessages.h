#ifndef MQTTMESSAGES_H
#define MQTTMESSAGES_H

#include <WString.h>

class MQTTMessagesClass {
public:
    MQTTMessagesClass();
    void begin();
    void handle();
    void enable();
    void disable();
    bool isEnabled();
    void setServer(const char *server, int port, const char *user, const char *passwd);
    void subscribe(const String topic);
    void sendMessage(const String topic, const String message);
private:
    void messageReceived(char *topic, unsigned char *payload, unsigned int len);
    int connectTry;
    int queueSize;
};

extern MQTTMessagesClass MQTTMessages;

#endif