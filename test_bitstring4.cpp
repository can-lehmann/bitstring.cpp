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
    unittest_assert(BitString4("").width() == 0);
    unittest_assert(BitString4(std::string(129, 'z')) == BitString4::fully(Bit4::Z, 129));
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
    unittest_assert(BitString4("1x0").eq(BitString4("010")) == Bit4::Zero);
    unittest_assert(BitString4::ones(100).eq(BitString4::ones(100)) == Bit4::One);
    unittest_assert(BitString4::ones(100).eq(BitString4(100)) == Bit4::Zero);
    unittest_assert(BitString4("0101").eq(BitString4("0101")) == Bit4::One);
    unittest_assert(BitString4("0101").eq(BitString4("0001")) == Bit4::Zero);
    unittest_assert(BitString4("01xz").eq(BitString4("01xz")) == Bit4::X);
    unittest_assert(BitString4("1x01").eq(BitString4("0001")) == Bit4::Zero);
    unittest_assert(BitString4("0001").eq(BitString4("1z01")) == Bit4::Zero);
    unittest_assert(BitString4::ones(129).eq(BitString4::ones(129)) == Bit4::One);
    unittest_assert(BitString4(0).eq(BitString4(0)) == Bit4::One);
    unittest_assert(BitString4::fully(Bit4::X, 129).eq(BitString4::ones(129)) == Bit4::X);
    unittest_assert(BitString4::concat(BitString4("1"), BitString4::fully(Bit4::X, 128)).eq(BitString4::zeros(129)) == Bit4::Zero);
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

  suite.test("vector constructor").run([]() {
    unittest_assert(BitString4(std::vector<Bit4>{Bit4::Zero, Bit4::One, Bit4::X, Bit4::Z}) == BitString4("zx10"));
    unittest_assert(BitString4(std::vector<Bit4>()).width() == 0);
    unittest_assert(BitString4(std::vector<Bit4>(129, Bit4::Z)) == BitString4::fully(Bit4::Z, 129));
  });

  suite.test("resolve").run([]() {
    unittest_assert(BitString4("01xz").resolve(BitString4("zzzz")) == BitString4("01xz"));
    unittest_assert(BitString4("zzzz").resolve(BitString4("01xz")) == BitString4("01xz"));
    unittest_assert(BitString4("0011").resolve(BitString4("0101")) == BitString4("0xx1"));
    unittest_assert(BitString4("xxzz").resolve(BitString4("zzxx")) == BitString4("xxxx"));
    unittest_assert(BitString4::ones(129).resolve(BitString4::fully(Bit4::Z, 129)) == BitString4::ones(129));
  });

  suite.test("as_float32").run([]() {
    unittest_assert(BitString4(32, 0x00000000).as_float32() == 0.0f);
    unittest_assert(BitString4(32, 0x3fc00000).as_float32() == 1.5f);
    unittest_assert(BitString4(32, 0xc0200000).as_float32() == -2.5f);
  });

  suite.test("as_float64").run([]() {
    unittest_assert(BitString4(64, 0x0000000000000000).as_float64() == 0.0);
    unittest_assert(BitString4(64, 0x3ff8000000000000).as_float64() == 1.5);
    unittest_assert(BitString4(64, 0xc004000000000000).as_float64() == -2.5);
  });

  suite.test("match_z").run([]() {
    unittest_assert(BitString4("01xz").match_z(BitString4("01xz")));
    unittest_assert(BitString4("01xz").match_z(BitString4("zzzz")));
    unittest_assert(!BitString4("0101").match_z(BitString4("01x1")));
    unittest_assert(!BitString4("z101").match_z(BitString4("0101")));
    unittest_assert(!BitString4("01").match_z(BitString4("zzz")));
    unittest_assert(BitString4::ones(129).match_z(BitString4::fully(Bit4::Z, 129)));
  });

  suite.test("match_x_z").run([]() {
    unittest_assert(BitString4("01xz").match_x_z(BitString4("01xz")));
    unittest_assert(BitString4("0101").match_x_z(BitString4("xzxz")));
    unittest_assert(!BitString4("x101").match_x_z(BitString4("0101")));
    unittest_assert(!BitString4("0101").match_x_z(BitString4("0001")));
    unittest_assert(!BitString4("01").match_x_z(BitString4("xxx")));
    unittest_assert(BitString4::ones(129).match_x_z(BitString4::fully(Bit4::X, 129)));
  });

  suite.test("clog2").run([]() {
    unittest_assert(BitString4(0).clog2() == 0);
    unittest_assert(BitString4("0000").clog2() == 0);
    unittest_assert(BitString4("0001").clog2() == 0);
    unittest_assert(BitString4("0010").clog2() == 1);
    unittest_assert(BitString4("0011").clog2() == 2);
    unittest_assert(BitString4("1000").clog2() == 3);
    unittest_assert(BitString4("1001").clog2() == 4);
    unittest_assert(BitString4(129, 1).shl(128).clog2() == 128);
    unittest_assert(BitString4::ones(129).clog2() == 129);
  });

  suite.test("count_ones").run([]() {
    unittest_assert(BitString4(0).count_ones() == 0);
    unittest_assert(BitString4("0000").count_ones() == 0);
    unittest_assert(BitString4("1xz1").count_ones() == 2);
    unittest_assert(BitString4::ones(129).count_ones() == 129);
  });

  suite.test("is_one_hot").run([]() {
    unittest_assert(!BitString4("0000").is_one_hot());
    unittest_assert(BitString4("0100").is_one_hot());
    unittest_assert(!BitString4("0101").is_one_hot());
    unittest_assert(BitString4("x1z0").is_one_hot());
    unittest_assert(BitString4(129, 1).shl(128).is_one_hot());
  });

  suite.test("is_one_hot0").run([]() {
    unittest_assert(BitString4("0000").is_one_hot0());
    unittest_assert(BitString4("0100").is_one_hot0());
    unittest_assert(!BitString4("0101").is_one_hot0());
    unittest_assert(BitString4("xz00").is_one_hot0());
    unittest_assert(!BitString4::ones(129).is_one_hot0());
  });

  suite.test("case_eq").run([]() {
    unittest_assert(BitString4("01xz").case_eq(BitString4("01xz")) == BitString4("1"));
    unittest_assert(BitString4("01xz").case_eq(BitString4("01zx")) == BitString4("0"));
    unittest_assert(BitString4("01xz").case_eq(BitString4("0101")) == BitString4("0"));
    unittest_assert(BitString4::fully(Bit4::X, 129).case_eq(BitString4::fully(Bit4::X, 129)) == BitString4("1"));
  });

  suite.test("div_u").run([]() {
    unittest_assert(BitString4(8, 17).div_u(BitString4(8, 3)) == BitString4(8, 5));
    unittest_assert(BitString4(4, 15).div_u(BitString4(4, 2)) == BitString4(4, 7));
    unittest_assert(BitString4(8, 17).div_u(BitString4(8)) == BitString4::fully(Bit4::X, 8));
    unittest_assert(BitString4("x001").div_u(BitString4("0010")) == BitString4("xxxx"));
    unittest_assert(BitString4("0001").div_u(BitString4("00z1")) == BitString4("xxxx"));
    unittest_assert(BitString4(64, UINT64_MAX).div_u(BitString4(64, 1)) == BitString4(64, UINT64_MAX));
  });

  suite.test("div_s").run([]() {
    unittest_assert(BitString4(8, -17).div_s(BitString4(8, 3)) == BitString4(8, -5));
    unittest_assert(BitString4(8, 17).div_s(BitString4(8, -3)) == BitString4(8, -5));
    unittest_assert(BitString4(8, -17).div_s(BitString4(8, -3)) == BitString4(8, 5));
    unittest_assert(BitString4("1000").div_s(BitString4("1111")) == BitString4("1000"));
    unittest_assert(BitString4(64, INT64_MIN).div_s(BitString4(64, -1)) == BitString4(64, INT64_MIN));
    unittest_assert(BitString4(8, 17).div_s(BitString4(8)) == BitString4::fully(Bit4::X, 8));
    unittest_assert(BitString4("x001").div_s(BitString4("0010")) == BitString4("xxxx"));
    unittest_assert(BitString4("0001").div_s(BitString4("00z1")) == BitString4("xxxx"));
  });

  suite.test("mod_u").run([]() {
    unittest_assert(BitString4(8, 17).mod_u(BitString4(8, 3)) == BitString4(8, 2));
    unittest_assert(BitString4(4, 15).mod_u(BitString4(4, 2)) == BitString4(4, 1));
    unittest_assert(BitString4(8, 17).mod_u(BitString4(8)) == BitString4::fully(Bit4::X, 8));
    unittest_assert(BitString4("z001").mod_u(BitString4("0010")) == BitString4("xxxx"));
    unittest_assert(BitString4("0001").mod_u(BitString4("00x1")) == BitString4("xxxx"));
    unittest_assert(BitString4(64, UINT64_MAX).mod_u(BitString4(64, 2)) == BitString4(64, 1));
  });

  suite.test("mod_s").run([]() {
    unittest_assert(BitString4(8, -17).mod_s(BitString4(8, 3)) == BitString4(8, -2));
    unittest_assert(BitString4(8, 17).mod_s(BitString4(8, -3)) == BitString4(8, 2));
    unittest_assert(BitString4(8, -17).mod_s(BitString4(8, -3)) == BitString4(8, -2));
    unittest_assert(BitString4("1000").mod_s(BitString4("1111")) == BitString4("0000"));
    unittest_assert(BitString4(64, INT64_MIN).mod_s(BitString4(64, -1)) == BitString4(64));
    unittest_assert(BitString4(8, 17).mod_s(BitString4(8)) == BitString4::fully(Bit4::X, 8));
    unittest_assert(BitString4("z001").mod_s(BitString4("0010")) == BitString4("xxxx"));
    unittest_assert(BitString4("0001").mod_s(BitString4("00x1")) == BitString4("xxxx"));
  });

  suite.test("pow").run([]() {
    unittest_assert(BitString4(8, 3).pow(BitString4(8, 4)) == BitString4(8, 81));
    unittest_assert(BitString4(4, 3).pow(BitString4(4, 4)) == BitString4(4, 1));
    unittest_assert(BitString4(8, 0).pow(BitString4(8, 0)) == BitString4(8, 1));
    unittest_assert(BitString4(8, 0).pow(BitString4(8, 1)) == BitString4(8, 0));
    unittest_assert(BitString4("00x1").pow(BitString4("0010")) == BitString4("xxxx"));
    unittest_assert(BitString4("0011").pow(BitString4("00z0")) == BitString4("xxxx"));
    unittest_assert(BitString4(64, 2).pow(BitString4(64, 63)) == BitString4(64, uint64_t(1) << 63));
  });

  suite.test("mux").run([]() {
    unittest_assert(BitString4("1").mux(BitString4("01xz"), BitString4("zx10")) == BitString4("01xz"));
    unittest_assert(BitString4("0").mux(BitString4("01xz"), BitString4("zx10")) == BitString4("zx10"));
    unittest_assert(BitString4("x").mux(BitString4("01xz"), BitString4("01xz")) == BitString4("01xz"));
    unittest_assert(BitString4("z").mux(BitString4("1zzx"), BitString4("0zzz")) == BitString4("xzzx"));
    unittest_assert(BitString4("x").mux(BitString4::ones(129), BitString4::zeros(129)) == BitString4::fully(Bit4::X, 129));
  });

  suite.test("to_bitstring2").run([]() {
    unittest_assert(BitString4("01xz").to_bitstring2() == BitString2(4, 0b0100));
    unittest_assert(BitString4::ones(129).to_bitstring2() == BitString2::ones(129));
    unittest_assert(BitString4::fully(Bit4::X, 129).to_bitstring2() == BitString2::zeros(129));
    unittest_assert(BitString4::fully(Bit4::Z, 129).to_bitstring2() == BitString2::zeros(129));
    unittest_assert(BitString4(0).to_bitstring2().width() == 0);
  });

  suite.test("view operations").run([]() {
    BitString4 a(8, -17), b(8, 3);
    BitStringView4 view_a(a.data(), a.width()), view_b(b.data(), b.width());
    unittest_assert(view_a.div_u(view_b) == BitString4(8, 79));
    unittest_assert(view_a.div_s(view_b) == BitString4(8, -5));
    unittest_assert(view_a.mod_u(view_b) == BitString4(8, 2));
    unittest_assert(view_a.mod_s(view_b) == BitString4(8, -2));
    unittest_assert(view_b.pow(view_b) == BitString4(8, 27));
    unittest_assert(view_a.eq(view_b) == Bit4::Zero);
    unittest_assert(view_a.case_eq(view_a) == BitString4("1"));
    BitString4 pattern("01xz"), wildcard("zzzz"), condition("1");
    BitStringView4 view_pattern(pattern.data(), pattern.width());
    BitStringView4 view_wildcard(wildcard.data(), wildcard.width());
    BitStringView4 view_condition(condition.data(), condition.width());
    unittest_assert(view_pattern.resolve(view_wildcard) == BitString4("01xz"));
    unittest_assert(view_pattern.match_z(view_wildcard));
    unittest_assert(view_pattern.match_x_z(view_wildcard));
    unittest_assert(view_pattern.clog2() == 2);
    unittest_assert(view_pattern.count_ones() == 1);
    unittest_assert(view_pattern.is_one_hot());
    unittest_assert(view_pattern.is_one_hot0());
    unittest_assert(view_pattern.to_bitstring2() == BitString2(4, 0b0100));
    unittest_assert(view_condition.mux(view_pattern, view_wildcard) == BitString4("01xz"));
    condition.set(0, Bit4::Zero);
    unittest_assert(view_condition.mux(view_pattern, view_wildcard) == BitString4("zzzz"));
    condition.set(0, Bit4::X);
    unittest_assert(view_condition.mux(view_pattern, view_wildcard) == BitString4("xxxz"));
    BitString4 f32(32, 0x3fc00000), f64(64, 0xc004000000000000);
    unittest_assert(BitStringView4(f32.data(), f32.width()).as_float32() == 1.5f);
    unittest_assert(BitStringView4(f64.data(), f64.width()).as_float64() == -2.5);
    BitString4 wide = BitString4::fully(Bit4::Z, 129);
    BitStringView4 view_wide(wide.data(), wide.width());
    unittest_assert(view_wide.resolve(view_wide) == BitString4::fully(Bit4::Z, 129));
    unittest_assert(view_wide.case_eq(view_wide) == BitString4("1"));
    unittest_assert(view_wide.eq(view_wide) == Bit4::X);
  });

  return suite.finish();
}
