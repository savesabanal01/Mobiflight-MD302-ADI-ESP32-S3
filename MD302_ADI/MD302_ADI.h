#pragma once

#include "Arduino.h"

class MD302_ADI
{
public:
    MD302_ADI(uint8_t Pin1, uint8_t Pin2);
    void begin();
    void attach(uint16_t Pin3, char *init);
    void detach();
    void set(int16_t messageID, char *setPoint);
    void update();

private:
    bool    _initialised;
    uint8_t _pin1, _pin2, _pin3;
    float pitch = 0;
    float roll = 0;
    float newRoll = 0;
    float pitchPosition = 0;
    float newPitch = 0;
    float slipAngle = 0; // slip angle value from sim
    float instrumentBrightnessRatio = 1;
    int instrumentBrightness = 255;
    int screenRotation = 2;
    int prevScreenRotation = 2;
    bool powerSaveMode = false;

    // Function declarations
    float scaleValue(float x, float in_min, float in_max, float out_min, float out_max);
    void drawAll();
    void drawBall();
    void drawAttitudeIndicator();
    void drawPitchScale(float pitch);
    void setPitch(float value);
    void setRoll(float value);
    void setSlipAngle(float value);
    void setInstrumentBrightnessRatio(float value);
    void setScreenRotation (int value);
    void setPowerSaveMode(bool mode);
};