// Copyright 2026 Can Joshua Lehmann
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "bitstring.hpp"

#include "../unittest.cpp/unittest.hpp"

using namespace bitstring;

int main(int argc, char** argv) {
  unittest::Suite suite(argc, argv);

  suite.test("constructor").run([]() {
    BitString4 a(100);
    BitString4 b(100);
    unittest_assert(a == b);
  });

  suite.test("operator~").run([]() {
    unittest_assert(~BitString4("01xz") == BitString4("10xx"));
  });

  suite.test("operator|").run([]() {
    unittest_assert((BitString4("01xz01xz01xz01xz") | BitString4("00001111xxxxzzzz")) == BitString4("01xx1111x1xxx1xx"));
  });

  suite.test("operator&").run([]() {
    unittest_assert((BitString4("01xz01xz01xz01xz") & BitString4("00001111xxxxzzzz")) == BitString4("000001xx0xxx0xxx"));
  });

  suite.test("operator^").run([]() {
    unittest_assert((BitString4("01xz01xz01xz01xz") ^ BitString4("00001111xxxxzzzz")) == BitString4("01xx10xxxxxxxxxx"));
  });

  suite.test("eq").run([]() {
    unittest_assert(BitString4("010").eq(BitString4("010")) == Bit4::One);
    unittest_assert(BitString4("010").eq(BitString4("011")) == Bit4::Zero);
    unittest_assert(BitString4("0x0").eq(BitString4("010")) == Bit4::X);
    unittest_assert(BitString4("010").eq(BitString4("0z0")) == Bit4::X);
    unittest_assert(BitString4("1x0").eq(BitString4("010")) == Bit4::X);
    unittest_assert(BitString4::ones(100).eq(BitString4::ones(100)) == Bit4::One);
    unittest_assert(BitString4::ones(100).eq(BitString4(100)) == Bit4::Zero);
  });

  suite.test("lt_u").run([]() {
    unittest_assert(BitString4("001").lt_u(BitString4("010")) == Bit4::One);
    unittest_assert(BitString4("010").lt_u(BitString4("001")) == Bit4::Zero);
    unittest_assert(BitString4("010").lt_u(BitString4("010")) == Bit4::Zero);
    unittest_assert(BitString4("001").lt_u(BitString4("100")) == Bit4::One);
    unittest_assert(BitString4("0x1").lt_u(BitString4("010")) == Bit4::X);
    unittest_assert(BitString4("001").lt_u(BitString4("01z")) == Bit4::X);
    unittest_assert(BitString4(100).lt_u(BitString4::ones(100)) == Bit4::One);
    unittest_assert(BitString4(100).lt_u(BitString4::fully(Bit4::X, 100)) == Bit4::X);
  });

  suite.test("lt_s").run([]() {
    unittest_assert(BitString4("100").lt_s(BitString4("001")) == Bit4::One);
    unittest_assert(BitString4("001").lt_s(BitString4("100")) == Bit4::Zero);
    unittest_assert(BitString4("111").lt_s(BitString4("000")) == Bit4::One);
    unittest_assert(BitString4("001").lt_s(BitString4("010")) == Bit4::One);
    unittest_assert(BitString4("010").lt_s(BitString4("010")) == Bit4::Zero);
    unittest_assert(BitString4("x00").lt_s(BitString4("001")) == Bit4::X);
    unittest_assert(BitString4("000").lt_s(BitString4("00z")) == Bit4::X);
    unittest_assert(BitString4::ones(100).lt_s(BitString4(100)) == Bit4::One);
  });

  return suite.finish();
}
