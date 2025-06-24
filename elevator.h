#include <vector>
#include <queue>
#include <thread>
#include <cmath>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
//#pragma comment (lib,"Gdiplus.lib")

class Floor; // Forward declaration

class Person {
public:
    Person(HWND hwnd, Floor *destination, int x, int y, int width, int height)
        : hwnd(hwnd),  destination(destination), x(x), y(y), width(width), height(height) {}

    int x;
    int y;
    int height;
    int width;

    Floor* getDestination() { return destination; }

    void move(int x_offset, int y_offset) {
        x+= x_offset; // Move the person in x direction
        y+=y_offset; // Move the person in y direction
        RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW); // Update the entire window
    }
    void animate(int x_offset, int y_offset, int duration) {
        // Move the elevator up or down by the specified offset over the specified duration
        std::thread t([this, x_offset, y_offset, duration]() mutable {
            int max_offset = (std::abs(x_offset) > std::abs(y_offset)) ? abs(x_offset) : abs(y_offset); // Calculate the total offset
            int stepDuration = duration / max_offset; // Duration of each step, we have offset amount of steps
            int x_direction = (x_offset > 0) ? 1 : (x_offset < 0) ? -1 : 0; // Determine the direction of movement in x axis
            int y_direction = (y_offset > 0) ? 1 : (y_offset < 0) ? -1 : 0; // Determine the direction of movement in y axis
            for (int i = 0; i < max_offset; ++i) {
                if(x_offset != 0 && y_offset != 0) {
                    move(x_direction, y_direction); // Move in x and y directions
                    x_offset -= x_direction; // Decrease the offset in x direction
                    y_offset -= y_direction; // Decrease the offset in y direction
                }
                Sleep(stepDuration); // Wait for the duration of each step
            }
        });
        t.detach(); // Detach the thread to allow it to run independently
    }
    void draw(Graphics& graphics) const {
        SolidBrush brush(Color(255, 0, 0, 255)); // Blue color for the person
        graphics.FillRectangle(&brush, x, y, width, height);
    }
private:
    Floor *destination;
    HWND hwnd; // Handle to the window for drawing
};

class Floor {
public:
    Floor(HWND hwnd, int floorNumber, int x, int y, int length) : hwnd(hwnd), floorNumber(floorNumber), x(x), y(y), length(length) {}

    int getFloorNumber() const { return floorNumber; }
    std::deque<Person>& getQueue() { return queue; }
    std::vector<Person>& getLeaving() { return leaving; }

    int x;
    int y;
    int length;

    void addPerson(const Person& person) {
        queue.push_back(person);
    }

    void draw(Graphics& graphics) const {
        Pen pen(Color(255, 0, 0, 0)); // Black color for the floor line
        graphics.DrawLine(&pen, x, y, x+length, y);
        for (const auto& person : queue) {
            person.draw(graphics); // Draw each person in the queue
        }
        for (const auto& person : leaving) {
            person.draw(graphics); // Draw each person leaving the floor
        }
    }
private:
    int floorNumber;
    std::deque<Person> queue; // Queue of people waiting on this floor
    std::vector<Person> leaving; // People leaving the floor
    HWND hwnd; // Handle to the window for drawing
};

class Elevator {
public:
    Elevator(HWND hwnd)
        : hwnd(hwnd) {}

    const std::vector<Person>& getPassengers() const { return passengers; }
    const Floor *getCurrentFloor() const { return currentFloor; }

    int x;
    int y;
    int width;
    int height;

    void setValues(Floor *currentFloor, int x, int y, int width, int height) {
        this->currentFloor = currentFloor;
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
    }

    void move(int offset) {
        y+=offset; // Move the elevator up
        for(auto& passenger : passengers) 
            passenger.move(0, offset); // Move each passenger in the elevator
        RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW); // Update the entire window
    }

    //Made redundant by the moveToFloor function, but left here for reference
/*     void animate(int offset, double duration) {
        // Move the elevator up or down by the specified offset over the specified duration
        std::thread t([this, offset, duration]() {
            double stepDuration = duration / std::abs(offset); // Duration of each step, we have offset amount of steps
            int direction = (offset > 0) ? 1 : -1; // Determine the direction of movement
            for (int i = 0; i < std::abs(offset); ++i) {
                move(direction);
                Sleep(stepDuration); // Wait for the duration of each step
            }
        });
        t.detach(); // Detach the thread to allow it to run independently
    } */

    void moveToFloor(Floor *destination, int duration) {
        if (currentFloor->getFloorNumber() != destination->getFloorNumber()) {
            int offset = destination->y - currentFloor->y; // Calculate the offset to move to the destination floor
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
    void grabPassengers(std::queue<Person>& floor) {
        double totalWeight = 0.0;
        // if 1 more passenger will still fit in limit then grab
        while(!floor.empty() && (passengers.size() + 1) * 70 <= 600){
            passengers.push_back(floor.front());
            floor.pop();
        };
    }

    void draw(Graphics& graphics) const {
        Pen pen(Color(255, 255, 0, 0)); // Red color for the elevator
        graphics.DrawRectangle(&pen, Rect(x, y, width, height));
        for (const auto& passenger : passengers) {
            passenger.draw(graphics); // Draw each passenger in the elevator
        }
    }
private:
    Floor *currentFloor;
    std::vector<Person> passengers;
    HWND hwnd; // Handle to the window for drawing
};

class Building {
public:
    Building(HWND hwnd, int numFloors)
        : hwnd(hwnd), elevator(hwnd) {
            RECT rect;
        if (GetClientRect(hwnd, &rect)) {
            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;
            //height of window - 1px per floor and 3px space form bottom for clarity
            int floorHeight = (height-numFloors-3) / numFloors;
            for (int i = 0; i < numFloors; ++i) {
                int floorY = height - i * floorHeight - 3; // Calculate the y position of the floor
                if (i % 2 == 0) {
                    // Create a floor with a queue for even numbered floors
                    floors.emplace_back(hwnd, i, 0, floorY, (width-floorHeight*2)/2); // Queue occupies the left side of the floor
                } else {
                    // Create a floor without a queue for odd numbered floors
                    floors.emplace_back(hwnd, i, (width+floorHeight*2)/2, floorY, (width-floorHeight*2)/2); // Queue occupies the right side of the floor
                }
            }
            elevator.setValues(&floors[0], (width - floorHeight*2)/2+1, height - floorHeight - 2, floorHeight*2-3, floorHeight-2);// Initialize elevator in the middle and last floor
        }
    }

    // One big draw function bc i don't understand how to do this in separate functions (it was very wierd when it was here)
    void draw() {
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

    Elevator& getElevator() { return elevator; }
    std::vector<Floor>& getFloors() { return floors; }

private:
    HWND hwnd; // Handle to the window for drawing 
    Elevator elevator; // Elevator in the building
    std::vector<Floor> floors; // Floors in the building
};
