#include <Arduino.h>
#include <SoftwareSerial.h>
#include <TaskScheduler.h>
#include "EasyLogger.hpp"
#include "car.h"
#include "controller.h"



void check_distance();
void read_instruction();

Task distanceTask(5, TASK_FOREVER, &check_distance);
Task btTask(2000000, TASK_FOREVER, &read_instruction);
Scheduler runner;

void setup_scheduler()
{
    runner.init();
    runner.addTask(distanceTask);
    runner.addTask(btTask);
    distanceTask.enable();
    btTask.enable();
}

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(9600);
    
    uint8_t f_pin[2] = {4,7};
    uint8_t b_pin[2] = {5,6};
    uint8_t s_pin[2] = {10,11};
    uint8_t tr_echo_pins[2] = {2,3};
    g_car = new car(f_pin,b_pin,s_pin,tr_echo_pins);

    g_controller = new controller(8,9);

    setup_scheduler();
}

void loop()
{
    runner.execute();
}

void check_distance()
{
    if (g_car == NULL){
        return;
    }   
    g_car->check_distance();
}

void read_instruction()
{
    if(g_controller==NULL){
        return ;
    }

    g_controller->read_instruction();
}
