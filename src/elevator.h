#pragma once
#include <vector>
#include <queue>
#include <atomic>
#include <thread>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;

#include "person.h"
#include "button.h"

class Floor; // Forward declaration
class Button; // Forward declaration
class Elevator {
public:
    Elevator(HWND hwnd, int numFloors);
    ~Elevator();

    const std::deque<Person>& getPassengers() const { return passengers; }
    const Floor *getCurrentFloor() const { return currentFloor; }
    const std::vector<Floor>& getFloors() const { return floors; }
    const std::deque<Floor*>& getFloorQueue() const { return floorQueue; } 
    std::vector<Button>& getButtons() { return buttons; }

    void addButton(const Button& button) { buttons.push_back(button); } // Add a button to the floor

    void queueFloor(Floor *floor) {
        floorQueue.push_back(floor); // Add a floor to the queue
    }

    void unqueueFloor(Floor *floor) {
        auto it = std::find(floorQueue.begin(), floorQueue.end(), floor);
        if (it != floorQueue.end()) // erase end is undefined (like it never should return end but better safe than sorry)
            floorQueue.erase(it);
    }

    void move(int offset);

    void moveToFloor(const Floor *destination, int duration);
    void grabPassengers(); // Grab passengers from the current floor's queue
    void dropPassengers(); // Drop passengers at the current floor
    void awaitInput();
    void resetWait() { wait = false; }
    void killThread();

    int getTotalWeight() const {
        int total = 0;
        for (const auto& p : passengers) total += 70;
        return total;
    }

    void startDraw();

    void draw() const;
private:
    int x, y, height, width; // Position and size of the elevator
    Floor *currentFloor;
    Floor *destinationFloor; // Destination floor for the elevator
    bool goingUp;
    std::deque<Person> passengers;
    std::deque<Floor*> floorQueue; // Queue of floors to move elevator to
    std::thread thread;
    std::atomic<bool> wait;
    std::vector<Floor> floors; // Floors in the building
    std::vector<Button> buttons; // Ppl spawning buttons for the floor
    HWND hwnd; // Handle to the window for drawing
};
