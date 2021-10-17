#pragma once

#include "system/Texture.h"

#include <memory>
#include <stdint.h>


/// A Mino represents one block of a piece.
class Mino {
public:
    /// Create a mino with the texture and Ascii value
    Mino(std::unique_ptr<Texture> texture, char ascii_val);

    /// Draw the Mino at the provided coordinates
    void draw(int x, int y);
    /// Draw part of the Mino texture to the provided area
    void drawPartial(const Rectangle& from, const Rectangle& to);
    /// The Ascii value of the Mino, used mainly for debugging
    char asAscii() const { return ascii_val; }

    // TODO: load from config maybe
    static constexpr int8_t texture_size_px = 32;

private:
    const std::unique_ptr<Texture> texture;
    const char ascii_val;
};
