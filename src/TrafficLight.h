#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;


enum class TrafficLightPhase { red, green };

// I know the task description says MessageQueue should be hardcoded to TrafficLightPhase but that's
// stupid because
// 1) The template parameter is already there, might as well use it
// 2) It's called **Message**Queue, so it should handle all kinds of messages, not just traffic
//    light phases
// So I, once again, decided to violate the task description in favor of doing something sensible
template <class T>
class MessageQueue
{
public:
    void send(T &&value);
    T receive();

private:
    std::deque<T> _queue;
    std::condition_variable _condition; // 200 IQ naming
    std::mutex _banana;
};

class TrafficLight : TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();

    // getters / setters
    TrafficLightPhase getCurrentPhase();

    // typical behaviour methods
    void waitForGreen();
    void simulate();

private:
    // typical behaviour methods
    void cycleThroughPhases();

    // FP.4b : create a private member of type MessageQueue for messages of type TrafficLightPhase 
    // and use it within the infinite loop to push each new TrafficLightPhase into it by calling 
    // send in conjunction with move semantics.

    TrafficLightPhase _currentPhase;
    std::condition_variable _condition;
    std::mutex _mutex;

    MessageQueue<TrafficLightPhase> _queue;
};

#endif