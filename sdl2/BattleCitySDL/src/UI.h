#ifndef GUI_H
#define GUI_H

#include <string>
#include "Types.h"

using namespace std;

class UI {
public:
    void DrawGameplayStats();
    void RenderText(const string& text, double x, double y, ColorType color = COLOR_NONE);

private:

};

#endif // GUI_H
