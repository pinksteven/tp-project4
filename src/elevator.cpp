#include "elevator.h"
#include "floor.h"
#include "person.h"
#include "button.h"

#include <string>
#include <thread>
#include <cmath>
#include <algorithm>

#define ELEVATOR_MOVE (WM_USER+1)
#define ELEVATOR_DROP (WM_USER + 2)
#define ELEVATOR_GRAB (WM_USER + 3)
#define ELEVATOR_WAIT (WM_USER + 4)

Elevator::Elevator(HWND hwnd, int numFloors)
        : hwnd(hwnd), destinationFloor(nullptr), goingUp(true) {
        RECT rect;
    if (GetClientRect(hwnd, &rect)) {
        int clientWidth = rect.right - rect.left;
        int clientHeight = rect.bottom - rect.top;
        //height of window - 1px per floor and 3px space form bottom for clarity
        int floorHeight = (clientHeight-numFloors-3) / numFloors;
        floors.reserve(numFloors); // Reserve space for the floors to avoid multiple reallocations
        for (int i = 0; i < numFloors; ++i) {
            int floorY = clientHeight - i * floorHeight - 3; // Calculate the y position of the floor
            int floorX = (i % 2 == 0) ? 0 : (clientWidth + floorHeight * 2)/2; // X position based on floor number
            // Create a new floor and add it to the vector
            floors.emplace_back(hwnd, i, floorX, floorY, (clientWidth-floorHeight*2)/2, floorHeight);
        }
        currentFloor = &floors[0]; // Set the current floor to the first floor
        x = (clientWidth - floorHeight*2)/2; // Center the elevator horizontally
        y = clientHeight - floorHeight - 3; // Position the elevator at the bottom of window (3px for clarity)
        width = floorHeight*2; // Width of the elevator
        height = floorHeight; // Height of the elevator
        for(int i = 0; i < numFloors; ++i) {
            for(int j=0; j<floors.size(); j++) {
                int btnSize = (floorHeight - numFloors*2) / (numFloors+1); // Calculate button size based on the number of floors
                int btnX = (i % 2 == 0) ? 1 : clientWidth-btnSize-1; // X position based on floor number
                int btnY = floors[i].getY() - (j+1) * (btnSize+2) - btnSize/2; // Y position based on floor number
                // Create a button for each floor
                Button button(btnX, btnY, btnSize, btnSize, std::to_wstring(j+1), &floors[i], &floors[j]); // Button for each floor
                if(i!=j) addButton(button); // Add the button to the floor
            }
        }
        awaitInput();
    }
}


void Elevator::move(int offset) {
        y+=offset; // Move the elevator up
        for(auto& passenger : passengers) 
            passenger.move(0, offset); // Move each passenger in the elevator
        RECT invalidate;
        invalidate.left = x;
        invalidate.top = floors.back().getY() - floors.back().getHeight();
        invalidate.right = x + width+1;
        invalidate.bottom = floors[0].getY()+1;
        InvalidateRect(hwnd, &invalidate, TRUE); // Update the entire window
    }

void Elevator::moveToFloor(const Floor *destination, int duration) {
    if (*currentFloor != *destination) {
        int offset = destination->getY() - currentFloor->getY(); // Calculate the offset to move to the destination floor
        // Move the elevator up or down by the specified offset over the specified duration
        killThread();
        thread = std::thread ([this, destination, offset, duration]() {
            int stepDuration = duration / std::abs(offset); // Duration proportional to the distance
            int direction = (offset > 0) ? 1 : -1; // Determine the direction of movement
            for (int i = 0; i < std::abs(offset); ++i) {
                move(direction);
                Sleep(stepDuration); // Wait for the duration of each step
            }
                currentFloor = const_cast<Floor*>(destination); // Update the current floor after moving
                //PostMessage(hwnd, ELEVATOR_DROP, 0, 0); // Drop passengers at the current floor completing the movement loop
        });
    }
}

void Elevator::grabPassengers() {
    killThread();
    thread = std::thread([this]() {
        auto& queue = currentFloor->getQueue();
        /* for (auto it = queue.begin(); it != queue.end(); ) {
            if ((*it).isGoingUp()==goingUp && destinationFloor != nullptr && (passengers.size()+1)*70 <= 600) {
                floorQueue.erase(std::find(floorQueue.begin(), floorQueue.end(), currentFloor)); // Remove the current floor from the queue
                Person enteringPerson = *it; // Create a copy of the person entering the elevator
                int movement;
                if (currentFloor->getFloorNumber()%2==0){
                    passengers.push_front(enteringPerson); // Add the waiting to passenger vector
                    movement = enteringPerson.getWidth(); // Calculate the movement direction
                    for(auto& p : passengers) {
                        p.animate(movement, 0, 100); // Move each passenger in the elevator
                    }
                    int newMove = x - enteringPerson.getX();
                    passengers.front().animate(newMove, 0, 100); // Animate the waiting passenger
                } else {
                    passengers.push_back(enteringPerson); // Add the waiting to passenger vector
                    movement = -enteringPerson.getWidth(); // Calculate the movement direction
                    for(auto& p : passengers) {
                        p.animate(movement, 0, 100); // Move each passenger in the elevator
                    }
                    int newMove = x + width - enteringPerson.getX();
                    passengers.back().animate(newMove, 0, 100); // Animate the waiting passenger
                    
                }
                for(auto j = std::next(it); it != queue.end(); ++j) {
                    j->animate(movement, 0, 100); // Move each passenger in the elevator
                }
                it = queue.erase(it); // Remove the passenger from the queue
                Sleep(100); // Wait for a short duration to have spacing between leaving passengers
            }
        } */
        if(destinationFloor != nullptr){
            int current = currentFloor->getFloorNumber();
            Floor* nextFloor = (goingUp) ? &floors[current+1] : &floors[current-1];
            PostMessage(hwnd, ELEVATOR_MOVE, 0, reinterpret_cast<LPARAM>(nextFloor));
        } else if(!passengers.empty()){

        }
        else {
            PostMessage(hwnd, ELEVATOR_WAIT, 0, 0); // Wait for user input if no destination floor is set
        }
    });
}

void Elevator::dropPassengers(){
    killThread();
    // Drop passengers at the current floor
    thread = std::thread ([this]() {
        /* for (auto it = passengers.begin(); it != passengers.end(); ) {
            if (*it->getDestination() == *currentFloor) {
                Person leavingPerson = *it;
                currentFloor->getLeaving().push_back(leavingPerson); // Add the passenger to the leaving list of the current floor
                //currentFloor->getLeaving().back().leave(); // Animate the passenger leaving the floor
                int movement = (currentFloor->getFloorNumber() % 2 == 0) ? -leavingPerson.getWidth() : leavingPerson.getWidth(); // Calculate the movement direction
                for(auto j = std::next(it); j!=passengers.end(); ++j) {
                    j->animate(movement, 0, 100); // Move each passenger in the elevator
                }
                it = passengers.erase(it); // Remove the passenger from the elevator
                Sleep(100); // Wait for a short duration to have spacing between leaving passengers
            } else {
                ++it; // Move to the next passenger if not leaving
            }
        } */
        if(*currentFloor == *destinationFloor || destinationFloor == nullptr) {
            Floor* newDest = (!floorQueue.empty()) ? floorQueue.front() : nullptr;
            if(newDest!=nullptr) unqueueFloor(newDest);
            destinationFloor = newDest;
        };
        PostMessage(hwnd, ELEVATOR_GRAB, 0, 0);
    });
}

void Elevator::awaitInput() {
    killThread();
    wait = true;
    thread = std::thread ([this]() {
        int timer = 0;
        while (wait) {
            Sleep(1); // Wait for a request
            timer++;
            if (timer==5000) PostMessage(hwnd, ELEVATOR_MOVE, 0, 0);
        }
        // There is a request
        Floor* requestedFloor = (!floorQueue.empty()) ? floorQueue.front() : nullptr;
        if (currentFloor != requestedFloor && requestedFloor != nullptr) {
            PostMessage(hwnd, ELEVATOR_MOVE, 0, reinterpret_cast<LPARAM>(requestedFloor)); // Move to requested floor
        } else {
            //PostMessage(hwnd, ELEVATOR_DROP, 0, 0); // Already at requested floor
        }
    });
}

void Elevator::killThread() {
    wait=false;
    if(thread.joinable()) thread.join();
}

void Elevator::draw() const {
    RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE); //It doesn't work without it, it seems like it should, but it doesn't
    // Draw the elevator and all floors
    HDC          hdc;
    PAINTSTRUCT  ps;
    hdc = BeginPaint(hwnd, &ps);
    Graphics graphics(hdc);

    // Set the clip region to the invalidated rectangle
    Rect clipRect(ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top);
    graphics.SetClip(clipRect);

    Pen pen(Color(255, 255, 0, 0)); // Red color for the elevator
    graphics.DrawRectangle(&pen, Rect(x, y, width, height));
    for (const auto& passenger : passengers)
        passenger.draw(graphics); // Draw each passenger in the elevator
    for (const auto& floor : floors) 
        floor.draw(graphics); // Draw each floor and people on it
    for (const auto& button : buttons) 
        button.draw(graphics); // Draw each button on the floor
    EndPaint(hwnd, &ps);
}