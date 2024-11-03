#ifndef CONTROLLER_H
#define CONTROLLER_H
#pragma once

#include "base.h"
#include <Arduino.h>
#include <SoftwareSerial.h>

class controller
{
private:
    SoftwareSerial *m_bt; //  bluetooth
    char m_instruction;   // 指令

    controller() {};
    void exec_instruction();

public:
    controller(uint8_t tx_pin, uint8_t rx_pin)
    {
        m_bt = new SoftwareSerial(tx_pin, rx_pin);
        m_bt->begin(9600);
        m_instruction = '0';
    }

    void read_instruction();
};

static controller *g_controller = NULL;

#endif