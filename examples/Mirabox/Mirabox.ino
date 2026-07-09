#include <USBHost_t36.h>
#include "DeviceManager.h"

USBHost myusb;
DeviceManager dm(myusb);

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
  for (int i = 0; i < device->image_keys(); i++) {
    device->set_key_image(i+1, images[i]);
  }
  device->set_key_callback(key_callback);
  device->refresh();

}

void onRemoved(StreamDock* d) {
  Serial.println("Device removed");
}

void key_callback(StreamDock *device, const InputEvent &event)
{
  Serial.println("key_callback");
}

void setup() {
  Serial.begin(115200);
  dm.setDeviceChangeCallback(onAdded, onRemoved);
  dm.begin(true, true);
  myusb.begin();  // required
}
void loop() {
  dm.poll();      // calls host_.Task() internally
}