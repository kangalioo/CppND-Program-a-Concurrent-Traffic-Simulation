#include <iostream>
#include <random>
#include <chrono>
#include <ratio>
#include <thread>
#include <cstdlib>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

template <typename T>
T MessageQueue<T>::receive()
{
    // Wait until a message is ready
    std::unique_lock lock(_banana);
    _condition.wait(lock);

    // you can never have enough std::move's, RIGHT???
    T value = std::move(_queue.front());
    _queue.pop_front();
    return std::move(value);
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::lock_guard lock(_banana);
    _queue.push_back(msg);
    _condition.notify_one();
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    while (true) {
        if (_queue.receive() == TrafficLightPhase::green) return;
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    _threads.push_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

float random_float_in_range(float min, float max) {
    auto random_float = (float) rand() / (float) RAND_MAX;
    return min + (max - min) * random_float;
}

// aaaaaaaaaaaaaaaaaaaaaaaaa why is chrono so jank
std::chrono::duration<float> duration_from_seconds(float seconds) {
    return std::chrono::nanoseconds((long) (seconds * 1'000'000'000));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    while (true) {
        auto cycle_duration = duration_from_seconds(random_float_in_range(4, 6));
        std::cout << "Waiting for " << cycle_duration.count() << "\n";

        // "Why use the idiomatic C++ way when you can overengineer it"
        //   - Udacity, probably
        auto wait_start = std::chrono::steady_clock::now();
        while (std::chrono::steady_clock::now() - wait_start < cycle_duration) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        if (_currentPhase == TrafficLightPhase::green) {
            _currentPhase = TrafficLightPhase::red;
        } else {
            _currentPhase = TrafficLightPhase::green;
        }

        // Move semantics are absolutely overkill and useless here, the type in question is a
        // freaking 2-variant enum, it's literally a single bit
        _queue.send(std::move(_currentPhase));
    }
}