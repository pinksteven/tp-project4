#pragma once
#include <vector>
#include <queue>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;

#include "person.h"

class Floor; // Forward declaration
class Elevator {
public:
    Elevator(HWND hwnd, int numFloors);

    const std::deque<Person>& getPassengers() const { return passengers; }
    const Floor *getCurrentFloor() const { return currentFloor; }
    std::vector<Floor>& getFloors() { return floors; }

    void queueFloor(Floor *floor) {
        floorQueue.push_back(floor); // Add a floor to the queue
    }

    void move(int offset);

    void moveToFloor(Floor *destination, int duration);
    void grabPassengers(); // Grab passengers from the current floor's queue
    void dropPassengers(); // Drop passengers at the current floor
    void awaitInput();

    void draw() const;
private:
    int x, y, height, width; // Position and size of the elevator
    Floor *currentFloor;
    Floor *destinationFloor; // Destination floor for the elevator
    bool goingUp;
    std::deque<Person> passengers;
    std::deque<Floor*> floorQueue; // Queue of floors to move elevator to
    std::vector<Floor> floors; // Floors in the building
    HWND hwnd; // Handle to the window for drawing
};
