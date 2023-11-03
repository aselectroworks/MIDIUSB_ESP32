#if ARDUINO_USB_MODE
#warning This sketch should be used when USB is in OTG mode
void setup() {}
void loop() {}
#else

#include "MIDIUSB_ESP32.h"

static const char *TAG = "example";

void setup() {
    HWSerial.begin(115200);
    HWSerial.setDebugOutput(true);

    MidiUSB.begin();
}

void noteOn(uint8_t channel, uint8_t pitch, uint8_t velocity) {
    channel |= 0x90;
    midiEventPacket_t noteOn = {0x09, channel, pitch, velocity};
    MidiUSB.sendMIDI(noteOn);
}

void noteOff(uint8_t channel, uint8_t pitch, uint8_t velocity) {
    channel |= 0x80;
    midiEventPacket_t noteOff = {0x08, channel, pitch, velocity};
    MidiUSB.sendMIDI(noteOff);
}

void controlChange(uint8_t channel, uint8_t control, uint8_t value) {
    channel |= 0xB0;
    midiEventPacket_t event = {0x0B, channel, control, value};
    MidiUSB.sendMIDI(event);
}

void loop() {
    midiEventPacket_t rx;
    do {
        rx = MidiUSB.read();
        if (rx.header != 0) {
            ESP_LOGI(TAG, "Midi Read Data: %02hhX %02hhX %02hhX %02hhX",
                        rx.header, rx.byte1, rx.byte2, rx.byte3);

            // send back the received MIDI command
            MidiUSB.sendMIDI(rx);
        }
    } while (rx.header != 0);
}
#endif /* ARDUINO_USB_MODE */
