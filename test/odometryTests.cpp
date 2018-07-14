/**
 * @author Ryan Benasutti, WPI
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "test/tests/api/odometryTests.hpp"
#include "okapi/api/odometry/odometry.hpp"
#include "test/testRunner.hpp"
#include "test/tests/api/implMocks.hpp"
#include <memory>
#include <gtest/gtest.h>
#include "okapi/api/chassis/model/skidSteerModel.hpp"

using namespace okapi;
using namespace snowhouse;

TEST(OdometryTest, BasicTest) {
  class MockModel : public SkidSteerModel {
  public:
    MockModel() : SkidSteerModel(std::make_shared<MockMotor>(), std::make_shared<MockMotor>()) {
    }

    std::valarray<std::int32_t> getSensorVals() const override {
      return std::valarray<std::int32_t>{leftEnc, rightEnc};
    }

    void setSensorVals(std::int32_t left, std::int32_t right) {
      leftEnc = left;
      rightEnc = right;
    }

    std::int32_t leftEnc = 0;
    std::int32_t rightEnc = 0;
  };

  auto model = std::make_shared<MockModel>();
  Odometry odom(model, 143.239449, 16.875, std::make_unique<MockRate>());
}
