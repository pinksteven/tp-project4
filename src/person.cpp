#include <thread>
#include <cmath>
#include "person.h"
#include "floor.h"

#define PERSON_ANIMATION_END (WM_USER+5)
#define PERSON_LEFT (WM_USER + 6)

void Person::move(short x_offset, short y_offset) {
    int oldX=x, oldY=y;
    x += x_offset; // Move the person in x direction
    y += y_offset; // Move the person in y direction
    RECT invalidate;
    invalidate.left = min(oldX, x)-3;
    invalidate.top = min(oldY, y);
    invalidate.right = max(oldX, x)+width;
    invalidate.bottom = max(oldY, y)+height+1;
    InvalidateRect(hwnd, &invalidate, TRUE);
}

void Person::animateX(short offset, int duration) {
    killThread();
    // Move the person by the specified offsets over the specified duration
    thread = std::thread([this, offset, duration]() {
        int stepDuration = duration / std::abs(offset); // Duration of each step
        int direction = (offset > 0) ? 1 : (offset < 0) ? -1 : 0; // Determine the direction of movement
        for (int i = 0; i < std::abs(offset); ++i) {
            move(direction, 0); // Move in x and y directions
            Sleep(stepDuration); // Wait for the duration of each step
        }
        PostMessage(hwnd, PERSON_ANIMATION_END, 0, reinterpret_cast<LPARAM>(this));
    });
}

void Person::leave() {
    // Move the person to the destination floor and remove them from the queue
    killThread();
    thread = std::thread([this]() {
        int direction = (destination->getFloorNumber()%2==0) ? -4 : 4; // Determine the direction of movement
        for (int i = 0; i < 60; ++i) {
            move(direction, 0); // Move in x and y directions
            Sleep(10); // Wait for the duration of each step
        }
        PostMessage(hwnd, PERSON_LEFT, 0, reinterpret_cast<LPARAM>(this));
    });
}

void Person::draw(Graphics& graphics) const {
    // Calculate stickman proportions (the numbners are arbitrary and can be adjusted, i just pikced what looked good)
    int headRadius = width / 3.5;
    int centerX = x + width / 3.5;
    int headCenterY = y + headRadius;
    int bodyTopY = y + 2 * headRadius;
    int legY = y + height - headRadius*2.2;
    int armY = bodyTopY + (legY - bodyTopY) / 4;
    int armLength = width / 2 - 1;
    int legLength = width / 3.5;

    // Black stickman
    SolidBrush brush(Color(255, 0, 0, 0));
    Pen pen(Color(255, 0, 0, 0), 2);

    graphics.FillEllipse(&brush, centerX - headRadius, y, headRadius * 2, headRadius * 2); // Head
    graphics.DrawLine(&pen, centerX, bodyTopY, centerX, legY); // Body
    graphics.DrawLine(&pen, centerX, armY, centerX - armLength, armY + armLength); // Left Arm
    graphics.DrawLine(&pen, centerX, armY, centerX + armLength, armY + armLength); // Right Arm
    graphics.DrawLine(&pen, centerX, legY, centerX - legLength, y+height); // Left leg
    graphics.DrawLine(&pen, centerX, legY, centerX + legLength, y+height); // Right leg
}