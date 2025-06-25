#pragma once
#include <windows.h>
#include <gdiplus.h>
#include <string>
#include <tuple>

using namespace Gdiplus;
class Floor; // Forward declaration
class Elevator; // Forward declaration
class Button {
public:
    Button(int x, int y, int width, int height, 
                  const std::wstring& label, 
                  Floor* floor, Floor* destination)
        : x(x), y(y), width(width), height(height), label(label), floor(floor), destination(destination), hovered(false), pressed(false) {}

    void draw(Graphics& graphics) const;

    // Call this from WM_LBUTTONDOWN/UP/MOUSEMOVE
    std::tuple<Floor*, Floor*> handleMouse(UINT msg, int mx, int my);

    // For hit testing (optional)
    bool contains(int mx, int my) {
        return mx >= x && mx <= x + width && my >= y && my <= y + height;
    }

private:
    int x, y, width, height;
    std::wstring label;
    Floor* floor, *destination;
    bool hovered;
    bool pressed;
};