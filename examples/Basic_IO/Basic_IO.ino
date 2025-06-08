/*
 This exapmle need  Arduino MIDI Library and Arduino USB-MIDI Transport(multi-cable-support branch)
 https://github.com/FortySevenEffects/arduino_midi_library
 https://github.com/aselectroworks/Arduino-USBMIDI/tree/multi-cable-support
*/

#if ARDUINO_USB_MODE
#warning This sketch should be used when USB is in OTG mode
void setup() {}
void loop() {}
#else

#include <USB-MIDI.h>

USBMIDI_CREATE_INSTANCE(0, MIDI1)
#if USB_MIDI_NUM_CABLES >= 2
USBMIDI_CREATE_INSTANCE(1, MIDI2)
#endif
#if USB_MIDI_NUM_CABLES >= 3
USBMIDI_CREATE_INSTANCE(2, MIDI3)
#endif

void setup()
{
    HWSerial.begin(115200);
    // pinMode(LED_BUILTIN, OUTPUT);
    MIDI1.begin(MIDI_CHANNEL_OMNI);  // Launch MIDI and listen to all channel
#if USB_MIDI_NUM_CABLES >= 2
    MIDI2.begin(MIDI_CHANNEL_OMNI);  // Launch MIDI and listen to all channel
#endif
#if USB_MIDI_NUM_CABLES >= 3
    MIDI3.begin(MIDI_CHANNEL_OMNI);  // Launch MIDI and listen to all channel
#endif
    HWSerial.printf("Setup Finished\r\n");

    USB.onEvent(usbEventCallback);
    MidiUSB.onEvent(usbEventCallback);

    MidiUSB.begin();
    USB.begin();
}

void loop()
{
    // HWSerial.printf("MIDI Read......\r\n");
    if (MIDI1.read())  // If we have received a message
    {
        // digitalWrite(LED_BUILTIN, HIGH);
        HWSerial.printf("MIDI1 NoteOn\r\n");
        MIDI1.sendNoteOn(42, 127, 1);  // Send a Note (pitch 42, velo 127 on channel 1)
        delay(100);                    // Wait for a second
        HWSerial.printf("MIDI1 NoteOff\r\n");
        MIDI1.sendNoteOff(42, 0, 1);  // Stop the note
        // digitalWrite(LED_BUILTIN, LOW);
    }
    // delay(100);
#if USB_MIDI_NUM_CABLES >= 2
    if (MIDI2.read())  // If we have received a message
    {
        // digitalWrite(LED_BUILTIN, HIGH);
        HWSerial.printf("MIDI2 NoteOn\r\n");
        MIDI2.sendNoteOn(44, 63, 1);  // Send a Note (pitch 42, velo 127 on channel 1)
        delay(100);                   // Wait for a second
        HWSerial.printf("MIDI2 NoteOff\r\n");
        MIDI2.sendNoteOff(44, 63, 1);  // Stop the note
        // digitalWrite(LED_BUILTIN, LOW);
    }
    // delay(100);
#endif
#if USB_MIDI_NUM_CABLES >= 3
    if (MIDI3.read())  // If we have received a message
    {
        // digitalWrite(LED_BUILTIN, HIGH);
        HWSerial.printf("MIDI3 NoteOn\r\n");
        MIDI3.sendNoteOn(42, 127, 1);  // Send a Note (pitch 42, velo 127 on channel 1)
        delay(100);                    // Wait for a second
        HWSerial.printf("MIDI3 NoteOff\r\n");
        MIDI3.sendNoteOff(42, 0, 1);  // Stop the note
        // digitalWrite(LED_BUILTIN, LOW);
    }
    // delay(100);
#endif
}

#endif