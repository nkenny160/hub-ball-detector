#include "ctre/phoenix6/CANrange.hpp"
#include "RobotBase.hpp"
#include "lcd_i2c.hpp"
#include <cstdio>
#include <iostream>

using namespace ctre::phoenix6;

/* Distance (in meters) below which the CANrange triggers a detection */
static constexpr double DETECT_THRESHOLD_M = 0.3;

/**
 * This is the main robot. Put all actuators, sensors,
 * game controllers, etc. in this class.
 */
class Robot : public RobotBase {
private:
    /* This can be a CANivore name, CANivore serial number,
     * SocketCAN interface, or "*" to select any CANivore. */
    static constexpr CANBus CANBUS{"*"};


    /* CANrange sensor on device ID 4 */
    
    hardware::CANrange canrange1{1, CANBUS};
    hardware::CANrange canrange2{2, CANBUS};
    hardware::CANrange canrange3{3, CANBUS};
    hardware::CANrange canrange4{4, CANBUS};

    /* I2C LCD display (0x27 = default QA Pass backpack address) */
    LCD_I2C lcd{0x27, "/dev/i2c-1"};

    /* detection counter and edge-detection state */
    int detectionCount{0};
    bool wasDetected{false};


public:
    /* main robot interface */
    void RobotInit() override;
    void RobotPeriodic() override;

    bool IsEnabled() override;
    void EnabledInit() override;
    void EnabledPeriodic() override;

    void DisabledInit() override;
    void DisabledPeriodic() override;
};

/**
 * Runs once at code initialization.
 */
void Robot::RobotInit()
{
}

/**
 * Runs periodically during program execution.
 */
void Robot::RobotPeriodic()
{

    /* read distance from CANrange (meters) */
    
    double distance1 = canrange1.GetDistance().GetValueAsDouble();
    double distance2 = canrange2.GetDistance().GetValueAsDouble();
    double distance3 = canrange3.GetDistance().GetValueAsDouble();
    double distance4 = canrange4.GetDistance().GetValueAsDouble();
    
    bool isDetected1 = distance1 < DETECT_THRESHOLD_M;
    bool isDetected2 = distance2 < DETECT_THRESHOLD_M;
    bool isDetected3 = distance3 < DETECT_THRESHOLD_M;
    bool isDetected4 = distance4 < DETECT_THRESHOLD_M;


    bool isDetected = isDetected1 || isDetected2 || isDetected3 || isDetected4;

    /* increment counter on the rising edge of a detection */
    if (isDetected && !wasDetected) {
        ++detectionCount;
        std::cout << "Ball detected! Count: " << detectionCount << "\n";

        /* update LCD: line 0 = label, line 1 = count */
        
    }
    if (lcd.ok()) {
            char buf[17];
            lcd.setCursor(0, 0);
            lcd.print("  Ball Counter  ");
            lcd.setCursor(0, 1);
            std::snprintf(buf, sizeof(buf), "Count: %-9d", detectionCount);
            lcd.print(buf);
        }
    wasDetected = isDetected;
}

/**
 * Returns whether the robot should be enabled.
 */
bool Robot::IsEnabled()
{
}

/**
 * Runs when transitioning from disabled to enabled.
 */
void Robot::EnabledInit() {}

/**
 * Runs periodically while enabled.
 */
void Robot::EnabledPeriodic()
{
}

/**
 * Runs when transitioning from enabled to disabled,
 * including after robot startup.
 */
void Robot::DisabledInit() {}

/**
 * Runs periodically while disabled.
 */
void Robot::DisabledPeriodic()
{
}

/* ------ main function ------ */
int main()
{
    /* create and run robot */
    Robot robot{};
    // robot.SetLoopTime(20_ms); // optionally change loop time for periodic calls
    return robot.Run();
}
