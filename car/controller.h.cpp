#include "controller.h"
#include "car.h"

extern car * g_car;

void controller::read_instruction()
{
    if (m_bt->available() <= 0)
    {
        return;
    }
    m_instruction = m_bt->read();

    exec_instruction();
}

void controller::exec_instruction()
{
    
    if (g_car == NULL)
    {
        Serial.println("controller:exec_instruction null g_car");
        return;
    }
    command cmd = command::COMMAND_NONE;
    switch (m_instruction)
    {
    case 'A': // 0x41
        cmd = command::COMMAND_TURN_LEFT;
        break;

    case 'D': // 0x44
        cmd = command::COMMAND_TURN_RIGHT;
        break;

    case 'W': // 0x57
        cmd = command::COMMAND_FORWARD;
        break;

    case 'S': // 0x53
        cmd = command::COMMAND_BACKWARD;
        break;
    case 'H': // 0x48
        cmd = command::COMMAND_SPEEDUP;
        break;

    case 'L': // 0x4c
        cmd = command::COMMAND_SLOWDOWN;
        break;
    default:
        cmd = command::COMMAND_STOP;
        break;
    }
    Serial.print("controller:exec_instruction ");
    Serial.print(m_instruction);
    Serial.print(" cmd=");
    Serial.println(int8_t(cmd));
    if (cmd != command::COMMAND_NONE)
    {
        g_car->handle_command(cmd);
    }
};