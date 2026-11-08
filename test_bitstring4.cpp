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

  suite.test("from_bool").run([]() {
    unittest_assert(BitString4::from_bool(true) == BitString4("1"));
    unittest_assert(BitString4::from_bool(false) == BitString4("0"));
  });

  suite.test("random").run([]() {
    unittest_assert(BitString4::random(0).width() == 0);
    BitString4 value = BitString4::random(100);
    unittest_assert(value.width() == 100);
    unittest_assert(!value.has_unknown());
  });

  suite.test("mask_lower").run([]() {
    unittest_assert(BitString4::mask_lower(5, 3) == BitString4("00111"));
    unittest_assert(BitString4::mask_lower(5, 0) == BitString4("00000"));
    unittest_assert(BitString4::mask_lower(5, 9) == BitString4("11111"));
    unittest_assert(BitString4::mask_lower(128, 128) == BitString4::ones(128));
  });

  suite.test("concat").run([]() {
    unittest_assert(BitString4::concat(BitString4("1x"), BitString4("0z1")) == BitString4("1x0z1"));
    unittest_assert(BitString4::concat(BitString4::ones(70), BitString4(60)).slice(60, 70) == BitString4::ones(70));
    unittest_assert(BitString4::concat(BitString4(0), BitString4(0)).width() == 0);
  });

  suite.test("slice").run([]() {
    unittest_assert(BitString4("1x0z1101").slice(2, 4) == BitString4("0z11"));
    unittest_assert(BitString4("1x0z").slice(0, 4) == BitString4("1x0z"));
    unittest_assert(BitString4("1x0z").slice(1, 0).width() == 0);
  });

  suite.test("reverse").run([]() {
    unittest_assert(BitString4("1x0z").reverse() == BitString4("z0x1"));
  });

  suite.test("repeat").run([]() {
    unittest_assert(BitString4("1xz").repeat(2) == BitString4("1xz1xz"));
    unittest_assert(BitString4("1xz").repeat(0).width() == 0);
  });

  suite.test("as_ascii").run([]() {
    unittest_assert(BitString4(16, 0x4142).as_ascii() == "AB");
    unittest_assert(BitString4(0).as_ascii() == "");
  });

  suite.test("as_u").run([]() {
    unittest_assert(BitString4(8, 200).as_u<uint8_t>() == 200);
    unittest_assert(BitString4(100, 1234).as_u<uint64_t>() == 1234);
    bool thrown = false;
    try {
      BitString4("01x").as_u<uint8_t>();
    } catch (const std::runtime_error&) {
      thrown = true;
    }
    unittest_assert(thrown);
  });

  suite.test("has_unknown").run([]() {
    unittest_assert(!BitString4("0101").has_unknown());
    unittest_assert(BitString4("01x1").has_unknown());
    unittest_assert(BitString4("z101").has_unknown());
  });

  suite.test("unary operator-").run([]() {
    unittest_assert(-BitString4("00001") == BitString4("11111"));
    unittest_assert(-BitString4("00000") == BitString4("00000"));
    unittest_assert(-BitString4("00x01") == BitString4::fully(Bit4::X, 5));
  });

  suite.test("write_hex").run([]() {
    std::ostringstream stream;
    BitString4("01zx").write_hex(stream);
    unittest_assert(stream.str() == "4'h01zx");
  });

  return suite.finish();
}
