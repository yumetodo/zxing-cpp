/*
 *  ReedSolomonTest.cpp
 *  zxing
 *
 *  Created by Christian Brunschen on 06/05/2008.
 *  Copyright 2008 Google UK. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ReedSolomonTest.h"
#include <zxing/common/reedsolomon/ReedSolomonDecoder.h>
#include <zxing/common/reedsolomon/ReedSolomonException.h>
#include <zxing/common/IllegalArgumentException.h>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <limits>
#include "../cpp11_rand.h"

namespace zxing {
using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(ReedSolomonTest);

static inline ArrayRef<int> makeVector(int n, int *items) {
  ArrayRef<int> v(new Array<int>(items, n));
  return v;
}

static int QR_CODE_TEST[] = { 0x10, 0x20, 0x0C, 0x56, 0x61, 0x80, 0xEC, 0x11, 0xEC,
                              0x11, 0xEC, 0x11, 0xEC, 0x11, 0xEC, 0x11, 0xA5
                            };
static int QR_CODE_TEST_WITH_EC[] = { 0x10, 0x20, 0x0C, 0x56, 0x61, 0x80, 0xEC, 0x11, 0xEC,
                                      0x11, 0xEC, 0x11, 0xEC, 0x11, 0xEC, 0x11, 0xA5, 0x24,
                                      0xD4, 0xC1, 0xED, 0x36, 0xC7, 0x87, 0x2C, 0x55
                                    };

void ReedSolomonTest::setUp() {
  qrCodeTest_ =
    makeVector(sizeof(QR_CODE_TEST) / sizeof(QR_CODE_TEST[0]),
               QR_CODE_TEST);
  qrCodeTestWithEc_ =
    makeVector(sizeof(QR_CODE_TEST_WITH_EC) / sizeof(QR_CODE_TEST_WITH_EC[0]),
               QR_CODE_TEST_WITH_EC);
  qrCodeCorrectable_ =
    (qrCodeTestWithEc_->size() - qrCodeTest_->size()) / 2;

  qrRSDecoder_ = new ReedSolomonDecoder(GenericGF::QR_CODE_FIELD_256);
}

void ReedSolomonTest::tearDown() {
  delete qrRSDecoder_;
}

void ReedSolomonTest::testNoError() {
  ArrayRef<int> received(new Array<int>(qrCodeTestWithEc_->size()));
  *received = *qrCodeTestWithEc_;
  checkQRRSDecode(received);
}

void ReedSolomonTest::testOneError() {
	ArrayRef<int> received(new Array<int>(qrCodeTestWithEc_->size()));
	auto engine = create_random_engine(0xDEADBEEF);
	std::uniform_int_distribution<int> distribution(0, 255);
	for (int i = 0; i < received->size(); i++) {
		*received = *qrCodeTestWithEc_;
		received[i] = distribution(engine);
		checkQRRSDecode(received);
	}
}

void ReedSolomonTest::testMaxErrors() {
  ArrayRef<int> received(new Array<int>(qrCodeTestWithEc_->size()));
  for (int i = 0; i < qrCodeTest_->size(); i++) {
    *received = *qrCodeTestWithEc_;
    corrupt(received, qrCodeCorrectable_);
    checkQRRSDecode(received);
  }
}

void ReedSolomonTest::testTooManyErrors() {
  ArrayRef<int> received(new Array<int>(qrCodeTestWithEc_->size()));
  *received = *qrCodeTestWithEc_;
  try {
    corrupt(received, qrCodeCorrectable_ + 1);
    checkQRRSDecode(received);
    cout << "expected exception!\n";
    CPPUNIT_FAIL("should not happen!");
  } catch (ReedSolomonException const&) {
    // expected
  } catch (...) {
    CPPUNIT_FAIL("unexpected exception!");
  }
}


void ReedSolomonTest::checkQRRSDecode(ArrayRef<int> &received) {
  int twoS = 2 * qrCodeCorrectable_;
  qrRSDecoder_->decode(received, twoS);
  for (int i = 0; i < qrCodeTest_->size(); i++) {
    CPPUNIT_ASSERT_EQUAL(qrCodeTest_[i], received[i]);
  }
}

void ReedSolomonTest::corrupt(ArrayRef<int> &received, int howMany) {
	vector<bool> corrupted(received->size());
	auto engine = create_random_engine(0xDEADBEEF);
	std::uniform_real_distribution<double> distribution_f(0.0);
	std::uniform_int_distribution<int> distribution_i(0, 255);
	for (int j = 0; j < howMany; j++) {
		const double r = distribution_f(engine);
		int location = static_cast<int>(floor((1.0 == r) ? received->size() * (r - 1) : received->size() * r));
		if (corrupted[location]) {
			j--;
		}
		else {
			corrupted[location] = true;
			int newByte = distribution_i(engine);
			received[location] = newByte;
		}
	}
}

}
