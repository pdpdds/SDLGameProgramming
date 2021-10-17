#pragma once

#include "Event.h"
#include "InputMap.h"

#include <memory>
#include <string>
#include <vector>


class AudioContext;
class GraphicsContext;


/// The Window represents the game window, and it is the interface between
/// the native operating system and hardware. It provides a way to draw on
/// the screen from the game, and collects and makes the native events
/// (eg. input) available to other parts of the game.
class Window {
public:
    virtual ~Window() {};

    /// Change between windowed and fullscreen mode.
    virtual void toggleFullscreen() = 0;
    /// Save a screenshot of the game window to the provided path
    /// at the end of the current render cycle
    virtual void requestScreenshot(const std::string& path) = 0;

    /// Return the graphics context component of the window,
    /// which can be used for drawing on this window.
    virtual GraphicsContext& graphicsContext() = 0;
    /// Returns the audio context component of the window,
    /// which can be used for playing music and sound effects.
    virtual AudioContext& audioContext() = 0;

    /// In every frame, the Window should collect the native events,
    /// and return them in a platform-independent format.
    /// If the user wants to quit the game by a native event, then after this call
    /// `quit_requested()` should return true.
    virtual std::vector<Event> collectEvents() = 0;
    /// Return `true` if the user wants to quit the program, eg. by closing the game
    /// window or pressing certain key combinations (Alt-F4, Ctrl-Q, ...).
    virtual bool quitRequested() = 0;

    /// Set the known input mappings previously read from a config file.
    virtual void setInputConfig(const std::map<DeviceName, DeviceData>&) = 0;
    /// Set the key binding of an input event to a raw key on a device.
    virtual void setKeyBinding(DeviceID, InputType, uint16_t raw_key) = 0;
    /// Get all the input mappings the window has knowledge of in a
    /// format similar to config files.
    virtual std::map<DeviceName, DeviceData> createInputConfig() const = 0;
    /// Get all currently connected devices.
    virtual const DeviceMap& connectedDevices() const = 0;
    /// Get the name of a raw button on a device, or an empty string.
    virtual std::string buttonName(DeviceID, uint16_t raw_key) const = 0;

    /// If possible, show an error message box with the title "Error" and the
    /// provided content text. This operation should work even without a Window object.
    static void showErrorMessage(const std::string& content);

private:
    static std::unique_ptr<Window> create();

friend class AppContext;
};
