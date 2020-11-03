#include "src/xBee-arduino-master/XBee.h"
#include <SoftwareSerial.h>
#include <assert.h>

#define DEBUG //comment: off | uncomment: on
//#define VERBOSE //Only works with DEBUG set | comment: off | uncomment: on
#define makeValueToString(x) makestring(x)
#define makestring(x) #x

#define NODE_ID 2 // CHANGE FOR EVERY DEVICE
#define NODE_TYPE_ENDPOINT 0
#define NODE_TYPE_ROUTER 1
#define NODE_TYPE_COORDINATOR 2
#define NODE_TYPE NODE_TYPE_COORDINATOR //possible values: NODE_TYPE_ENDPOINT || NODE_TYPE_ROUTER || NODE_TYPE_COORDINATOR

#define PIN_RX_XBEE 2
#define PIN_TX_XBEE 3
#define PIN_SLEEP_XBEE 9

enum at_command_codes {
    AT_RE, //reset xBee
    AT_ID,
    AT_SM, //put before AT_CE
    AT_CE,
    AT_NI,
    AT_EE,
    AT_EO,
    AT_KY,
    AT_AP,    
    AT_WR  //write changes to persistent xBee
};

static_assert(
    NODE_TYPE == NODE_TYPE_ENDPOINT
    || NODE_TYPE == NODE_TYPE_ROUTER
    || NODE_TYPE == NODE_TYPE_COORDINATOR
    , "NODE_TYPE must be NODE_TYPE_ENDPOINT or NODE_TYPE_ROUTER or NODE_TYPE_COORDINATOR"
    );

SoftwareSerial* xBeeSerial;
XBee* xBee;
AtCommandRequest atRequest;
AtCommandResponse atResponse;


void setup() {
    Serial.begin(9600);

    #ifdef DEBUG
    Serial.println();
    Serial.println("##############");
    Serial.println("#  STARTING  #");
    Serial.println("##############");
    Serial.println();
    Serial.print("TYPE              : ");
    #if NODE_TYPE == NODE_TYPE_ROUTER
    Serial.println("ROUTER");
    #elif NODE_TYPE == NODE_TYPE_ENDPOINT
    Serial.println("ENDPOINT");
    #elif NODE_TYPE == NODE_TYPE_COORDINATOR
    Serial.println("COORDINATOR");
    #endif
    Serial.print("NODE_ID           : "); Serial.println(NODE_ID);
    Serial.println();
    Serial.println("PIN_RX_XBEE       : " + String(PIN_RX_XBEE));
    Serial.println("PIN_TX_XBEE       : " + String(PIN_TX_XBEE));
    Serial.println("PIN_SLEEP_XBEE    : " + String(PIN_SLEEP_XBEE));
    Serial.println("--------------");
    #endif

    xBeeSerial = new SoftwareSerial(PIN_RX_XBEE, PIN_TX_XBEE); // RX, TX
    xBeeSerial->begin(9600);

    xBee = new XBee;
    xBee->setSerial(*xBeeSerial);
}

void loop() {
    digitalWrite(PIN_SLEEP_XBEE, LOW); //wake up XBee just in case
    delay(200); //wait for the XBee to wake up

    int MAX_VALUE_LENGHT = 5;
    uint8_t at_Cmd[2];
    uint8_t at_Value[MAX_VALUE_LENGHT];
    int at_Value_length;

    for (int at_command_codes_int = AT_ID; at_command_codes_int <= AT_WR; at_command_codes_int++) {
        at_command_codes at_cmd_code = static_cast<at_command_codes>(at_command_codes_int);

        buildAtCommand(at_cmd_code, at_Cmd, at_Value, at_Value_length, MAX_VALUE_LENGHT);

        atRequest.clearCommandValue();
        atResponse.reset();

        atRequest.setCommand(at_Cmd);
        atRequest.setCommandValue(at_Value);
        atRequest.setCommandValueLength(at_Value_length);

        sendAtCommand(atRequest, atResponse);
        delay(1000);
    }

    Serial.println("DONE!");
    Serial.flush();

    delay(2000);
    abort();
}


void sendAtCommand(AtCommandRequest& atRequest, AtCommandResponse& atResponse) {
    xBee->send(atRequest);

    Serial.print("[.] Sending ");
    Serial.print((char)atRequest.getCommand()[0]);
    Serial.println((char)atRequest.getCommand()[1]);

    if (xBee->readPacket(5000)) {
        if (xBee->getResponse().getApiId() == AT_COMMAND_RESPONSE) {
            xBee->getResponse().getAtCommandResponse(atResponse);

            if (atResponse.isOk()) {
                Serial.print("[+] Command ");
                Serial.print((char)atResponse.getCommand()[0]);
                Serial.print((char)atResponse.getCommand()[1]);
                Serial.println(" was successful!");

                if (atResponse.getValueLength() > 0) {
                    #ifdef DEBUG
                    #ifdef VERBOSE
                    Serial.print("[.] Response value length is ");
                    Serial.println(atResponse.getValueLength(), DEC);
                    #endif
                    #endif

                    Serial.print("[.] Response value: ");
                    Serial.print("0x(");
                    for (int i = 0; i < atResponse.getValueLength(); i++) {
                        if (i != 0) Serial.print(" ");
                        Serial.print(atResponse.getValue()[i], HEX);
                    }
                    Serial.println(")");
                }
            } else {
                Serial.print("[-] Command return error code: ");
                Serial.println(atResponse.getStatus(), HEX);
            }
        } else {
            Serial.print("[-] Expected AT response but got ");
            Serial.println(xBee->getResponse().getApiId(), HEX);
        }
    } else {
        // at command failed
        if (xBee->getResponse().isError()) {
            Serial.print("[-] Error reading packet. Error code: ");
            Serial.println(xBee->getResponse().getErrorCode());
        } else {
            Serial.println("[-] No response from radio");
        }
    }
}

void buildAtCommand(at_command_codes at_cmd_code, uint8_t* at_Cmd, uint8_t* at_Value, int& at_Value_length, const int MAX_VALUE_LENGHT) {
    switch (at_cmd_code) {
    case(AT_RE): {
        at_Cmd[0] = 'R'; at_Cmd[1] = 'E';
        at_Value_length = 0;
        break;
    }
    case(AT_ID): {
        at_Cmd[0] = 'I'; at_Cmd[1] = 'D';
        at_Value_length = 2;
        at_Value[0] = 0x11; at_Value[1] = 0x11;
        break;
    }
    case(AT_CE): {
        at_Cmd[0] = 'C'; at_Cmd[1] = 'E';
        at_Value_length = 1;
        if (NODE_TYPE == NODE_TYPE_ENDPOINT) {
            at_Value[0] = 0x0;
        } else if (NODE_TYPE == NODE_TYPE_ROUTER) {
            at_Value[0] = 0x0;
        } else if (NODE_TYPE == NODE_TYPE_COORDINATOR) {
            at_Value[0] = 0x1;
        } else {
            Serial.print("[-] UNRECOGNIZED NODE_TYPE");
            Serial.println(NODE_TYPE);
            Serial.flush();
            abort();
        }
        break;
    }
    case(AT_NI): {
        at_Cmd[0] = 'N'; at_Cmd[1] = 'I';

        String node_id_str = makeValueToString(NODE_ID);
        at_Value_length = 2 + node_id_str.length();

        if (NODE_TYPE == NODE_TYPE_ENDPOINT) {
            at_Value[0] = 'E';
        } else if (NODE_TYPE == NODE_TYPE_ROUTER) {
            at_Value[0] = 'R';
        } else if (NODE_TYPE == NODE_TYPE_COORDINATOR) {
            at_Value[0] = 'C';
        } else {
            Serial.print("[-] UNRECOGNIZED NODE_TYPE");
            Serial.println(NODE_TYPE);
            Serial.flush();
            abort();
        }

        at_Value[1] = '_';

        for (int i = 0; i < node_id_str.length(); i++) {
            at_Value[2 + i] = node_id_str[i];
        }
        break;
    }
    case(AT_EE): {
        at_Cmd[0] = 'E'; at_Cmd[1] = 'E';
        at_Value_length = 1;
        at_Value[0] = 0x1;
        break;
    }
    case(AT_EO): {
        at_Cmd[0] = 'E'; at_Cmd[1] = 'O';
        at_Value_length = 1;
        at_Value[0] = 0x2;
        break;
    }
    case(AT_KY): {
        at_Cmd[0] = 'K'; at_Cmd[1] = 'Y';
        at_Value_length = 5;
        at_Value[0] = 0x48;
        at_Value[1] = 0x2F;
        at_Value[2] = 0x2C;
        at_Value[3] = 0x82;
        at_Value[4] = 0x2;
        break;
    }
    case(AT_AP): {
        at_Cmd[0] = 'A'; at_Cmd[1] = 'P';
        at_Value_length = 1;
        at_Value[0] = 0x2;
        break;
    }
    case(AT_SM): {
        at_Cmd[0] = 'S'; at_Cmd[1] = 'M';
        at_Value_length = 1;
        if (NODE_TYPE == NODE_TYPE_ENDPOINT) {
            at_Value[0] = 0x1;
        } else if (NODE_TYPE == NODE_TYPE_ROUTER || NODE_TYPE == NODE_TYPE_COORDINATOR) {
            at_Value[0] = 0x0;
        } else {
            Serial.print("[-] UNRECOGNIZED NODE_TYPE");
            Serial.println(NODE_TYPE);
            Serial.flush();
            abort();
        }
        break;
    }
    case(AT_WR): {
        at_Cmd[0] = 'W'; at_Cmd[1] = 'R';
        at_Value_length = 0;
        break;
    }
    default: {
        Serial.print("[-] UNRECOGNIZED COMMAND");
        Serial.println(at_cmd_code);
        Serial.flush();
        abort();
    }
    }

    if (at_Value_length > MAX_VALUE_LENGHT) {
        Serial.print("at_Value_length > MAX_VALUE_LENGHT");
        Serial.flush();
        abort();
    }
}