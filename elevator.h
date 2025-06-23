#include <vector>
#include <queue>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

class Person {
public:
    Person(HWND hwnd, int destination)
        : hwnd(hwnd),  destination(destination), x(x), y(y){}

    int getId() const { return id; }
    int getDestination() const { return destination; }
    void move(int x_offset, int y_offset) {
        x+= x_offset; // Move the person in x direction
        y+=y_offset; // Move the person in y direction
        RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW); // Update the entire window
    }

    void erase() {
        // Create a rectangle size of apx high line
        RECT updateRect;
        updateRect.left = x;
        updateRect.top = y;
        updateRect.right = x + width;
        updateRect.bottom = y + height;
        RedrawWindow(hwnd, &updateRect, NULL, RDW_INVALIDATE | RDW_UPDATENOW);//update the person part of the window
    }
/*     void draw() {
        HDC          hdc;
        PAINTSTRUCT  ps;
        hdc = BeginPaint(hwnd, &ps);
        Graphics graphics(hdc);
        SolidBrush brush(Color(255, 0, 0, 255)); // Blue color for the person
        graphics.FillRectangle(&brush, x, y, width, height);
        EndPaint(hwnd, &ps);
    } */
private:
    int id;
    int destination;
    int x;
    int y;
    int height;
    int width;
    HWND hwnd; // Handle to the window for drawing
};

class Floor {
public:
    Floor(HWND hwnd, int floorNumber, int x, int y, int length) : hwnd(hwnd), floorNumber(floorNumber), x(x), y(y), length(length) {}

    int getFloorNumber() const { return floorNumber; }
    std::queue<Person>& getQueue() { return queue; }

    int x;
    int y;
    int length;

    void addPerson(const Person& person) {
        queue.push(person);
    }

/*     void draw() {
        // Create a rectangle size of apx high line
        RECT updateRect;
        updateRect.left = x;
        updateRect.top = y;
        updateRect.right = x + length;
        updateRect.bottom = y + 1;

        // RedrawWindow(hwnd, &updateRect, NULL, RDW_INVALIDATE);//update the floor part of the window
        HDC          hdc;
        PAINTSTRUCT  ps;
        hdc = BeginPaint(hwnd, &ps);
        Graphics graphics(hdc);
        Pen redPen(Color(255, 0, 0, 0)); // Black color for the floor line
        graphics.DrawLine(&redPen, x, y, x+length, y);
        EndPaint(hwnd, &ps);
    } */
private:
    int floorNumber;
    std::queue<Person> queue; // Queue of people waiting on this floor
    std::vector<Person> leaving; // People leaving the floor
    HWND hwnd; // Handle to the window for drawing
};

class Elevator {
public:
    Elevator(HWND hwnd)
        : hwnd(hwnd) {}

    std::vector<Person> getPassengers() const { return passengers; }
    int getCurrentFloor() const { return currentFloor; }

    int x;
    int y;
    int width;
    int height;

    void setValues(int currentFloor, int x, int y, int width, int height) {
        this->currentFloor = currentFloor;
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
    }

    void moveToFloor(int floor) {
        if (floor >= 0) {
            currentFloor = floor;
        }
    }
    void grabPassengers(std::queue<Person>& floor) {
        double totalWeight = 0.0;
        // can't grab from empty floor, 1 passenger is 70kg, max weight is 600kg so if weight of all pasengers on bard is <= 530kg, we can grab more passengers
        while(!floor.empty() && passengers.size() * 70 <= 530){
            passengers.push_back(floor.front());
            floor.pop();
        };
    }

    void move(int offset) {
        y+=offset; // Move the elevator up
        for(auto& passenger : passengers) 
            passenger.move(0, offset); // Move each passenger in the elevator
        RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW); // Update the entire window
    }

/*     void draw() {
        // Create a rectangle spanning the entire elevator shaft
        RECT updateRect;
        GetClientRect(hwnd, &updateRect);//grab whole window rect
        updateRect.left = x;//change to start at elevator x position
        updateRect.right = x + width+1;//change to end when elevator ends add 1px bc of the border line
        // RedrawWindow(hwnd, &updateRect, NULL, RDW_INVALIDATE);//update the elevetaor shat portion of the window
        HDC          hdc;
        PAINTSTRUCT  ps;
        hdc = BeginPaint(hwnd, &ps);
        Graphics graphics(hdc);
        Pen redPen(Color(255, 255, 0, 0)); // Red color for the elevator
        graphics.DrawRectangle(&redPen, Rect(x, y, width, height));
        EndPaint(hwnd, &ps);
        move(-1);
    } */
private:
    int currentFloor;
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
            elevator.setValues(0, (width - floorHeight*2)/2+1, height - floorHeight - 2, floorHeight*2-3, floorHeight-2); // Initialize elevator in the middle and last floor
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
        }
    }

    // One big draw function bc i don't understand how to do this in separate functions (it was very wierd when it was here)
    void draw() {
        RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
        // Draw the elevator and all floors
        HDC          hdc;
        PAINTSTRUCT  ps;
        hdc = BeginPaint(hwnd, &ps);
        Graphics graphics(hdc);
        Pen redPen(Color(255, 255, 0, 0)); // Red color for the elevator
        Pen blackPen(Color(255, 0, 0, 0)); // Black color for the floor line
        graphics.DrawRectangle(&redPen, Rect(elevator.x, elevator.y, elevator.width, elevator.height));
        for (auto& floor : floors) {
            graphics.DrawLine(&blackPen, floor.x, floor.y, floor.x+floor.length, floor.y);
        }
        EndPaint(hwnd, &ps);
    }

    Elevator& getElevator() { return elevator; }
    std::vector<Floor>& getFloors() { return floors; }

private:
    HWND hwnd; // Handle to the window for drawing 
    Elevator elevator; // Elevator in the building
    std::vector<Floor> floors; // Floors in the building
};
