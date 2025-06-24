#include "elevator.h"
#include <thread>
#include <string>
#include <cmath>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;

#include "button.h"

void Person::move(int x_offset, int y_offset) {
    x += x_offset; // Move the person in x direction
    y += y_offset; // Move the person in y direction
    RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW); // Update the entire window
}

void Person::animate(int x_offset, int y_offset, int duration) {
    // Move the person by the specified offsets over the specified duration
    std::thread t([this, x_offset, y_offset, duration]() mutable {
        int max_offset = (std::abs(x_offset) > std::abs(y_offset)) ? abs(x_offset) : abs(y_offset); // Calculate the total offset
        int stepDuration = duration / max_offset; // Duration of each step
        int x_direction = (x_offset > 0) ? 1 : (x_offset < 0) ? -1 : 0; // Determine the direction of movement in x axis
        int y_direction = (y_offset > 0) ? 1 : (y_offset < 0) ? -1 : 0; // Determine the direction of movement in y axis
        for (int i = 0; i < max_offset; ++i) {
            if (x_offset != 0 || y_offset != 0) {
                move(x_direction, y_direction); // Move in x and y directions
                x_offset -= x_direction; // Decrease the offset in x direction
                y_offset -= y_direction; // Decrease the offset in y direction
            }
            Sleep(stepDuration); // Wait for the duration of each step
        }
    });
    t.detach(); // Detach the thread to allow it to run independently
}

void Person::draw(Graphics& graphics) const {
    SolidBrush brush(Color(255, 0, 0, 255)); // Blue color for the person
    graphics.FillRectangle(&brush, x, y, width, height);
}

std::vector<Button>& Floor::getButtons() {
    return buttons; // Return the vector of buttons on the floor
}

void Floor::spawnPerson(Floor *destination) {
    if (*this != *destination) {
        int x_pos = (floorNumber % 2 == 0) ? x + length - (queue.size()+1)*height/4 : x - queue.size()*height/4; // X Position based on floor number
        Person person(hwnd, destination, x_pos, y-height/2, height/4, height/2); // Create a new person
        queue.push_back(person); // Add the person to the queue
        RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW); // Update the entire window
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
    for (const auto& button : buttons) {
        button.draw(graphics); // Draw each button on the floor
    }
}

void Elevator::setValues(Floor *currentFloor, int x, int y, int width, int height) {
        this->currentFloor = currentFloor;
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
    }

void Elevator::move(int offset) {
        y+=offset; // Move the elevator up
        for(auto& passenger : passengers) 
            passenger.move(0, offset); // Move each passenger in the elevator
        RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW); // Update the entire window
    }

void Elevator::moveToFloor(Floor *destination, int duration) {
        if (*currentFloor != *destination) {
            int offset = destination->getY() - currentFloor->getY(); // Calculate the offset to move to the destination floor
            // Move the elevator up or down by the specified offset over the specified duration
            std::thread t([this, destination, offset, duration]() {
                int stepDuration = duration / std::abs(offset); // Duration proportional to the distance
                int direction = (offset > 0) ? 1 : -1; // Determine the direction of movement
                for (int i = 0; i < std::abs(offset); ++i) {
                    move(direction);
                    Sleep(stepDuration); // Wait for the duration of each step (1 ms per step)
                }
                    currentFloor = destination; // Update the current floor after moving
            });
            t.detach(); // Detach the thread to allow it to run independently
        }
    }

void Elevator::grabPassengers(std::queue<Person>& floor) {
        double totalWeight = 0.0;
        // if 1 more passenger will still fit in limit then grab
        while(!floor.empty() && (passengers.size() + 1) * 70 <= 600){
            passengers.push_back(floor.front());
            floor.pop();
        };
    }

void Elevator::draw(Graphics& graphics) const {
    Pen pen(Color(255, 255, 0, 0)); // Red color for the elevator
    graphics.DrawRectangle(&pen, Rect(x, y, width, height));
    for (const auto& passenger : passengers) {
        passenger.draw(graphics); // Draw each passenger in the elevator
    }
}

Building::Building(HWND hwnd, int numFloors)
        : hwnd(hwnd), elevator(hwnd) {
        RECT rect;
    if (GetClientRect(hwnd, &rect)) {
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;
        //height of window - 1px per floor and 3px space form bottom for clarity
        int floorHeight = (height-numFloors-3) / numFloors;
        floors.reserve(numFloors); // Reserve space for the floors to avoid multiple reallocations
        for (int i = 0; i < numFloors; ++i) {
            int floorY = height - i * floorHeight - 3; // Calculate the y position of the floor
            int floorX = (i % 2 == 0) ? 0 : (width + floorHeight * 2)/2; // X position based on floor number
            // Create a new floor and add it to the vector
            floors.emplace_back(hwnd, i, floorX, floorY, (width-floorHeight*2)/2, floorHeight);
        }
        elevator.setValues(&floors[0], (width - floorHeight*2)/2, height - floorHeight - 3, floorHeight*2, floorHeight);// Initialize elevator in the middle and last floor
        floors[0].spawnPerson(&floors[1]); // Spawning test
        for(int i = 0; i < numFloors; ++i) {
            for(int j=0; j<floors.size(); j++) {
                int btnSize = (floorHeight - numFloors*2) / (numFloors+1); // Calculate button size based on the number of floors
                int btnX = (i % 2 == 0) ? 1 : width-btnSize-1; // X position based on floor number
                int btnY = floors[i].getY() - (j+1) * (btnSize+2) - btnSize/2; // Y position based on floor number
                // Create a button for each floor
                Button button(btnX, btnY, btnSize, btnSize, std::to_wstring(j), &floors[i], &floors[j]); // Button for each floor
                if(i!=j) floors[i].addButton(button); // Add the button to the floor
            }
        }
    }
}

void Building::draw() {
    RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE); //It doesn't work without it, it seems like it should, but it doesn't
    // Draw the elevator and all floors
    HDC          hdc;
    PAINTSTRUCT  ps;
    hdc = BeginPaint(hwnd, &ps);
    Graphics graphics(hdc);
    Pen redPen(Color(255, 255, 0, 0)); // Red color for the elevator
    Pen blackPen(Color(255, 0, 0, 0)); // Black color for the floor line
    SolidBrush brush(Color(255, 0, 0, 255)); // Blue color for the person
    elevator.draw(graphics); // Draw the elevator and passengers within it
    for (auto& floor : floors) 
        floor.draw(graphics); // Draw each floor and people on it
    EndPaint(hwnd, &ps);
}