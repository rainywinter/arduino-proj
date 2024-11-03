#include "base.h"
#include <Arduino.h>
#include "EasyLogger.hpp"

void digitalHigh(uint8_t pin)
{
    LOG_DEBUG("digitalWrite", "pin=" << pin << " val=HIGH");
    digitalWrite(pin, HIGH);
}

void digitalLow(uint8_t pin)
{
    LOG_DEBUG("digitalWrite", "pin=" << pin << " val=LOW");
    digitalWrite(pin, LOW);
}

void pinInput(uint8_t pin)
{
    LOG_INFO("pinMode", "pin=" << pin << " mode=INPUT");
    pinMode(pin, INPUT);
}

void pinOutput(uint8_t pin)
{
    LOG_INFO("pinMode", "pin=" << pin << " mode=OUTPUT");
    pinMode(pin, OUTPUT);
}

void analogWritePercent(uint8_t pin, uint8_t percent)
{
    LOG_DEBUG("analogWritePercent", "pin=" << pin << " percent=" << percent);
    analogWrite(pin, 255 * percent / 100);
}

unsigned long pulseInHigh(uint8_t pin)
{
    LOG_DEBUG("pulseIn", "pin=" << pin << " val=HIGH");
    return pulseIn(pin, HIGH);
}

unsigned long pulseInLow(uint8_t pin)
{
    LOG_DEBUG("pulseIn", "pin=" << pin << " val=LOW");
    return pulseIn(pin, LOW);
}

// enum command
// {
//     COMMAND_NONE,
//     COMMAND_STOP,
//     COMMAND_SLOWDOWN,
//     COMMAND_SPEEDUP,
//     COMMAND_FORWARD,
//     COMMAND_BACKWARD,
//     COMMAND_TURN_LEFT,
//     COMMAND_TURN_RIGHT,
// };

// //
// enum event
// {
//     // EVENT_STOP,
//     // EVENT_SLOWDOWN,
//     // EVENT_SPEEDUP,
//     // EVENT_FORWARD,
//     // EVENT_BACKWARD,
//     EVENT_OBSTACLE,    // 遇到障碍
//     EVENT_RM_OBSTACLE, // 移除障碍
// };