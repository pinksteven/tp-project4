#include <thread>
#include <cmath>
#include "person.h"
#include "floor.h"

void Person::move(short x_offset, short y_offset) {
    int oldX=x, oldY=y;
    x += x_offset; // Move the person in x direction
    y += y_offset; // Move the person in y direction
    RECT invalidate;
    invalidate.left = min(oldX, x);
    invalidate.top = min(oldY, y);
    invalidate.right = max(oldX, x);
    invalidate.bottom = max(oldY, y);
    InvalidateRect(hwnd, &invalidate, TRUE);
}

void Person::animate(short x_offset, short y_offset, int duration) {
    //if(thread.joinable()) thread.join();
    // Move the person by the specified offsets over the specified duration
    /* thread = std::thread([this, x_offset, y_offset, duration]() mutable {
        int max_offset = (std::abs(x_offset) > std::abs(y_offset)) ? abs(x_offset) : abs(y_offset); // Calculate the total offset
        int stepDuration = duration / max_offset; // Duration of each step
        int x_direction = (x_offset > 0) ? 1 : (x_offset < 0) ? -1 : 0; // Determine the direction of movement in x axis
        int y_direction = (y_offset > 0) ? 1 : (y_offset < 0) ? -1 : 0; // Determine the direction of movement in y axis
        for (int i = 0; i < max_offset; ++i) {
            int x_move = (x_offset!=0) ? x_direction : 0;
            int y_move = (y_offset!=0) ? y_direction : 0;
            move(x_move, y_move); // Move in x and y directions
            x_offset -= x_move; // Decrease the offset in x direction
            y_offset -= y_move; // Decrease the offset in y direction
            Sleep(stepDuration); // Wait for the duration of each step
        }
    }); */
}

/* void Person::leave() {
    Person leavingDude = *this; // Create a copy of the person leaving
    // Move the person to the destination floor and remove them from the queue
    int movement = (destination->getFloorNumber() % 2==0) ? -destination->getLength() : destination->getLength();
    leavingDude.animate(movement, 0, 2000); // Animate the person leaving the floor
    if(thread.joinable()) thread.join();
    thread = std::thread([this]() {
        Sleep(2000); // Wait for the animation to finish// Remove the person from the queue
        auto& leavingVec = destination->getLeaving();
        leavingVec.erase(std::find(leavingVec.begin(), leavingVec.end(), *this)); // Remove the person from the leaving list of the destination floor
    });
} */

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