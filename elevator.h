#include <vector>
#include <queue>

class Person {
public:
    Person(int id, int destination)
        : id(id),  destination(destination) {}

    int getId() const { return id; }
    int getDestination() const { return destination; }
    const double weight = 70.0; // Default weight in kg
private:
    int id;
    int destination;
};

class Elevator {
public:
    Elevator(int currentFloor)
        : currentFloor(currentFloor) {}
    const double maxWeight = 600.0; // Maximum weight in kg

    std::vector<Person> getPassengers() const { return passengers; }
    int getCurrentFloor() const { return currentFloor; }

    void moveToFloor(int floor) {
        if (floor >= 0) {
            currentFloor = floor;
        }
    }
    void grabPassengers(std::queue<Person>& floor) {
        double totalWeight = 0.0;
        while(!floor.empty() && passengers.size() * passengers[0].weight <= maxWeight - passengers[0].weight){
            passengers.push_back(floor.front());
            floor.pop();
        };
    }
private:
    int currentFloor;
    std::vector<Person> passengers;
};