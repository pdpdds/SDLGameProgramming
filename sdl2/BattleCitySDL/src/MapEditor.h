#ifndef EDITOR_H
#define EDITOR_H

#include "Types.h"

class Editor {
public:
    Editor() : selectedField(BT_NONE) { }
    void LeftButtonDownHandle(int x, int y);
    void RightButtonDownHandle(int x, int y);
    void DrawEditor();

private:
    BrickType selectedField;
};


#endif // EDITOR_H
