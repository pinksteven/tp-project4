#pragma once
#include <windows.h>
#include <gdiplus.h>
#include <functional>
#include <string>

using namespace Gdiplus;

class Button {
public:
    Button(int x, int y, int width, int height, 
                  const std::wstring& label, 
                  std::function<void()> onClick)
        : x(x), y(y), width(width), height(height), label(label), onClick(onClick), hovered(false), pressed(false) {}

    void draw(Graphics& graphics) {
        Color bgColor = pressed ? Color(255, 180, 180, 180) : (hovered ? Color(255, 220, 220, 220) : Color(255, 200, 200, 200));
        SolidBrush brush(bgColor);
        Pen borderPen(Color(255, 0, 0, 0));
        graphics.FillRectangle(&brush, x, y, width, height);
        graphics.DrawRectangle(&borderPen, x, y, width, height);

        FontFamily fontFamily(L"Arial");
        Font font(&fontFamily, 16, FontStyleRegular, UnitPixel);
        SolidBrush textBrush(Color(255, 0, 0, 0));
        RectF layoutRect((REAL)x, (REAL)y, (REAL)width, (REAL)height);
        StringFormat format;
        format.SetAlignment(StringAlignmentCenter);
        format.SetLineAlignment(StringAlignmentCenter);
        graphics.DrawString(label.c_str(), -1, &font, layoutRect, &format, &textBrush);
    }

    // Call this from WM_LBUTTONDOWN/UP/MOUSEMOVE
    void handleMouse(UINT msg, int mx, int my) {
        bool inside = (mx >= x && mx <= x + width && my >= y && my <= y + height);
        if (msg == WM_MOUSEMOVE) {
            hovered = inside;
        } else if (msg == WM_LBUTTONDOWN) {
            pressed = inside;
        } else if (msg == WM_LBUTTONUP) {
            if (pressed && inside && onClick) onClick();
            pressed = false;
        }
    }

    // For hit testing (optional)
    bool contains(int mx, int my) const {
        return mx >= x && mx <= x + width && my >= y && my <= y + height;
    }

private:
    int x, y, width, height;
    std::wstring label;
    std::function<void()> onClick;
    bool hovered;
    bool pressed;
};