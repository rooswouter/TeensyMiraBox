#include <USBHost_t36.h>
#include "DeviceManager.h"

USBHost myusb;
DeviceManager dm(myusb);

StreamDock *_device = nullptr;


#define NR_SLOTS 3
#define IMAGES_PER_SLOT 6
#define BUTTONS_PER_SLOT 2
#define SLOT_SPEED 10.f


float slot_index[NR_SLOTS] = {0.0f, 0.0f, 0.0f};
float slot_speed[NR_SLOTS] = {SLOT_SPEED, SLOT_SPEED, SLOT_SPEED};
long last_update_time = -1;

const char *images[] = { "key1.jpg", "key2.jpg", "key3.jpg", "key4.jpg", "key5.jpg", "key6.jpg"};
void onAdded(StreamDock* device) {
  Serial.println("Device added");
  printf("ID: %s\n", device->id().c_str());
  printf("Serial: %s\n", device->get_serial_number().c_str());
  printf("Path: %s\n", device->getPath().c_str());
  
  printf("device->get_device_type() = %d\n", (int)device->get_device_type());
  if(device->get_device_type() == device_type::k1pro) {
    Serial.println("K1Pro setting to SDK Mode");
    K1Pro *k1Pro = (K1Pro*)(device);
    k1Pro->keyboard_mode(1);
  }

  device->clearAllIcon();
  // image_keys() can exceed the images[] array size (293S has 18 keys),
  // so clamp the loop to avoid reading past the end of the array.
  const int image_count = (int)(sizeof(images) / sizeof(images[0]));
  const int keys_to_set = min(device->image_keys(), image_count);
  for (int i = 0; i < keys_to_set; i++) {
    device->set_key_image(i+1, images[i]);
  }
  device->set_key_callback(key_callback);
  device->refresh();
  _device = device;
}

void onRemoved(StreamDock* d) {
  Serial.println("Device removed");
}

void key_callback(StreamDock *device, const InputEvent &event)
{
  Serial.println("key_callback");
  switch (event.event_type) {
    case EventType::KNOB_PRESS:
        slot_speed[(int)event.knob_id] = SLOT_SPEED - 0.1f;
        break;
  }
}

void setup() {
  Serial.begin(115200);
  dm.setDeviceChangeCallback(onAdded, onRemoved);
  dm.begin(true, true);
  myusb.begin();  // required
}
void loop() {
  dm.poll();      // calls host_.Task() internally
  updateSlots();

}


void startSlotMachine() {
    for (int i = 0; i < NR_SLOTS; i++) {
        slot_speed[i] = true;
        slot_index[i] = 0.1f;
    }
    last_update_time = millis();
}

void updateSlots() {
    if(_device == nullptr) {
        return;
    }
    long current_time = millis();
    long delta_time = current_time - last_update_time;
    if (delta_time < 10) {
        return;
    }
    last_update_time = current_time;
    bool changed = false;
    for (int i = 0; i < NR_SLOTS; i++) {
        int current_index = (int)slot_index[i];
        if(slot_speed[i] < SLOT_SPEED && slot_speed[i] > 0.0f) {
            slot_speed[i] -= 0.01f;
            if(slot_speed[i] < 0.0f) {
                slot_speed[i] = 0.0f;
            }
        }
        slot_index[i] += slot_speed[i] * delta_time / 1000.0f;
        printf("updateSlots() slot %d: %f\n", i, slot_index[i]);
        if (slot_index[i] >= IMAGES_PER_SLOT) {
            slot_index[i] = 0.0f;
        }
        int new_index = (int)slot_index[i];
        if (new_index != current_index) {
            
            _device->set_key_image(i + 1, images[new_index]);
            _device->set_key_image(i + NR_SLOTS + 1, images[new_index + 1 < IMAGES_PER_SLOT ? new_index + 1 : 0]);
            changed = true;
        }

    }
    if (changed) {
        _device->refresh();
    }
}

void stopSlot(int index) {
    slot_speed[index] = SLOT_SPEED - 0.01f;
}