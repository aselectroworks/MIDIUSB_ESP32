#ifndef ARDUINO_USB_MODE
#error This ESP32 SoC has no Native USB interface
#elif ARDUINO_USB_MODE == 1
#warning This sketch should be used when USB is in OTG mode
void setup() {}
void loop() {}
#else
#include "MIDIUSB_ESP32.h"

#if !ARDUINO_USB_CDC_ON_BOOT
USBCDC USBSerial;
#endif

static const char *TAG = "example";

// override usbEventCallback()
void usbEventCallback(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == ARDUINO_USB_EVENTS) {
        arduino_usb_event_data_t *data = (arduino_usb_event_data_t *)event_data;
        switch (event_id) {
            case ARDUINO_USB_STARTED_EVENT:
                Serial.println("USB PLUGGED");
                break;
            case ARDUINO_USB_STOPPED_EVENT:
                Serial.println("USB UNPLUGGED");
                break;
            case ARDUINO_USB_SUSPEND_EVENT:
                Serial.printf("USB SUSPENDED: remote_wakeup_en: %u\n", data->suspend.remote_wakeup_en);
                break;
            case ARDUINO_USB_RESUME_EVENT:
                Serial.println("USB RESUMED");
                break;

            default:
                break;
        }
    }
    else if (event_base == ARDUINO_USB_CDC_EVENTS) {
        arduino_usb_cdc_event_data_t *data = (arduino_usb_cdc_event_data_t *)event_data;
        switch (event_id) {
            case ARDUINO_USB_CDC_CONNECTED_EVENT:
                Serial.println("CDC CONNECTED");
                break;
            case ARDUINO_USB_CDC_DISCONNECTED_EVENT:
                Serial.println("CDC DISCONNECTED");
                break;
            case ARDUINO_USB_CDC_LINE_STATE_EVENT:
                Serial.printf("CDC LINE STATE: dtr: %u, rts: %u\n", data->line_state.dtr, data->line_state.rts);
                break;
            case ARDUINO_USB_CDC_LINE_CODING_EVENT:
                Serial.printf("CDC LINE CODING: bit_rate: %lu, data_bits: %u, stop_bits: %u, parity: %u\n", data->line_coding.bit_rate,
                              data->line_coding.data_bits, data->line_coding.stop_bits, data->line_coding.parity);
                break;
            case ARDUINO_USB_CDC_RX_EVENT:
                Serial.printf("CDC RX [%u]:", data->rx.len);
                {
                    uint8_t buf[data->rx.len];
                    size_t len = USBSerial.read(buf, data->rx.len);
                    Serial.write(buf, len);
                }
                Serial.println();
                break;
            case ARDUINO_USB_CDC_RX_OVERFLOW_EVENT:
                Serial.printf("CDC RX Overflow of %d bytes", data->rx_overflow.dropped_bytes);
                break;

            default:
                break;
        }
    }
    else if (event_base == ARDUINO_USB_MIDI_EVENTS) {
        midiEventPacket_t *data = (midiEventPacket_t *)event_data;
        switch (event_id) {
            default:
                HWSerial.printf("MIDI EVENT:  ID=%d, DATA=%d\r\n", event_id, (uint32_t)data);
                break;
        }
    }
}

void setup()
{
    HWSerial.begin(115200);
    HWSerial.setDebugOutput(true);

    /* Change USB Device Descriptor Parameter
        USB.VID(0x6666);
        USB.PID(0x1234);
        USB.productName("MIDIUSB_ESP32");
        USB.manufacturerName("A.S Electroworks");
        USB.serialNumber("0000");
        USB.firmwareVersion(0x0000);
        USB.usbVersion(0x0200);
        //USB.usbClass(TUSB_CLASS_AUDIO);
        //USB.usbSubClass(0x00);
        //USB.usbProtocol(0x00);
        //USB.usbAttributes(0x80);
        USB.usbPower(100);
        tinyusb_add_string_descriptor("MIDIUSB_ESP32_STR_DESC");
    */

    USB.onEvent(usbEventCallback);
    USBSerial.onEvent(usbEventCallback);

    MidiUSB.begin();
    USBSerial.begin();

    USB.begin();
}

void noteOn(uint8_t channel, uint8_t pitch, uint8_t velocity)
{
    channel |= 0x90;
    midiEventPacket_t noteOn = {0x09, channel, pitch, velocity};
    MidiUSB.sendMIDI(noteOn);
}

void noteOff(uint8_t channel, uint8_t pitch, uint8_t velocity)
{
    channel |= 0x80;
    midiEventPacket_t noteOff = {0x08, channel, pitch, velocity};
    MidiUSB.sendMIDI(noteOff);
}

void controlChange(uint8_t channel, uint8_t control, uint8_t value)
{
    channel |= 0xB0;
    midiEventPacket_t event = {0x0B, channel, control, value};
    MidiUSB.sendMIDI(event);
}

void loop()
{
    USBSerial.printf("Loop!!!\r\n");

    midiEventPacket_t rx;
    do {
        rx = MidiUSB.read();
        if (rx.header != 0) {
            ESP_LOGI(TAG, "Midi Read Data: %02hhX %02hhX %02hhX %02hhX", rx.header, rx.byte1, rx.byte2, rx.byte3);

            // send back the received MIDI command
            MidiUSB.sendMIDI(rx);
        }
    } while (rx.header != 0);

    delay(1000);
}
#endif /* ARDUINO_USB_MODE */
