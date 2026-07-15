#pragma once

/*
    Enable to use ArduinoJson for parsing the device configuration.
    Device configuration messages are only used by the K1Pro keyboard when in normal keyboard mode.
    These JSON messages contain the style and scene information displayed on the keyboard.
    Requires the ArduinoJson library.
*/
//#define WITH_ARDUINOJSON

/*
    Enable to use AnimatedGIF library for decoding GIF images.
    This is used to display animated GIFs on the keyboard.
    If not enabled, the keyboard will not be able to display animated GIFs.
    Requires the AnimatedGIF and JPEGENC libraries.
*/
//#define WITH_ANIMATEDGIF

#include "DeviceManager.h"