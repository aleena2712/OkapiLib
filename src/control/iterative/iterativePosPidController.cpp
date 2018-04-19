/**
 * Based on the Arduino PID controller: https://github.com/br3ttb/Arduino-PID-Library
 *
 * @author Ryan Benasutti, WPI
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "okapi/control/iterative/iterativePosPidController.hpp"
#include "api.h"
#include <algorithm>
#include <cmath>

namespace okapi {
IterativePosPIDControllerArgs::IterativePosPIDControllerArgs(const double ikP, const double ikI,
                                                             const double ikD, const double ikBias)
  : kP(ikP), kI(ikI), kD(ikD), kBias(ikBias) {
}

IterativePosPIDController::IterativePosPIDController(const double ikP, const double ikI,
                                                     const double ikD, const double ikBias)
  : IterativePosPIDController(ikP, ikI, ikD, ikBias, std::make_unique<Timer>(),
                              std::make_shared<SettledUtil>()) {
}

IterativePosPIDController::IterativePosPIDController(const IterativePosPIDControllerArgs &params)
  : IterativePosPIDController(params.kP, params.kI, params.kD, params.kBias,
                              std::make_unique<Timer>(), std::make_shared<SettledUtil>()) {
}

IterativePosPIDController::IterativePosPIDController(const double ikP, const double ikI,
                                                     const double ikD, const double ikBias,
                                                     std::unique_ptr<Timer> iloopDtTimer,
                                                     std::shared_ptr<SettledUtil> isettledUtil)
  : loopDtTimer(std::move(iloopDtTimer)), settledUtil(std::move(isettledUtil)) {
  if (ikI != 0) {
    setIntegralLimits(-1 / ikI, 1 / ikI);
  }
  setOutputLimits(-1, 1);
  setGains(ikP, ikI, ikD, ikBias);
}

void IterativePosPIDController::setTarget(const double itarget) {
  target = itarget;
}

double IterativePosPIDController::getOutput() const {
  return output;
}

double IterativePosPIDController::getError() const {
  return error;
}

double IterativePosPIDController::getDerivative() const {
  return derivative;
}

bool IterativePosPIDController::isSettled() {
  return settledUtil->isSettled(error);
}

void IterativePosPIDController::setSampleTime(const std::uint32_t isampleTime) {
  if (isampleTime > 0) {
    const double ratio = static_cast<double>(isampleTime) / static_cast<double>(sampleTime);
    kI *= ratio;
    kD /= ratio;
    sampleTime = isampleTime;
  }
}

void IterativePosPIDController::setOutputLimits(double imax, double imin) {
  // Always use larger value as max
  if (imin > imax) {
    const double temp = imax;
    imax = imin;
    imin = temp;
  }

  outputMax = imax;
  outputMin = imin;

  output = std::clamp(output, outputMin, outputMax);

  // Fix integral
  setIntegralLimits(imax, imin);
}

void IterativePosPIDController::setIntegralLimits(double imax, double imin) {
  // Always use larger value as max
  if (imin > imax) {
    const double temp = imax;
    imax = imin;
    imin = temp;
  }

  integralMax = imax;
  integralMin = imin;

  integral = std::clamp(integral, integralMin, integralMax);
}

void IterativePosPIDController::setErrorSumLimits(const double imax, const double imin) {
  errorSumMax = imax;
  errorSumMin = imin;
}

double IterativePosPIDController::step(const double inewReading) {
  if (isOn) {
    loopDtTimer->placeHardMark();

    if (loopDtTimer->getDtFromHardMark() >= sampleTime) {
      error = target - inewReading;

      if ((fabs(error) < target - errorSumMin && fabs(error) > target - errorSumMax) ||
          (fabs(error) > target + errorSumMin && fabs(error) < target + errorSumMax)) {
        integral += kI * error; // Eliminate integral kick while realtime tuning
      }

      if (shouldResetOnCross && copysign(1.0, error) != copysign(1.0, lastError)) {
        integral = 0;
      }

      integral = std::clamp(integral, integralMin, integralMax);

      // Derivative over measurement to eliminate derivative kick on setpoint change
      derivative = inewReading - lastReading;

      output = std::clamp(kP * error + integral - kD * derivative + kBias, outputMin, outputMax);

      lastReading = inewReading;
      lastError = error;
      loopDtTimer->clearHardMark(); // Important that we only clear if dt >= sampleTime

      settledUtil->isSettled(error);
    }
  } else {
    output = 0; // Controller is off so write 0
  }

  return output;
}

void IterativePosPIDController::setGains(const double ikP, const double ikI, const double ikD,
                                         const double ikBias) {
  const double sampleTimeSec = static_cast<double>(sampleTime) / 1000.0;
  kP = ikP;
  kI = ikI * sampleTimeSec;
  kD = ikD * sampleTimeSec;
  kBias = ikBias;
}

void IterativePosPIDController::reset() {
  error = 0;
  lastError = 0;
  lastReading = 0;
  integral = 0;
  output = 0;
}

void IterativePosPIDController::setIntegratorReset(bool iresetOnZero) {
  shouldResetOnCross = iresetOnZero;
}

void IterativePosPIDController::flipDisable() {
  isOn = !isOn;
}

void IterativePosPIDController::flipDisable(const bool iisDisabled) {
  isOn = !iisDisabled;
}

bool IterativePosPIDController::isDisabled() const {
  return !isOn;
}

std::uint32_t IterativePosPIDController::getSampleTime() const {
  return sampleTime;
}
} // namespace okapi