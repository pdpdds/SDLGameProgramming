#pragma once

#include "Event.h"

#include <stdint.h>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>


using DeviceName = std::string;
using EventToButtonsMap = std::unordered_map<InputType, std::vector<uint16_t>, InputTypeHash>;
using ButtonToEventsMap = std::unordered_map<uint16_t, std::vector<InputType>>;

EventToButtonsMap toEventMap(const ButtonToEventsMap&);
ButtonToEventsMap toButtonMap(const EventToButtonsMap&);

enum class DeviceType : uint8_t {
    KEYBOARD,
    GAMEPAD,
    LEGACY_JOYSTICK,
};

struct DeviceData {
    DeviceID id;
    DeviceName name;
    DeviceType type;
    ButtonToEventsMap buttonmap;
    EventToButtonsMap eventmap;
};

using DeviceMap = std::map<DeviceID, DeviceData>;
