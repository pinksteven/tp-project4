#include "button.h"
#include "elevator.h"
#include "floor.h"
#include "person.h"

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

std::tuple<Floor*, Floor*> Button::handleMouse(UINT msg, int mx, int my) {
    bool inside = (mx >= x && mx <= x + width && my >= y && my <= y + height);
    if (msg == WM_MOUSEMOVE) {
        hovered = inside;
        return std::make_tuple(floor, floor);
    } else if (msg == WM_LBUTTONDOWN) {
        pressed = inside;
        return std::make_tuple(floor, floor);
    } else if (msg == WM_LBUTTONUP) {
        if (pressed && inside) {pressed = false; return std::make_tuple(floor, destination);};
        pressed = false;
        return std::make_tuple(floor, floor);
    }
}