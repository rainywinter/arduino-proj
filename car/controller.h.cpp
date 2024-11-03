#include "controller.h"
#include "car.h"

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
        return;
    }
    enum command cmd = COMMAND_NONE;
    switch (m_instruction)
    {
    case 'A': // 0x41
        cmd = COMMAND_TURN_LEFT;
        break;

    case 'D': // 0x44
        cmd = COMMAND_TURN_RIGHT;
        break;

    case 'W': // 0x57
        cmd = COMMAND_FORWARD;
        break;

    case 'S': // 0x53
        cmd = COMMAND_BACKWARD;
        break;
    case 'H': // 0x48
        cmd = COMMAND_SPEEDUP;
        break;

    case 'L': // 0x4c
        cmd = COMMAND_SLOWDOWN;
        break;
    default:
        break;
    }
    LOG_INFO("controller:exec_instruction", "instruction=" << m_instruction << " cmd=" << cmd);
    if (cmd != COMMAND_NONE)
    {
        g_car->handle_command(cmd);
    }
};