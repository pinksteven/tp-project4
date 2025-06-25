#pragma once
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <vector>
using namespace Gdiplus;

class Person; // Forward declaration

class Floor {
public:
    Floor(HWND hwnd, int floorNumber, int x, int y, int length, int height) 
        : hwnd(hwnd), floorNumber(floorNumber), x(x), y(y), length(length), height(height) {}

    bool operator==(const Floor& other) const {
        return this==&other; // Compare based on pointer
    }
    bool operator!=(const Floor& other) const {
        return this!=&other; // Compare based on floor number
    }

    int getFloorNumber() const { return floorNumber; }
    std::vector<Person>& getQueue() { return queue; }
    std::vector<Person>& getLeaving() { return leaving; }
    int getX() const { return x; }
    int getY() const { return y; }
    int getHeight() const { return height; }
    int getLength() const { return length; }

    void spawnPerson(Floor *destination);

    void draw(Graphics& graphics) const;
private:
    int x, y, length, height; // Position and size of the floor
    int floorNumber;
    std::vector<Person> queue; // Queue of people waiting on this floor
    std::vector<Person> leaving; // People leaving the floor
    HWND hwnd; // Handle to the window for drawing
};