#pragma once

#define _USE_MATH_DEFINES

#include <myo/myo.hpp>

class DataCollector: public myo::DeviceListener 
{
public:
    DataCollector();

    void onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose);
    void onArmSync(myo::Myo* myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection, float rotation, myo::WarmupState warmupState);
    void onArmUnsync(myo::Myo* myo, uint64_t timestamp);
    void onUnlock(myo::Myo* myo, uint64_t timestamp);
    void onLock(myo::Myo* myo, uint64_t timestamp);
    void onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat);
    void print();
    void GripperPose();
    void GripperMode();

    // These values are set by onArmSync() and onArmUnsync() above.
    bool onArm;
    myo::Arm whichArm;

    // This is set by onUnlocked() and onLocked() above.
    bool isUnlocked;

    // These values are set by onPose() above.
    myo::Pose currentPose;

    int MyPose;
    int roll_w;
    int pitch_w;
    int yaw_w;
};