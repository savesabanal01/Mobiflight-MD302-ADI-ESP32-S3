#include "MD302_ADI.h"
#include "allocateMem.h"
#include "commandmessenger.h"
#include <TFT_eSPI.h>
#include <SPI.h>
#include <pitch_line_long.h>
#include <pitch_line_short.h>
#include <ball.h>
#include <plane_indicator.h>
#include <roll_indicator.h>
#include <roll_scale.h>
#include <slip_indicator.h>
#include <pitch_scale.h>
#include <chevron_down.h>
#include <chevron_up.h>
#include <B612Font18.h>
#include <ssFont.h>
#include <logo.h>

#define digits ssFont
#define PANEL_COLOR 0x7BEE

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

TFT_eSprite AttitudeIndSpr = TFT_eSprite(&tft);      // Sprite to hold attitude Indicator
TFT_eSprite AttitudeIndBackSpr = TFT_eSprite(&tft);  // Sprite to hold attitude Indicator to fit only 240x320
TFT_eSprite AttitudeIndBackSpr2 = TFT_eSprite(&tft); // Sprite to hold attitude Indicator to fit only 240x320
TFT_eSprite planeSpr = TFT_eSprite(&tft);            // Plane Indicator Sprite
TFT_eSprite rollScaleSpr = TFT_eSprite(&tft);        // Roll Scale Sprite
TFT_eSprite pitchScaleSpr = TFT_eSprite(&tft);       // Pitch Scale Sprite
TFT_eSprite rollIndicatorSpr = TFT_eSprite(&tft);    // Roll Indicator Sprite
TFT_eSprite slipIndicatorSpr = TFT_eSprite(&tft);    // Slip Indicator Sprite
TFT_eSprite ballSpr = TFT_eSprite(&tft);             // Ball Sprite
TFT_eSprite chevronUpSpr = TFT_eSprite(&tft);        // Chevron pointing up
TFT_eSprite chevronDownSpr = TFT_eSprite(&tft);      // Chevron pointing down

#define BROWN 0x80C3    // 0x5960
#define SKY_BLUE 0x255C // 0x0318 //0x039B //0x34BF

/* **********************************************************************************
    This is just the basic code to set up your custom device.
    Change/add your code as needed.
********************************************************************************** */

MD302_ADI::MD302_ADI(uint8_t Pin1, uint8_t Pin2)
{
    _pin1 = Pin1;
    _pin2 = Pin2;
}

void MD302_ADI::begin()
{

  Serial.begin(115200);
  tft.begin();
  tft.setRotation(screenRotation);
  tft.fillScreen(PANEL_COLOR);
  tft.setPivot(320, 160);
  tft.setSwapBytes(true);
  tft.pushImage(40, 80, 160, 160, logo);
  delay(3000);
  tft.fillScreen(TFT_BLACK);
  
  AttitudeIndSpr.createSprite(400, 400);
  AttitudeIndSpr.setSwapBytes(false);
  AttitudeIndSpr.fillSprite(TFT_BLACK);
  AttitudeIndSpr.setPivot(200, 200);

  pitchScaleSpr.createSprite(123, 148);
  pitchScaleSpr.setSwapBytes(false);
  pitchScaleSpr.fillSprite(TFT_BLACK);

  rollScaleSpr.createSprite(rollScaleWidth, rollScaleHeight);
  rollScaleSpr.setSwapBytes(false);
  rollScaleSpr.fillSprite(TFT_BLACK);

  rollIndicatorSpr.createSprite(rollIndicatorWidth, rollIndicatorHeight);
  rollIndicatorSpr.setSwapBytes(false);
  rollIndicatorSpr.fillSprite(TFT_BLACK);

  // Draw the slip indicator sprite
  slipIndicatorSpr.createSprite(slipIndicatorWidth, slipIndicatorHeight);
  slipIndicatorSpr.setSwapBytes(false);
  slipIndicatorSpr.fillSprite(TFT_BLACK);

  planeSpr.createSprite(planeIndicatorWidth, planeIndicatorHeight);
  planeSpr.setSwapBytes(true);
  planeSpr.fillSprite(TFT_BLACK);

  // Create the sprites to hold the red chevron that points up
  chevronUpSpr.createSprite(76, 36);
  chevronUpSpr.setSwapBytes(false);
  chevronUpSpr.fillSprite(TFT_BLACK);
  chevronUpSpr.pushImage(0, 0, 76, 36, chevron_up);

  // Create the sprites to hold the red chevron that points down
  chevronDownSpr.createSprite(76, 36);
  chevronDownSpr.setSwapBytes(false);
  chevronDownSpr.fillSprite(TFT_BLACK);
  chevronDownSpr.pushImage(0, 0, 76, 36, chevron_down);

}

void MD302_ADI::attach(uint16_t Pin3, char *init)
{
    _pin3 = Pin3;
}

void MD302_ADI::detach()
{
    if (!_initialised)
        return;
    _initialised = false;
}

void MD302_ADI::set(int16_t messageID, char *setPoint)
{
    /* **********************************************************************************
        Each messageID has it's own value
        check for the messageID and define what to do.
        Important Remark!
        MessageID == -2 will be send from the board when PowerSavingMode is set
            Message will be "0" for leaving and "1" for entering PowerSavingMode
        MessageID == -1 will be send from the connector when Connector stops running
        Put in your code to enter this mode (e.g. clear a display)

    ********************************************************************************** */
    // int32_t  data = atoi(setPoint);
    // uint16_t output;

    // do something according your messageID
    switch (messageID) {
    case -1:
        setPowerSaveMode(true);
        // tbd., get's called when Mobiflight shuts down
        break;
    case -2:
        if(atoi(setPoint) == 1)
          setPowerSaveMode(true);
        else setPowerSaveMode(false);
        // tbd., get's called when PowerSavingMode is entered
        break;
    case 0:
        // output = (uint16_t)data;
        // data   = output;
        setPitch(atof(setPoint));
        break;
    case 1:
        /* code */
        setRoll(atof(setPoint));
        break;
    case 2:
        /* code */
        setSlipAngle(atof(setPoint));
        break;
    case 3:
        /* code */
        setInstrumentBrightnessRatio(atof(setPoint));
        break;
    case 100:
        /* code */
        setScreenRotation(atoi(setPoint));
        break;
    default:
        break;
    }
}

void MD302_ADI::update()
{

    // Do something which is required regulary
  // if(!powerSaveFlag)
  // {
  //   analogWrite(TFT_BL, instrumentBrightness);
  //   if(prevScreenRotation != screenRotation)
  //   {
  //       tft.setRotation(screenRotation);
  //       prevScreenRotation = screenRotation;
  //   }
  //   drawAttitudeIndicator();
  //  }
  //  else digitalWrite(TFT_BL, LOW);
  
   analogWrite(TFT_BL, instrumentBrightness);
   if(prevScreenRotation != screenRotation)
    {
        tft.setRotation(screenRotation);
        prevScreenRotation = screenRotation;
    }
   drawAttitudeIndicator();

}

void MD302_ADI::drawAttitudeIndicator()
{
  int i = 0;
  // Do something which is required regulary
  // Implement the "smooth flip" when the pitch is 90 or -90 or 270 or -270 degrees
  if (round(pitch) > -90 && round(pitch) < 90)
  {
    newRoll = roll;
    drawAll();
  }
  else if (round(pitch) == 90)
  {
    for (i = 0; i <= 180; i += 30)
    {
      newRoll = roll + i;
      drawAll();
    }
  }
  else if (round(pitch) > 90 && round(pitch) < 270)
  {
    newRoll = roll + 180;
    drawAll();
  }
  else if (round(pitch) == 270)
  {
    for (i = 0; i <= 180; i += 30)
    {
      // newRoll = roll + i;
      newRoll = roll + 180 - i;
      drawAll();
    }
  }
  else if (round(pitch) > 270 && round(pitch) <= 360)
  {
    newRoll = roll;
    drawAll();
  }
  else if (round(pitch) == -90)
  {
    for (i = 0; i <= 180; i += 30)
    {
      // newRoll = roll + i;
      newRoll = roll - i;
      drawAll();
    }
  }
  else if (round(pitch) < -90 && round(pitch) > -270)
  {
    newRoll = roll - 180;
    drawAll();
  }
  else if (round(pitch) == -270)
  {
    for (i = 0; i <= 180; i += 30)
    {
      // newRoll = roll + i;
      newRoll = roll + 180 + i;
      drawAll();
    }
  }
  else if ((round(pitch) < -270 && round(pitch) >= -360))
  {
    newRoll = roll;
    drawAll();
  }

  // Calcualte the new pitch when the pitch is more than 90 or more than 270 or less that -90 or less -270 degrees
  // because it just shows a "flipped" 80 degrees instead of 110 degrees, for example
  if (pitch >= -90 && pitch <= 90)
    newPitch = pitch;
  else if (pitch > 90 & pitch <= 270)
    newPitch = 90 - (pitch - 90);
  else if (pitch > 270 & pitch <= 360)
    newPitch = pitch - 360;
  else if (pitch < -90 & pitch >= -270)
    newPitch = -90 - (pitch + 90);
  else if (pitch < -270 && pitch >= -360)
    newPitch = pitch + 360;

  pitchPosition = scaleValue(newPitch, -45, 45, 0, 320);
}

void MD302_ADI::drawAll()
{

  // Draw main sprite that holds the sky and ground
  AttitudeIndSpr.fillRect(0, 0, 400, pitchPosition + 40, SKY_BLUE);
  AttitudeIndSpr.fillRect(0, pitchPosition + 40, 400, 400, BROWN);
  AttitudeIndSpr.fillRect(0, pitchPosition + 40 - 2, 400, 4, TFT_WHITE);
  AttitudeIndSpr.setPivot(200, 200);

  // Draw the pitch scale sprite

  drawPitchScale(pitch);
  pitchScaleSpr.pushToSprite(&AttitudeIndSpr, 59 + 80, 88 + 40, TFT_BLACK);

  // Draw the roll scale sprite
  rollScaleSpr.pushImage(0, 0, rollScaleWidth, rollScaleHeight, roll_scale);
  rollScaleSpr.setSwapBytes(true);
  rollScaleSpr.pushToSprite(&AttitudeIndSpr, 17 + 80, 42 + 40, TFT_BLACK);
  rollScaleSpr.fillSprite(TFT_BLACK);

  slipIndicatorSpr.pushImage(0, 0, slipIndicatorWidth, slipIndicatorHeight, slip_indicator);
  // Draw the the ball. The degree of is -8 to 8 degrees according to the sim values by trial and error

  drawBall();
  slipIndicatorSpr.setSwapBytes(true);

  slipIndicatorSpr.setPivot(slipIndicatorWidth / 2, -135);
  slipIndicatorSpr.pushRotated(&AttitudeIndSpr, newRoll * -1.0, TFT_BLACK);

  // Draw the roll indicator sprite

  rollIndicatorSpr.setSwapBytes(true);
  rollIndicatorSpr.pushImage(0, 0, rollIndicatorWidth, rollIndicatorHeight, roll_indicator);
  rollIndicatorSpr.setPivot(rollIndicatorWidth / 2, 90);
  rollIndicatorSpr.pushRotated(&AttitudeIndSpr, newRoll * -1.0, TFT_RED);

  // Draw the plane indicator
  planeSpr.pushImage(0, 0, planeIndicatorWidth, planeIndicatorHeight, plane_indicator);
  planeSpr.setPivot(planeIndicatorWidth / 2, 5);
  planeSpr.pushRotated(&AttitudeIndSpr, newRoll * -1.0, TFT_BLACK);

  tft.setViewport(0, 0, 240, 320);
  tft.setSwapBytes(false);
  tft.setPivot(120, 160);
  AttitudeIndSpr.pushRotated(newRoll, TFT_BLACK);
  pitchScaleSpr.fillSprite(TFT_BLACK);
  AttitudeIndSpr.fillScreen(TFT_BLACK);
}

void MD302_ADI::drawPitchScale(float pitch)
{
  // Draw the pitch scale
  float startPitch = 0, endPitch = 0;
  long pitchAngle, pitchLinePos, angleIncrement;

  startPitch = newPitch - 20;
  endPitch = newPitch + 20;
  for (angleIncrement = startPitch; angleIncrement <= endPitch; angleIncrement++)
  {

    pitchLinePos = scaleValue(angleIncrement, endPitch, startPitch, 0, 141); // scale the angles to the number of pixels
    pitchAngle = round(angleIncrement);

    if ((pitchAngle % 5 == 0) && pitchAngle >= -40 && pitchAngle <= 40 && (pitchAngle % 10) != 0)
    {
      pitchScaleSpr.setSwapBytes(true);
      // pitchScaleSpr.drawWideLine(45, pitchLinePos, 45 + pitchLineShortWidth, pitchLinePos, 4, TFT_WHITE, TFT_WHITE);
      pitchScaleSpr.fillRect(45, pitchLinePos, pitchLineShortWidth, 4, TFT_WHITE);
    }

    if ((pitchAngle % 10) == 0) // draw long pitch line and numbers
    {
      pitchScaleSpr.setSwapBytes(true);
      // pitchScaleSpr.drawWideLine(23, pitchLinePos, pitchLineLongWidth + 20, pitchLinePos, 4, TFT_WHITE, TFT_WHITE);
      pitchScaleSpr.fillRect(23, pitchLinePos, pitchLineLongWidth, 4, TFT_WHITE);
      pitchScaleSpr.loadFont(digits);
      pitchScaleSpr.setTextColor(TFT_WHITE);
      if (pitchAngle != 0)
      {
        if (abs(pitchAngle) > 90)
          pitchAngle = 90 - (abs(pitchAngle) - 90);
        pitchScaleSpr.setSwapBytes(true);
        pitchScaleSpr.drawString(String(abs(pitchAngle)), 1, pitchLinePos - 5);
        pitchScaleSpr.drawString(String(abs(pitchAngle)), 101, pitchLinePos - 5);
      }
      // Draw chevron pointing down to horizon if the angle is 50 or 70 or 90
      if (pitchAngle == 50 || pitchAngle == 70 || pitchAngle == 90)
      {
        pitchScaleSpr.setSwapBytes(true);
        // pitchScaleSpr.pushImage(23, pitchLinePos - 20, 76, 36, chevron_down);
        chevronDownSpr.pushToSprite(&pitchScaleSpr, 23, pitchLinePos - 20, TFT_BLACK);
      }
      // Draw chevron pointing up to horizon if the angle is -50 or -70 or -90
      else if (pitchAngle == -50 || pitchAngle == -70 || pitchAngle == -90)
      {
        pitchScaleSpr.setSwapBytes(true);
        chevronUpSpr.pushToSprite(&pitchScaleSpr, 23, pitchLinePos - 20, TFT_BLACK);
        // pitchScaleSpr.pushImage(23, pitchLinePos - 20, 76, 36, chevron_up);
      }
    }
  }
}

void MD302_ADI::drawBall()
{
  float ballPos = 0;

  ballSpr.createSprite(ballWidth, ballHeight);
  ballSpr.fillSprite(TFT_BLACK);
  ballPos = scaleValue(slipAngle, -8, 8, 0, 79); // scale the angles to the number of pixels
  ballSpr.pushImage(0, 0, ballWidth, ballHeight, ball);
  ballSpr.setSwapBytes(false);
  ballSpr.pushToSprite(&slipIndicatorSpr, ballPos, 0, TFT_BLACK);
}

void MD302_ADI::setPitch(float value)
{
  pitch = value;
}

void MD302_ADI::setRoll(float value)
{
  roll = value * -1.0;
}

void MD302_ADI::setSlipAngle(float value)
{
  slipAngle = value * -1.0;
}

void MD302_ADI::setInstrumentBrightnessRatio(float value)
{
  instrumentBrightnessRatio = value;
  instrumentBrightness = (int)scaleValue(instrumentBrightnessRatio, 0, 1, 0, 255);
}

void MD302_ADI::setScreenRotation(int value)
{
  screenRotation = value;
}

void MD302_ADI::setPowerSaveMode(bool enabled)
{
    if(enabled)
    {
        digitalWrite(TFT_BL, LOW);
        powerSaveFlag = true;
    }
    else
    {
        analogWrite(TFT_BL, instrumentBrightness);
        powerSaveFlag = false;
    }
}

// Scale function
float MD302_ADI::scaleValue(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

