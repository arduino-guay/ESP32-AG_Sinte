//
//  ADSR.cpp
//
//  Created by Nigel Redmon on 12/18/12.
//  EarLevel Engineering: earlevel.com
//  Copyright 2012 Nigel Redmon
//
//  For a complete explanation of the ADSR envelope generator and code,
//  read the series of articles by the author, starting here:
//  http://www.earlevel.com/main/2013/06/01/envelope-generators/
//
//  License:
//
//  This source code is provided as is, without warranty.
//  You may copy and distribute verbatim copies of this document.
//  You may modify and use this source code to create binary code for your own purposes, free or commercial.
//
//  1.01  2016-01-02  njr   added calcCoef to SetTargetRatio functions that were in the ADSR widget but missing in this code
//  1.02  2017-01-04  njr   in calcCoef, checked for rate 0, to support non-IEEE compliant compilers
//

#include "ADSR.h"
#include <Arduino.h>
#include <math.h>


ADSR::ADSR(void) {
    reset();
    setAttackRate(0);
    setDecayRate(0);
    setReleaseRate(0);
    setSustainLevel(1.0);
    setTargetRatioA(0.1);
    setTargetRatioDR(0.0001);
}

float IRAM_ATTR ADSR::process()
{
    switch (state)
    {
    case env_idle:
        break;
    case env_attack:
        output = attackBase + output * attackCoef;
        if (output >= 1.0)
        {
            output = 1.0;
            state = env_decay;
        }
        break;
    case env_decay:
        output = decayBase + output * decayCoef;
        if (output <= sustainLevel)
        {
            output = sustainLevel;
            state = env_sustain;
        }
        break;
    case env_sustain:
        break;
    case env_release:
        output = releaseBase + output * releaseCoef;
        if (output <= 0.0)
        {
            output = 0.0;
            state = env_idle;
        }
    }
    return output;
}

void ADSR::setAttackRate(float rate) {
    attackRate = rate;
    attackCoef = calcCoef(rate, targetRatioA);
    attackBase = (1.0 + targetRatioA) * (1.0 - attackCoef);
}

void ADSR::setDecayRate(float rate) {
    decayRate = rate;
    decayCoef = calcCoef(rate, targetRatioDR);
    decayBase = (sustainLevel - targetRatioDR) * (1.0 - decayCoef);
}

void ADSR::setReleaseRate(float rate) {
    releaseRate = rate;
    releaseCoef = calcCoef(rate, targetRatioDR);
    releaseBase = -targetRatioDR * (1.0 - releaseCoef);
}

float ADSR::calcCoef(float rate, float targetRatio) {
    return (rate <= 0) ? 0.0 : exp(-log((1.0 + targetRatio) / targetRatio) / rate);
}

void ADSR::setSustainLevel(float level) {
    sustainLevel = level;
    decayBase = (sustainLevel - targetRatioDR) * (1.0 - decayCoef);
}

void ADSR::setAll(float aRate, float dRate, float sLevel, float rRate)
{
    setAttackRate(aRate);
    setDecayRate(dRate);
    setSustainLevel(sLevel);
    setReleaseRate(rRate);
}

void ADSR::setAll(adsrParam* param)
{
    setAttackRate(param->a);
    setDecayRate(param->d);
    setSustainLevel(param->s);
    setReleaseRate(param->r);
}

void ADSR::setTargetRatioA(float targetRatio) {
    if (targetRatio < 0.000001)
        targetRatio = 0.000001; 
    targetRatioA = targetRatio;
    attackCoef = calcCoef(attackRate, targetRatioA);
    attackBase = (1.0 + targetRatioA) * (1.0 - attackCoef);
}

void ADSR::setTargetRatioDR(float targetRatio) {
    if (targetRatio < 0.000001)
        targetRatio = 0.000001; 
    targetRatioDR = targetRatio;
    decayCoef = calcCoef(decayRate, targetRatioDR);
    releaseCoef = calcCoef(releaseRate, targetRatioDR);
    decayBase = (sustainLevel - targetRatioDR) * (1.0 - decayCoef);
    releaseBase = -targetRatioDR * (1.0 - releaseCoef);
}

void ADSR::gate(int gate)
{
    if (gate) {
        state = env_attack;
    }
    else if (state != env_idle) {
        state = env_release;
    }
}

void ADSR::reset()
{
    state = env_idle;
    output = 0.0;
}

void ADSR::retrigger()
{
    reset();
    gate(true);
}
