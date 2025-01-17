#include <SoftwareSerial.h>
#include <TaskScheduler.h>
#include <ArduinoLog.h>

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

    // Log.verboseln(F("Log mc: %p")  , mc );
}

struct Distancer
{
    uint8_t pinTrigger;
    uint8_t pinEcho;
    unsigned long duration; // micro second
    int16_t distance;  // cm
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
} bt = {SoftwareSerial(8, 9), '0'}; //(rx,tx);

void checkDistance();
void readBT();

Task distanceTask(5, TASK_FOREVER, &checkDistance);
Task btTask(5, TASK_FOREVER, &readBT);
Scheduler runner;

void setupScheduler()
{
    runner.init();
    runner.addTask(distanceTask);
    runner.addTask(btTask);
    distanceTask.enable();
    btTask.enable();
}

void setup()
{
    Serial.println("setup begin");
    // put your setup code here, to run once:
    Serial.begin(9600);
    bt.ss_bt.begin(9600);

    // while(!Serial && !Serial.available()){}
    // Log.begin(LOG_LEVEL_VERBOSE, &Serial);

    setupDistancer();
    setupBuzzer();
    setupMC();

    setupScheduler();

    Serial.println(mc.pinLeftSpeed);
    Serial.println(mc.pinRightSpeed);
    Serial.println("setup done");
}

void loop()
{
    runner.execute();
}

void checkDistance()
{

    digitalWrite(distancer.pinTrigger, LOW);
    delayMicroseconds(5);

    digitalWrite(distancer.pinTrigger, HIGH);
    delayMicroseconds(10);
    digitalWrite(distancer.pinTrigger, LOW);

    distancer.duration = pulseIn(distancer.pinEcho, HIGH);
    Serial.print("pin: ");
    Serial.println(distancer.pinEcho);
    Serial.print("duration: ");
    Serial.println(distancer.duration);
    distancer.distance = distancer.duration * 0.034 / 2;
    Serial.print("distance: ");
    Serial.println(distancer.distance);
    if (0 < distancer.distance && distancer.distance <= 30)
    {
        digitalWrite(buzzer.pinBuzzer, HIGH);
        stop();
    }
    else
    {
        digitalWrite(buzzer.pinBuzzer, LOW);
    }
}

void readBT()
{
    if (bt.ss_bt.available() > 0)
    {
        bt.readCh = bt.ss_bt.read();
        Serial.print("bt read:");
        Serial.println(bt.readCh, HEX);
        runCMD(bt.readCh);
    }
}

void runCMD(char c)
{
    Serial.print("press :");
    Serial.print(c);
    Serial.print("speed:");
    Serial.println(mc.speed);
    switch (c)
    {
    case 'A': // 0x41
        turnLeft();
        break;

    case 'D': // 0x44
        turnRight();
        break;

    case 'W': // 0x57
        moveForward();
        break;

    case 'S': // 0x53
        moveBackward();
        break;
    case 'H': // 0x48
        speedUp();
        break;

    case 'L': // 0x4c
        speedDown();
        break;

    default:
        Serial.println("unsupport cmd.");
        stop();
        break;
    }
}

void keepSpeed()
{
    uint8_t val = (mc.speed * 255) / 10;
    Serial.print("keep val");
    Serial.println(val);
    analogWrite(mc.pinLeftSpeed, val);
    analogWrite(mc.pinRightSpeed, val);
}

void moveForward()
{
    Serial.println("forward");
    digitalWrite(mc.pinLeftForward, HIGH);
    digitalWrite(mc.pinRightForwad, HIGH);
    digitalWrite(mc.pinLeftBackward, LOW);
    digitalWrite(mc.pinRightBackward, LOW);
    keepSpeed();
}

void moveBackward()
{
    Serial.println("backward");
    digitalWrite(mc.pinLeftForward, LOW);
    digitalWrite(mc.pinRightForwad, LOW);
    digitalWrite(mc.pinLeftBackward, HIGH);
    digitalWrite(mc.pinRightBackward, HIGH);

    keepSpeed();
}

void turnLeft()
{
    Serial.println("left");
    uint8_t fast = (mc.speed * 255) / 10;
    uint8_t slow = fast - 50 > 0 ? fast - 50 : 0;
    analogWrite(mc.pinLeftSpeed, slow);
    analogWrite(mc.pinRightSpeed, fast);
}
void turnRight()
{
    Serial.println("right");
    uint8_t fast = (mc.speed * 255) / 10;
    uint8_t slow = fast - 50 > 0 ? fast - 50 : 0;
    analogWrite(mc.pinLeftSpeed, fast);
    analogWrite(mc.pinRightSpeed, slow);
}

void speedUp()
{
    Serial.println("speedup");

    if (mc.speed >= 10)
    {
        mc.speed = 10;
    }
    else
    {
        mc.speed++;
    }
    uint8_t val = (mc.speed * 255) / 10;
    analogWrite(mc.pinLeftSpeed, val);
    analogWrite(mc.pinRightSpeed, val);
}

void speedDown()
{
    Serial.println("speeddown");
    if (mc.speed <= 0)
    {
        mc.speed = 0;
    }
    else
    {
        mc.speed--;
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
