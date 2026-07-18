#include <mcp_can.h>
#include <SPI.h>

// Note: Transmitting to car (t command) is not supported.


#define MCP2515_CS_PIN     53
#define MCP2515_IRQ_PIN    2

#define CAN_SPEED          CAN_500KBPS
#define MCP_CLOCK         MCP_8MHZ

#define SERIAL_BAUDRATE    1000000
#define ERROR_LED_PIN      13

MCP_CAN can(MCP2515_CS_PIN);

bool canOpen = false;

void setup() {
    Serial.begin(SERIAL_BAUDRATE);
    
    pinMode(MCP2515_IRQ_PIN, INPUT);
    pinMode(ERROR_LED_PIN, OUTPUT);
    
    if (can.begin(MCP_STD, CAN_SPEED, MCP_8MHZ) == CAN_OK) {
        Serial.print("\r");
    } else {
        while(1) {
            digitalWrite(ERROR_LED_PIN, HIGH);
            delay(200);
            digitalWrite(ERROR_LED_PIN, LOW);
            delay(200);
        }
    }
    
    can.setMode(MCP_NORMAL);
}

void loop() {
    while (Serial.available()) {
        handleSerial(Serial.read());
    }
    
    if (canOpen && digitalRead(MCP2515_IRQ_PIN) == LOW) {
        sendCANtoSerial();
    }
}

void handleSerial(uint8_t c) {
    static char cmd = 0;
    static uint8_t idx = 0;
    static uint32_t canId = 0;
    static uint8_t dlc = 0;
    static uint8_t data[8];
    static bool readingData = false;
    static uint8_t dataIdx = 0;
    
    if (c == '\r' || c == '\n') {
        if (cmd == 't' && readingData && dataIdx == dlc) {
            can.sendMsgBuf(canId, 0, dlc, data);
        }
        cmd = 0;
        idx = 0;
        readingData = false;
        dataIdx = 0;
        canId = 0;
        dlc = 0;
        return;
    }
    
    if (cmd == 0 && (c == 'O' || c == 'C' || c == 'S' || c == 'F')) {
        cmd = c;
        if (c == 'O') {
            canOpen = true;
            can.setMode(MCP_NORMAL);
            Serial.print("\r");
        } else if (c == 'C') {
            canOpen = false;
            Serial.print("\r");
        } else if (c == 'S') {
            Serial.print("\r");
        } else if (c == 'F') {
            Serial.print("F\r");
        }
        return;
    }
    
    if (cmd == 't') {
        if (idx < 3) {
            canId = (canId << 4) | hexToNibble(c);
            idx++;
        } else if (idx == 3) {
            dlc = c - '0';
            if (dlc > 8) dlc = 8;
            idx++;
            if (dlc == 0) {
                readingData = false;
                can.sendMsgBuf(canId, 0, 0, data);
            } else {
                readingData = true;
                dataIdx = 0;
            }
        } else if (readingData) {
            if (idx % 2 == 0) {
                data[dataIdx] = hexToNibble(c) << 4;
            } else {
                data[dataIdx] |= hexToNibble(c);
                dataIdx++;
            }
            idx++;
        }
    }
}

uint8_t hexToNibble(uint8_t c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return 0;
}

void sendCANtoSerial() {
    uint32_t canId;
    uint8_t dlc;
    uint8_t data[8];
    
    if (can.readMsgBuf(&canId, &dlc, data) == CAN_OK) {
        Serial.print('t');
        Serial.print(canId, HEX);
        Serial.print(dlc, HEX);
        
        for (uint8_t i = 0; i < dlc; i++) {
            if (data[i] < 16) Serial.print('0');
            Serial.print(data[i], HEX);
        }
        
        Serial.print("\r");
    }
}
