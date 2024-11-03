#ifndef BASE_H
#define BASE_H
#pragma one 
#define LOG_LEVEL LOG_LEVEL_DEBUG


#include <Arduino.h>
#include "EasyLogger.hpp"

void digitalHigh(uint8_t pin);

void digitalLow(uint8_t pin);

void pinInput(uint8_t pin);

void pinOutput(uint8_t pin);

void analogWritePercent(uint8_t pin, uint8_t percent);

unsigned long pulseInHigh(uint8_t pin);

unsigned long pulseInLow(uint8_t pin);

// extern enum command;
// enum event;
enum command
{
    COMMAND_NONE,
    COMMAND_STOP,
    COMMAND_SLOWDOWN,
    COMMAND_SPEEDUP,
    COMMAND_FORWARD,
    COMMAND_BACKWARD,
    COMMAND_TURN_LEFT,
    COMMAND_TURN_RIGHT,
};

//
enum event
{
    // EVENT_STOP,
    // EVENT_SLOWDOWN,
    // EVENT_SPEEDUP,
    // EVENT_FORWARD,
    // EVENT_BACKWARD,
    EVENT_OBSTACLE,    // 遇到障碍
    EVENT_RM_OBSTACLE, // 移除障碍
};

#endif