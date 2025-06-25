#include "floor.h"
#include "button.h"
#include "person.h"

void Floor::spawnPerson(Floor *destination) {
    if (*this != *destination) {
        int x_pos = (floorNumber % 2 == 0) ? x + length - (queue.size()+1)*height/4 : x + height/10 + queue.size()*height/4; // X Position based on floor number
        bool goingUp = (destination->getFloorNumber() > floorNumber); // Determine if the person is going up or down
        queue.emplace_back(hwnd, destination, goingUp, x_pos, y-height/2, height/4, height/2); // Create and add the person to the queue
        RECT invalidate;
        invalidate.left = x;
        invalidate.top = y-height;
        invalidate.right = x + length;
        invalidate.bottom = y;
        InvalidateRect(hwnd, &invalidate, TRUE); // Update the entire window
    }
}

void Floor::draw(Graphics& graphics) const {
    Pen pen(Color(255, 0, 0, 0)); // Black color for the floor line
    graphics.DrawLine(&pen, x, y, x+length, y);
    for (const auto& person : queue) {
        person.draw(graphics); // Draw each person in the queue
    }
    for (const auto& person : leaving) {
        person.draw(graphics); // Draw each person leaving the floor
    }
}