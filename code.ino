#include <Arduino.h>
#include <SoftwareSerial.h>

// Calculate CRC
uint16_t calculate_crc(const uint8_t *data, size_t length) {
    uint16_t crc = 0xFFFF;
    for (size_t pos = 0; pos < length; pos++) {
        crc ^= data[pos];
        for (int i = 0; i < 8; i++) {
            if ((crc & 1) != 0) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

// Parse Modbus RTU
void parse_modbus_rtu(const uint8_t *data, size_t length) {
    if (length < 4) {
        Serial.println("Incomplete Modbus RTU frame");
        return;
    }

    uint8_t device_id = data[0];
    uint8_t function_code = data[1];
    uint16_t crc_received = data[length - 2] | (data[length - 1] << 8);
    uint16_t crc_calculated = calculate_crc(data, length - 2);

    if (crc_received != crc_calculated) {
        Serial.println("CRC check failed");
        return;
    }

    Serial.print("Device_id: ");
    Serial.println(device_id);
    Serial.print("Function Code: ");
    Serial.println(function_code);

    if (function_code == 3) {  // Read Holding Registers
        uint8_t byte_count = data[2];
        Serial.print("Read Holding Registers: ");
        for (uint8_t i = 0; i < byte_count; i++) {
            Serial.print(data[3 + i], HEX);
            Serial.print(" ");
        }
        Serial.println();
    }
}

// Read serial data
void read_serial_data() {
    SoftwareSerial mySerial(16, 17); // RX, TX
    mySerial.begin(19200);

    Serial.begin(9600);
    Serial.println("Listening for Modbus RTU packets...");

    uint8_t buffer[256];
    size_t buffer_index = 0;

    while (true) {
        if (mySerial.available() > 0) {
            int byte_read = mySerial.read();
            if (byte_read != -1) {
                buffer[buffer_index++] = (uint8_t)byte_read;
                if (buffer_index >= 4) {
                    parse_modbus_rtu(buffer, buffer_index);
                    buffer_index = 0;
                }
            }
        }
    }
}

void setup() {
    

}

void loop() {
  // read_serial_data continuous reading
    read_serial_data();
}
