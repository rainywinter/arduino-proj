#include <SoftwareSerial.h>
#include <TaskScheduler.h>

struct MoveController
{
    uint8_t pinLeftForward;
    uint8_t pinLeftBackward;
    uint8_t pinRightForwad;
    uint8_t pinRightBackward;
    uint8_t pinLeftSpeed;
    uint8_t pinRightSpeed;
    uint8_t speed; // percent, 0~10;
} mc = {4, 5, 7, 6, 10, 11, 0};

void setupMC()
{
    pinMode(mc.pinLeftForward, OUTPUT);
    pinMode(mc.pinLeftBackward, OUTPUT);
    pinMode(mc.pinRightBackward, OUTPUT);
    pinMode(mc.pinRightForwad, OUTPUT);
    pinMode(mc.pinLeftSpeed, OUTPUT);
    pinMode(mc.pinRightSpeed, OUTPUT);
}

struct Distancer
{
    uint8_t pinTrigger;
    uint8_t pinEcho;
    long duration; // micro second
    int distance;  // cm
} distancer = {2, 3, 0, 0};
void setupDistancer()
{
    pinMode(distancer.pinTrigger, OUTPUT);
    pinMode(distancer.pinEcho, INPUT);
}

struct Buzzer
{
    uint8_t pinBuzzer;
} buzzer = {12};
void setupBuzzer()
{
    pinMode(buzzer.pinBuzzer, OUTPUT);
}

struct Bluetoother
{
    // do not need to pin mode
    SoftwareSerial ss_bt;
    char readCh;
} bt = {SoftwareSerial(8, 9),''};

// void checkDistance();
// void readBT();

Task distanceTask(10, TASK_FOREVER, &checkDistance);
Task btTask(5, TASK_FOREVER, &readBT);
Scheduler runner;

void setupScheduler()
{
    // Scheduler.startLoop(checkDistance);
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
    bts.begin(9600);

    setupDistancer();
    setupBuzzer();
    setupMC();

    setupScheduler();
}

void loop()
{
    runner.execute();
}

void checkDistance()
{

    digitalWrite(trigger, LOW);
    delayMicroseconds(5);

    digitalWrite(trigger, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigger, LOW);

    duration = pulseIn(echo, HIGH);
    distance = duration * 0.034 / 2;
    // Serial.println("distance: ",distance);

    if (0 < distance && distance <= 30)
    {
        digitalWrite(buzzer, HIGH);
    }
    else
    {
        digitalWrite(buzzer, LOW);
    }
}

void readBT()
{
    if (bt.ss_bt.available() > 0)
    {
        bt.readCh = bt.ss_bt.read();
        Serial.print("bt read:");
        Serial.println(bt.readCh, HEX);
        runCMD(btChar);
    }
}

void runCMD(char c)
{
    Serial.println("press :", c);
    stop();
    switch (c)
    {
    case 'A':
        turnLeft();
        break;

    case 'D':
        turnRight();
        break;

    case 'W':
        moveForward();
        break;

    case 'S':
        moveBackward();
        break;
    case 'H':
        speedUp();
        break;

    case 'L':
        speedDown();
        break;

    default:
        Serial.println("unsupport cmd.");
        break;
    }
}

void moveForward()
{
    digitalWrite(mc.pinLeftForward, HIGH);
    digitalWrite(mc.pinRightForwad, HIGH);
}

void moveBackward()
{
    digitalWrite(mc.pinLeftBackward, HIGH);
    digitalWrite(mc.pinRightBackward, HIGH);
}

void turnLeft()
{
    uint8_t fast = (mc.speed * 255) / 10;
    uint8_t slow = fast - 75 > 0 ? fast - 75 : 0;
    analogWrite(mc.pinLeftSpeed, slow);
    analogWrite(mc.pinRightSpeed, fast);
}
void turnRight()
{
    uint8_t fast = (mc.speed * 255) / 10;
    uint8_t slow = fast - 75 > 0 ? fast - 75 : 0;
    analogWrite(mc.pinLeftSpeed, fast);
    analogWrite(mc.pinRightSpeed, fast);
}

void speedUp()
{
    mc.speed++;
    if (mc.speed > 10)
    {
        mc.speed = 10;
    }
    uint8_t val = (mc.speed * 255) / 10;
    analogWrite(mc.pinLeftSpeed, val);
    analogWrite(mc.pinRightSpeed, val);
}

void speedDown()
{
    mc.speed--;
    if (mc.speed < 0)
    {
        mc.speed = 0;
    }
    uint8_t val = (mc.speed * 255) / 10;
    analogWrite(mc.pinLeftSpeed, val);
    analogWrite(mc.pinRightSpeed, val);
}

void stop()
{
    digitalWrite(mc.pinLeftForward, LOW);
    digitalWrite(mc.pinLeftBackward, LOW);
    digitalWrite(mc.pinRightForwad, LOW);
    digitalWrite(mc.pinRightBackward, LOW);
    analogWrite(mc.pinLeftSpeed, 0);
    analogWrite(mc.pinRightSpeed, 0);
}
