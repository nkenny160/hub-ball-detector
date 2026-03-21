#include "ctre/phoenix6/CANrange.hpp"
#include "RobotBase.hpp"
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
    hardware::CANrange canrange{4, CANBUS};

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
    /* periodically check that the joystick is still good */
    joy.Periodic();

    /* read distance from CANrange (meters) */
    double distance = canrange.GetDistance().GetValueAsDouble();
    bool isDetected = distance < DETECT_THRESHOLD_M;

    /* increment counter and print on the rising edge of a detection */
    if (isDetected && !wasDetected) {
        ++detectionCount;
        std::cout << "Ball detected! Count: " << detectionCount
                  << "  (distance: " << distance << " m)\n";
    }
    wasDetected = isDetected;
}

/**
 * Returns whether the robot should be enabled.
 */
bool Robot::IsEnabled()
{
    /* enable while joystick is an Xbox controller (6 axes),
     * and we are holding the right bumper */
    if (joy.GetNumAxes() < 6) return false;
    return joy.GetButton(5); // SDL_CONTROLLER_BUTTON_RIGHTSHOULDER
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
