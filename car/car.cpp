#include "Arduino.h"
#include "car.h"

extern car *g_car;

void car::handle_event(event e) {

};

void car::handle_command(command c)
{
    Serial.print("car:handle_command ");
    Serial.println(int8_t(c));
    switch (c)
    {
    case command::COMMAND_STOP:
        m_gear->update_state(GEAR_STATE_P);
        break;
    case command::COMMAND_FORWARD:
        m_gear->update_state(GEAR_STATE_D);
        break;
    case command::COMMAND_BACKWARD:
        m_gear->update_state(GEAR_STATE_R);
        break;

    case command::COMMAND_SPEEDUP:
        m_accelerator->speedup();
        break;
    case command::COMMAND_SLOWDOWN:
        m_accelerator->slowdown();
        break;

    case command::COMMAND_TURN_LEFT:
        m_sterring->turnLeft();
        break;
    case command::COMMAND_TURN_RIGHT:
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
    if (m_engine->m_speed[0] != m_engine->m_speed[1]){
      m_engine->sync_speed("from turnRight to turnLeft");
    }

    if(m_engine->m_speed[1] - m_gap > 0){
      m_engine->m_speed[0] = m_engine->m_speed[1] - m_gap;
    }else{
      m_engine->m_speed[0] = 0;
    }
    m_engine->apply_speed("turnLeft");
}

void sterring::turnRight()
{
    if (m_engine->m_speed[0] > m_engine->m_speed[1])
    {
        return;
    }
    if (m_engine->m_speed[0] != m_engine->m_speed[1]){
      m_engine->sync_speed("from turnLeft to turnRight");
    }
    if (m_engine->m_speed[0] - m_gap > 0){
      m_engine->m_speed[1] = m_engine->m_speed[0] - m_gap;
    }else{
      m_engine->m_speed[1] = 0;
    }
    m_engine->apply_speed("turnRight");
}

void accelerator::speedup()
{
    for (int i = 0; i < 2; i++)
    {
        if(m_engine->m_speed[i] < 100){
            m_engine->m_speed[i] += 10;
        }
    }
    m_engine->apply_speed("speedup");
}

void accelerator::slowdown()
{
    for (int i = 0; i < 2; i++)
    {
        if (m_engine->m_speed[i] > 0){
            m_engine->m_speed[i] -= 10;
        }
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
            digitalWrite(m_engine->m_forward_pin[i],HIGH);
            digitalWrite(m_engine->m_backward_pin[i],LOW);
        }
        m_engine->apply_speed(gear_states[state]);
        break;
    case GEAR_STATE_R:
        for (int i = 0; i < 2; i++)
        {
            digitalWrite(m_engine->m_forward_pin[i],LOW);
            digitalWrite(m_engine->m_backward_pin[i],HIGH);
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

    Serial.print("engine:sync_speed ");
    Serial.print(max);
    Serial.print(" ");
    Serial.println(reason);
}

void engine::apply_speed(char reason[])
{
    for (int i = 0; i < 2; i++)
    {
        analogWrite(m_speed_pin[i], 255 * m_speed[i] / 100);
    }
    Serial.print("engine:apply_speed, l=");
    Serial.print(m_speed[0]);
    Serial.print(" r=");
    Serial.print(m_speed[1]);
    Serial.print(" ");
    Serial.println(reason);
}

void engine::shutdown()
{
    Serial.println("engine:shutdown");
    for (int i = 0; i < 2; i++)
    {
        digitalWrite(m_forward_pin[i],LOW);
        digitalWrite(m_backward_pin[i],LOW);
        digitalWrite(m_speed_pin[i],LOW);
    }
}

void engine::pause()
{
    Serial.println("engine:pause");
    for (int i = 0; i < 2; i++)
    {
        digitalWrite(m_speed_pin[i],LOW);
    }
}

void radar::check_distance()
{
    digitalWrite(m_trigger_pin,LOW);
    delayMicroseconds(5);

    digitalWrite(m_trigger_pin,HIGH);
    delayMicroseconds(10);
    digitalWrite(m_trigger_pin,LOW);

    m_duration = pulseIn(m_echo_pin,HIGH);
    m_distance = m_duration * 0.034 / 2;

    if (0 < m_distance && m_distance <= m_safe_distance)
    {
        if (!m_emit[0])
        {
            // 停车
            m_engine->pause();

            // 抛出事件，为自动避障提供依据
            g_car->handle_event(event::EVENT_OBSTACLE);
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

            g_car->handle_event(event::EVENT_RM_OBSTACLE);
            m_emit[1] = true;
            m_emit[0] = false;
        }
    }
}