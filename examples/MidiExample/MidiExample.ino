#include <USBHost_t36.h>
#include <MiraBox.h>

USBHost myusb;
DeviceManager dm(myusb);

struct NoteData {
    int note = 60;
    int velocity = 99;
    int channel = 1;
};
NoteData notes[16];
int noteIndex = 0;

void onAdded(StreamDock *device) {
  Serial.println("Device added");
  device->init();
  device->set_key_callback(key_callback);
}

void key_callback(StreamDock *device, const InputEvent &event)
{
  switch(event.event_type) {
    case EventType::BUTTON:
        noteIndex = (int)event.key;
        if(event.state == 1) {
            usbMIDI.sendNoteOn(notes[noteIndex].note, notes[noteIndex].velocity, notes[noteIndex].channel);
            printf("NoteOn: %i, %i, %i\n", notes[noteIndex].note, notes[noteIndex].velocity, notes[noteIndex].channel);
        } else {
            usbMIDI.sendNoteOff(notes[noteIndex].note, notes[noteIndex].velocity, notes[noteIndex].channel);
            printf("NoteOff: %i, %i, %i\n", notes[noteIndex].note, notes[noteIndex].velocity, notes[noteIndex].channel);
        }
      break;
    case EventType::KNOB_ROTATE:
        if((int)event.knob_id == 0) {
            notes[noteIndex].note = std::clamp(notes[noteIndex].note + ((int)event.direction == 0 ? -1 : +1), 0, 128);
        }
        break;
    default:
      break;
  }
  usbMIDI.send_now();
}


void setup()
{
    Serial.begin(115200);
    while (!Serial && millis() < 3000) {}
    Serial.println("Mirabox GifExample Started, waiting for device...");
    dm.setDeviceChangeCallback(onAdded, nullptr);
    dm.begin(true, true);
    myusb.begin();

}

void loop()
{
    dm.poll();
    while (usbMIDI.read()) {
        Serial.println("MIDI data");
    }
}
