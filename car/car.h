#ifndef CAR_H
#define CAR_H
#pragma once 

#include <Arduino.h>


class car;
class engine;

class sterring
{
private:
    uint8_t m_gap;
    engine *m_engine;

    sterring() {};

public:
    sterring(engine *e) {
        Serial.println("sterring constructor=>");
        m_engine = e;
        m_gap = 15;
    }
    void turnLeft();
    void turnRight();
};

class accelerator
{
private:
    engine *m_engine;

    accelerator() {};

public:
    accelerator(engine *e) : m_engine(e) {
        Serial.println("accelerator constructor=>");
    }
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
    gear(engine *e) : m_engine(e) { 
        Serial.println("gear constructor=>"); 
        m_state = GEAR_STATE_P; 
    }
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
        Serial.println("rardar constructor=>");
        m_trigger_pin=pins[0];
        m_echo_pin=pins[1];
        pinMode(m_trigger_pin,OUTPUT);
        pinMode(m_echo_pin,INPUT);
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
    void pause();

public:
    engine(uint8_t f_pin[2], uint8_t b_pin[2], uint8_t s_pin[2])
    {
        // memcpy(m_forward_pin, f_pin, sizeof(f_pin));
        // memcpy(m_backward_pin, b_pin, sizeof(b_pin));
        // memcpy(m_speed_pin, s_pin, sizeof(s_pin));
        Serial.println("engine constructor=>");
        for(int i=0; i<2; i++){
            m_forward_pin[i] = f_pin[i];
            m_backward_pin[i] = b_pin[i];
            m_speed_pin[i] = s_pin[i];
        }
       

        for (int i = 0; i < 2; i++)
        {
            pinMode(m_forward_pin[i],OUTPUT);
            pinMode(m_backward_pin[i],OUTPUT);
            pinMode(m_speed_pin[i],OUTPUT);

            m_speed[i] = 0;
        }
    }
};

enum class event
{
    // EVENT_STOP,
    // EVENT_SLOWDOWN,
    // EVENT_SPEEDUP,
    // EVENT_FORWARD,
    // EVENT_BACKWARD,
    EVENT_OBSTACLE,    // 遇到障碍
    EVENT_RM_OBSTACLE, // 移除障碍
};

enum class command
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

class car
{
private:
    engine *m_engine;
    sterring *m_sterring;
    accelerator *m_accelerator;
    gear *m_gear;
    radar *m_radar;


public:
    car(uint8_t f_pin[2], uint8_t b_pin[2], uint8_t s_pin[2],uint8_t tr_echo_pins[2]){
        Serial.println("car constructor begin=>");
        m_engine = new engine(f_pin, b_pin, s_pin);
        m_sterring = new sterring(m_engine);
        m_accelerator = new accelerator(m_engine);
        m_gear = new gear(m_engine);
        m_radar = new radar(m_engine,tr_echo_pins);
    }
    ~car(){
        Serial.println("car deconstructor");
        m_engine->shutdown();
    }

    void handle_event(event e);
    void handle_command(command c);
    void check_distance();
};


#endif