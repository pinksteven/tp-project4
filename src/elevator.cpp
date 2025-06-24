#include "elevator.h"
#include "floor.h"
#include "person.h"
#include "button.h"

#include <thread>
#include <string>
#include <cmath>
#include <algorithm>


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
            floors.push_back(Floor(hwnd, i, floorX, floorY, (clientWidth-floorHeight*2)/2, floorHeight));
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
                Button button(btnX, btnY, btnSize, btnSize, std::to_wstring(j+1), &floors[i], &floors[j], this); // Button for each floor
                if(i!=j) floors[i].addButton(button); // Add the button to the floor
            }
        }
        awaitInput(); // Start waiting for user input
    }
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
                dropPassengers(); // Drop passengers at the current floor completing the movement loop
        });
        t.detach(); // Detach the thread to allow it to run independently
    }
}

void Elevator::grabPassengers() {
    std::thread([this]() {
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
            if (goingUp && current + 1 < floors.size()) {
                moveToFloor(&floors[current + 1], 1000);
            } else if(current - 1 >= 0) {
                moveToFloor(&floors[current - 1], 1000);
            }
        } else {
            awaitInput(); // Wait for user input if no destination floor is set
        }
    }).detach();
}

void Elevator::dropPassengers(){
    // Drop passengers at the current floor
    std::thread([this]() {
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
            destinationFloor = floorQueue.empty() ? nullptr : floorQueue.front(); // Set the next destination floor if available
            //floorQueue.pop_front(); // Remove the current destination floor from the queue
            goingUp = (destinationFloor && destinationFloor->getFloorNumber() > currentFloor->getFloorNumber()); // Determine if the elevator is going up or down
        };
        grabPassengers();
    }).detach();
}

void Elevator::awaitInput() {
    // Wait for user input to move the elevator
    std::thread([this]() {
        int timer = 0;
        while (floorQueue.empty()) {
            if(currentFloor->getFloorNumber()!=0) timer++; // Increment the timer if the elevator is not on the ground floor
            /* if (timer >= 5000) { // If the timer exceeds 5 seconds
                moveToFloor(&building->getFloors()[0], currentFloor->getFloorNumber() * 1000); // Move to the ground floor
                Sleep(currentFloor->getFloorNumber() * 1000);
            } */
            Sleep(1); // Sleep for 1 ms to avoid busy waiting
        }
        dropPassengers(); //This 
    }).detach();
}

void Elevator::draw() const {
    RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE); //It doesn't work without it, it seems like it should, but it doesn't
    // Draw the elevator and all floors
    HDC          hdc;
    PAINTSTRUCT  ps;
    hdc = BeginPaint(hwnd, &ps);
    Graphics graphics(hdc);
    Pen pen(Color(255, 255, 0, 0)); // Red color for the elevator
    graphics.DrawRectangle(&pen, Rect(x, y, width, height));
    for (const auto& passenger : passengers) {
        passenger.draw(graphics); // Draw each passenger in the elevator
    }
    for (auto& floor : floors) 
        floor.draw(graphics); // Draw each floor and people on it
    EndPaint(hwnd, &ps);
}