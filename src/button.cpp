#include "button.h"

#include "elevator.h"

void Button::draw(Graphics& graphics) const {
    Color bgColor = pressed ? Color(255, 180, 180, 180) : (hovered ? Color(255, 220, 220, 220) : Color(255, 200, 200, 200));
    SolidBrush brush(bgColor);
    Pen borderPen(Color(255, 0, 0, 0));
    graphics.FillRectangle(&brush, x, y, width, height);
    graphics.DrawRectangle(&borderPen, x, y, width, height);

    FontFamily fontFamily(L"Arial");
    Font font(&fontFamily, height, FontStyleRegular, UnitPixel);
    SolidBrush textBrush(Color(255, 0, 0, 0));
    RectF layoutRect((REAL)x, (REAL)y, (REAL)width, (REAL)height);
    StringFormat format;
    format.SetAlignment(StringAlignmentCenter);
    format.SetLineAlignment(StringAlignmentCenter);
    graphics.DrawString(label.c_str(), -1, &font, layoutRect, &format, &textBrush);
}

void Button::handleMouse(UINT msg, int mx, int my) {
    bool inside = (mx >= x && mx <= x + width && my >= y && my <= y + height);
    if (msg == WM_MOUSEMOVE) {
        hovered = inside;
    } else if (msg == WM_LBUTTONDOWN) {
        pressed = inside;
    } else if (msg == WM_LBUTTONUP) {
        if (pressed && inside) floor->spawnPerson(destination); // Call the onClick function if pressed and inside
        pressed = false;
    }
}