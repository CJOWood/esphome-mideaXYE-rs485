#include "esphome.h"

#define RX_PIN 16
#define TX_PIN 17

#define CMD_VAL 1
#define SEND_MODE 6
#define SEND_FAN 7
#define SEND_TEMP 8
#define SEND_CRC 14
#define SEND_LEN 16

#define REC_MODE 8
#define REC_FAN 9
#define REC_TEMP 10
#define T1_INDEX 11
#define T2A_INDEX 12
#define T2B_INDEX 13
#define T3_INDEX 14

using namespace esphome;

#if defined(ESP8266)
    #define IS_8266 1
    HardwareSerial mySerial(0);
#else
    #define IS_8266 0
    HardwareSerial mySerial(2);
#endif

class xyeVars {
  public:
    uint8_t setTemp = 70;
    
    std::string op = "off";
    std::string fan = "auto";
    uint8_t fanBytes = 0x00;
    uint8_t opBytes = 0x00;
    uint8_t doneReading = 0;
    uint8_t prevResp = 0;
    uint8_t dumpResp = 0;
    uint8_t waitingToSend = 0;
    uint8_t waitingForResponse = 0;
    uint8_t commandSent = 0;
    uint8_t recData[30] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t sendData[16] = {0xaa, 0xc3, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x55};
    uint8_t waitSendData[16] = {0xaa, 0xc3, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x55};
    uint8_t queryData[16] = {0xaa, 0xc0, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x81, 0x55};
} xyeVars;
