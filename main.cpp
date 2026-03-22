#include "ctre/phoenix6/CANrange.hpp"
#include "RobotBase.hpp"
#include "oled_i2c.hpp"
#include <cstdio>
#include <iostream>

using namespace ctre::phoenix6;

/* Distance (in meters) below which the CANrange triggers a detection */
static constexpr double DETECT_THRESHOLD_M = 0.02;

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

    /* SSD1306 128x64 OLED over I2C (default address 0x3C) */
    OLED_I2C oled{0x3C, "/dev/i2c-1"};

    /* detection counter and edge-detection state */
    int detectionCount{0};

    bool wasDetected1{false};
    bool wasDetected2{false};
    bool wasDetected3{false};


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
    
    bool isDetected1 = distance1 < DETECT_THRESHOLD_M;
    bool isDetected2 = distance2 < DETECT_THRESHOLD_M;
    bool isDetected3 = distance3 < DETECT_THRESHOLD_M;

    /* increment counter on the rising edge of a detection */
    if (isDetected1 && !wasDetected1) {
        ++detectionCount;
        std::cout << "Ball detected 1! Count: " << detectionCount << "\n";

        /* update OLED display */
        if (oled.ok()) {
            oled.showCount(detectionCount);
        }
    }
    if (isDetected2 && !wasDetected2) {
        ++detectionCount;
        std::cout << "Ball detected 2! Count: " << detectionCount << "\n";

        /* update OLED display */
        if (oled.ok()) {
            oled.showCount(detectionCount);
        }
    }
    if (isDetected3 && !wasDetected3) {
        ++detectionCount;
        std::cout << "Ball detected 3! Count: " << detectionCount << "\n";

        /* update OLED display */
        if (oled.ok()) {
            oled.showCount(detectionCount);
        }
    }
    wasDetected1 = isDetected1;
    wasDetected2 = isDetected2;
    wasDetected3 = isDetected3;
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
