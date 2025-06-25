#pragma once
#include <thread>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;

class Floor; // Forward declaration

class Person {
public:
    Person(HWND hwnd, Floor *destination, bool goingUp, int x, int y, int width, int height)
        : hwnd(hwnd),  destination(destination), goingUp(goingUp), x(x), y(y), width(width), height(height) {}
    ~Person() { killThread(); }

    Person(const Person&) = delete;
    Person& operator=(const Person&) = delete;
    Person(Person&&) noexcept = default;
    Person& operator=(Person&&) noexcept = default;

    bool operator==(const Person& other) const { return &other != nullptr && this==&other; } // Compare based on pointer address
    bool operator!=(const Person& other) const { return &other == nullptr || this!=&other; } //

    Floor* getDestination() { return destination; }
    int getX() const { return x; }
    int getY() const { return y; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    bool isGoingUp() const { return goingUp; }

    void move(short x_offset, short y_offset);
    void animateX(short x_offset, int duration);
    void leave();
    void killThread() { if(thread.joinable()) thread.join(); }
    void draw(Graphics& graphics) const;
private:
    int x, y, width, height;
    bool goingUp;
    std::thread thread;
    Floor *destination;
    HWND hwnd; // Handle to the window for drawing
};