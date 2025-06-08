#pragma once

#include "soc/soc_caps.h"
#if SOC_USB_OTG_SUPPORTED

#include "sdkconfig.h"
#if CONFIG_TINYUSB_MIDI_ENABLED

#include <stdint.h>
#include <inttypes.h>
#include "esp_event.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "Stream.h"

#include "USB.h"
#include "esp32-hal-tinyusb.h"

#include "MIDIUSB_Defs.h"

#include "HardwareSerial.h"
#include "HWCDC.h"

#if ARDUINO_USB_CDC_ON_BOOT
#define HWSerial Serial0
#define USBSerial Serial
#else
#define HWSerial Serial
#endif

ESP_EVENT_DECLARE_BASE(ARDUINO_USB_MIDI_EVENTS);
#include "midi_cable.h"
#if (USB_MIDI_NUM_CABLES <= 0) || (USB_MIDI_NUM_CABLES > 3)
#error "USB_MIDI_NUM_CABLES must be 1, 2 or 3"
#endif

__attribute__((weak)) void usbEventCallback(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

typedef enum {
    ARDUINO_USB_MIDI_ANY_EVENT = ESP_EVENT_ANY_ID,
    ARDUINO_USB_MIDI_RX_EVENT = 0,
    ARDUINO_USB_MIDI_EVENT_MAX
} arduino_usb_midi_event_t;

class MIDIUSB {
   public:
    MIDIUSB();
    ~MIDIUSB();

    void onEvent(esp_event_handler_t callback);
    void onEvent(arduino_usb_midi_event_t event, esp_event_handler_t callback);

    // MIDIUSB method
    void begin();
    midiEventPacket_t read();
    void flush(void);
    void sendMIDI(midiEventPacket_t event);
};

extern MIDIUSB MidiUSB;

#endif /* CONFIG_TINYUSB_MIDI_ENABLED */
#endif /* SOC_USB_OTG_SUPPORTED */
