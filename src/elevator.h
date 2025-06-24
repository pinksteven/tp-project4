#pragma once
#include <vector>
#include <queue>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;

class Floor; // Forward declaration
class Button; // Forward declaration
class Person {
public:
    Person(HWND hwnd, Floor *destination, int x, int y, int width, int height)
        : hwnd(hwnd),  destination(destination), x(x), y(y), width(width), height(height) {}

    Floor* getDestination() { return destination; }

    void move(int x_offset, int y_offset);
    void animate(int x_offset, int y_offset, int duration);
    void draw(Graphics& graphics) const;
private:
    int x, y, width, height;
    Floor *destination;
    HWND hwnd; // Handle to the window for drawing
};

class Floor {
public:
    Floor(HWND hwnd, int floorNumber, int x, int y, int length, int height) 
        : hwnd(hwnd), floorNumber(floorNumber), x(x), y(y), length(length), height(height) {}

    bool operator==(const Floor& other) const {
        return floorNumber == other.floorNumber; // Compare based on floor number
    }
    bool operator!=(const Floor& other) const {
        return floorNumber != other.floorNumber; // Compare based on floor number
    }

    int getFloorNumber() const { return floorNumber; }
    std::deque<Person>& getQueue() { return queue; }
    std::vector<Person>& getLeaving() { return leaving; }
    int getX() const { return x; }
    int getY() const { return y; }
    std::vector<Button>& getButtons();

    void addButton(const Button& button) { buttons.push_back(button); } // Add a button to the floor

    void spawnPerson(Floor *destination);

    void draw(Graphics& graphics) const;
private:
    int x, y, length, height; // Position and size of the floor
    int floorNumber;
    std::deque<Person> queue; // Queue of people waiting on this floor
    std::vector<Person> leaving; // People leaving the floor
    std::vector<Button> buttons; // Pppl spawning buttons for the floor
    HWND hwnd; // Handle to the window for drawing
};

class Elevator {
public:
    Elevator(HWND hwnd)
        : hwnd(hwnd) {}

    const std::vector<Person>& getPassengers() const { return passengers; }
    const Floor *getCurrentFloor() const { return currentFloor; }

    void setValues(Floor *currentFloor, int x, int y, int width, int height);

    void move(int offset);

    void moveToFloor(Floor *destination, int duration);
    void grabPassengers(std::queue<Person>& floor);

    void draw(Graphics& graphics) const;
private:
    int x, y, height, width; // Position and size of the elevator
    Floor *currentFloor;
    std::vector<Person> passengers;
    HWND hwnd; // Handle to the window for drawing
};

class Building {
public:
    Building(HWND hwnd, int numFloors);

    // One big draw function bc i don't understand how to do this in separate functions (it was very wierd when it was here)
    void draw();

    Elevator& getElevator() { return elevator; }
    std::vector<Floor>& getFloors() { return floors; }

private:
    HWND hwnd; // Handle to the window for drawing
    Elevator elevator; // Elevator in the building
    std::vector<Floor> floors; // Floors in the building
};
