#pragma once

#include "game/Timing.h"


class Well;


namespace WellComponents {

class Gravity {
public:
    Gravity(Duration duration = Duration::max());

    void setRate(Duration);
    /// Updates the gravity timer, and calls applyGravity() if needed
    void update(Well&);
    /// Do not apply gravity during the next update() call
    void skipNextUpdate();

    Duration currentDelay() { return gravity_delay; }

private:
    Duration gravity_delay;
    Duration gravity_timer;
    bool skip_gravity;

    /// Asks the well to move the active piece one row down
    void applyGravity(Well&);
};

} // namespace WellComponents
