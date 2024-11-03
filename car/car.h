#ifndef CAR_H
#define CAR_H
#pragma once 

#include <Arduino.h>
#include "base.h"


class car;
class sterring;
class accelerator;
class gear;
class radar;
class engine;

class sterring
{
private:
    uint8_t m_gap;
    engine *m_engine;

    sterring() {};

public:
    sterring(engine *e) : m_engine(e), m_gap(30) {}
    void turnLeft();
    void turnRight();
};

class accelerator
{
private:
    engine *m_engine;

    accelerator() {};

public:
    accelerator(engine *e) : m_engine(e) {}
    void speedup();
    void slowdown();
};

enum gear_state
{
    GEAR_STATE_P, // stop
    GEAR_STATE_N, // ready
    GEAR_STATE_D, // forward
    GEAR_STATE_R, // backward
};

char *const gear_states[] =
    {
        [GEAR_STATE_P] = "stop",
        [GEAR_STATE_N] = "none",
        [GEAR_STATE_D] = "drive",
        [GEAR_STATE_R] = "return"};

class gear
{
private:
    gear_state m_state;
    engine *m_engine;

    gear() {};

public:
    gear(engine *e) : m_engine(e) { m_state = GEAR_STATE_P; }
    void update_state(gear_state state);
};

class radar
{
private:
    radar() {};
    engine *m_engine;
    uint8_t m_trigger_pin;
    uint8_t m_echo_pin;

    unsigned long m_duration; // micro second
    uint16_t m_distance;      // cm

    uint16_t m_safe_distance;

    bool m_emit[2]; // obstacle , rm obstacle
public:
    radar(engine *e, uint8_t pins[2])
    {
        m_trigger_pin=pins[0];
        m_echo_pin=pins[1];
        pinOutput(m_trigger_pin);
        pinInput(m_echo_pin);
        m_engine = e;
        m_duration = 0;
        m_distance = 0;
        
        m_safe_distance = 30;
        for (int i = 0; i < 2; i++)
        {
            m_emit[i] = false;
        }
    };
    void check_distance();
};

class engine
{
    friend class car;
    friend class sterring;
    friend class accelerator;
    friend class gear;
    friend class radar;

private:
    // below 3 pin arrar[2] means: left, right
    uint8_t m_forward_pin[2];
    uint8_t m_backward_pin[2];
    uint8_t m_speed_pin[2];

    uint8_t m_speed[2];

    void sync_speed(char reason[]);
    void apply_speed(char reason[]);
    void shutdown();

    engine();

public:
    engine(uint8_t f_pin[2], uint8_t b_pin[2], uint8_t s_pin[2])
    {
        memcpy(m_forward_pin, f_pin, sizeof(f_pin));
        memcpy(m_backward_pin, b_pin, sizeof(b_pin));
        memcpy(m_speed_pin, s_pin, sizeof(s_pin));

        for (int i = 0; i < 2; i++)
        {
            pinOutput(m_forward_pin[i]);
            pinOutput(m_backward_pin[i]);
            pinOutput(m_speed_pin[i]);

            m_speed[i] = 0;
        }
    }
};


static car *g_car = NULL;
class car
{
private:
    engine *m_engine;
    sterring *m_sterring;
    accelerator *m_accelerator;
    gear *m_gear;
    radar *m_radar;

    car(/* args */) {}

public:
    car(uint8_t f_pin[2], uint8_t b_pin[2], uint8_t s_pin[2],uint8_t tr_echo_pins[2]);
    ~car();

    void handle_event(event e);
    void handle_command(command c);
    void check_distance();
};


#endif