#include "car.h"
#include "EasyLogger.hpp"

car::car(uint8_t f_pin[2], uint8_t b_pin[2], uint8_t s_pin[2],uint8_t tr_echo_pins[2])
{
    m_engine = new engine(f_pin, b_pin, s_pin);
    m_sterring = new sterring(m_engine);
    m_accelerator = new accelerator(m_engine);
    m_gear = new gear(m_engine);
    m_radar = new radar(m_engine,tr_echo_pins);
}

car::~car()
{
    m_engine->shutdown();
}

void car::handle_event(event e) {

};

void car::handle_command(command c)
{
    LOG_INFO("car:handle_command", "comman=" << c);
    switch (c)
    {
    case COMMAND_STOP:
        m_gear->update_state(GEAR_STATE_P);
        break;
    case COMMAND_FORWARD:
        m_gear->update_state(GEAR_STATE_D);
        break;
    case COMMAND_BACKWARD:
        m_gear->update_state(GEAR_STATE_R);
        break;

    case COMMAND_SPEEDUP:
        m_accelerator->speedup();
        break;
    case COMMAND_SLOWDOWN:
        m_accelerator->slowdown();
        break;

    case COMMAND_TURN_LEFT:
        m_sterring->turnLeft();
        break;
    case COMMAND_TURN_RIGHT:
        m_sterring->turnRight();
        break;

    default:
        break;
    }
};

void car::check_distance(){
    m_radar->check_distance();
}

void sterring::turnLeft()
{
    if (m_engine->m_speed[0] < m_engine->m_speed[1])
    {
        return;
    }
    m_engine->m_speed[0] = m_engine->m_speed[1] - m_gap;
    m_engine->m_speed[0] = m_engine->m_speed[0] > 0 ? m_engine->m_speed[0] : 0;
    m_engine->apply_speed("turnLeft");
}

void sterring::turnRight()
{
    if (m_engine->m_speed[0] > m_engine->m_speed[1])
    {
        return;
    }
    m_engine->m_speed[1] = m_engine->m_speed[0] - m_gap;
    m_engine->m_speed[1] = m_engine->m_speed[1] > 0 ? m_engine->m_speed[1] : 0;
    m_engine->apply_speed("turnRight");
}

void accelerator::speedup()
{
    for (int i = 0; i < 2; i++)
    {
        m_engine->m_speed[i] += 10;
        m_engine->m_speed[i] = m_engine->m_speed[i] >= 100 ? 100 : m_engine->m_speed[i];
    }
    m_engine->apply_speed("speedup");
}

void accelerator::slowdown()
{
    for (int i = 0; i < 2; i++)
    {
        m_engine->m_speed[i] -= 10;
        m_engine->m_speed[i] = m_engine->m_speed[i] <= 0 ? 0 : m_engine->m_speed[i];
    }
    m_engine->apply_speed("slowdown");
}

void gear::update_state(gear_state state)
{
    m_engine->sync_speed(gear_states[state]);

    switch (state)
    {
    case GEAR_STATE_P:
    case GEAR_STATE_N:
        m_engine->shutdown();
        break;

    case GEAR_STATE_D:
        for (int i = 0; i < 2; i++)
        {
            digitalHigh(m_engine->m_forward_pin[i]);
            digitalLow(m_engine->m_backward_pin[i]);
        }
        m_engine->apply_speed(gear_states[state]);
        break;
    case GEAR_STATE_R:
        for (int i = 0; i < 2; i++)
        {
            digitalLow(m_engine->m_forward_pin[i]);
            digitalHigh(m_engine->m_backward_pin[i]);
        }
        m_engine->apply_speed(gear_states[state]);
        break;
    default:
        break;
    }
}

void engine::sync_speed(char reason[])
{
    uint8_t max = m_speed[0] > m_speed[1] ? m_speed[0] : m_speed[1];
    m_speed[0] = max;
    m_speed[1] = max;

    LOG_INFO("engine:sync_speed", "speed=" << max << " reason=" << reason);
}

void engine::apply_speed(char reason[])
{
    for (int i = 0; i < 2; i++)
    {
        analogWritePercent(m_speed_pin[i], m_speed[i]);
    }
    LOG_INFO("engine:apply_speed", "lspeed=" << m_speed[0] << " rspeed=" << m_speed[1] << " reason=" << reason);
}

void engine::shutdown()
{
    LOG_INFO("engine:shutdown", "");
    for (int i = 0; i < 2; i++)
    {
        digitalLow(m_forward_pin[i]);
        digitalLow(m_backward_pin[i]);
        digitalLow(m_speed_pin[i]);
    }
}

void radar::check_distance()
{
    digitalLow(m_trigger_pin);
    delayMicroseconds(5);

    digitalHigh(m_trigger_pin);
    delayMicroseconds(10);
    digitalLow(m_trigger_pin);

    m_duration = pulseInHigh(m_echo_pin);
    m_distance = m_duration * 0.034 / 2;

    LOG_DEBUG("radar:check_distance", "duration=" << m_duration << " distance=" << m_distance);

    if (0 < m_distance && m_distance <= m_safe_distance)
    {
        if (!m_emit[0])
        {
            // 停车
            m_engine->shutdown();

            // 抛出事件，为自动避障提供依据
            g_car->handle_event(EVENT_OBSTACLE);
            m_emit[0] = true;
            m_emit[1] = false;
        }
    }
    else
    {
        if (!m_emit[1])
        {
            // 移除障碍，恢复行驶状态
            m_engine->apply_speed("rm_obstacle");

            g_car->handle_event(EVENT_RM_OBSTACLE);
            m_emit[1] = true;
            m_emit[0] = false;
        }
    }
}