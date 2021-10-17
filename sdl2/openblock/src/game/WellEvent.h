#pragma once

#include <stdint.h>

enum class LineClearType : uint8_t {
    NORMAL,
    TSPIN,
    MINI_TSPIN,
};

struct WellEvent {
    enum class Type: uint8_t {
        PIECE_LOCKED,
        PIECE_ROTATED,
        PIECE_MOVED_HORIZONTALLY,
        NEXT_REQUESTED,
        HOLD_REQUESTED,
        LINE_CLEAR_ANIMATION_START,
        LINE_CLEAR,
        TSPIN_DETECTED,
        MINI_TSPIN_DETECTED,
        SOFTDROPPED,
        HARDDROPPED,
        GAME_OVER,
    };

    struct harddrop_t {
        uint8_t count;
    };

    struct lineclear_t {
        uint8_t count;
        LineClearType type;
    };

    Type type;
    union {
        harddrop_t harddrop;
        lineclear_t lineclear;
    };

    explicit WellEvent(Type type): type(type) {}
};
