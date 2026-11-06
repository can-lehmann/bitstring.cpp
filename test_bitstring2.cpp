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
    unittest_assert(BitString2(10).to_string() == "10'b0");
    unittest_assert(BitString2(0).to_string() == "0'b");
    unittest_assert(BitString2::zeros(128).to_string() == "128'b0");
    unittest_assert(BitString2::ones(10).to_string() == "10'b1111111111");
    unittest_assert(BitString2::fully(true, 3).to_string() == "3'b111");
    unittest_assert(BitString2::fully(false, 2).to_string() == "2'b0");
    unittest_assert(BitString2(32, 10).to_string() == "32'b1010");
    unittest_assert(BitString2(3, ~0).to_string() == "3'b111");
  });

  suite.test("at").run([]() {
    unittest_assert(BitString2(5, 0b10101).at(0) == true);
    unittest_assert(BitString2(5, 0b10101).at(1) == false);
    unittest_assert(BitString2(5, 0b10101).at(4) == true);
    unittest_assert(BitString2::ones(256).at(255) == true);
  });

  suite.test("set").run([]() {
    BitString2 a(5);
    a.set(0, true);
    unittest_assert(a.to_string() == "5'b1");
    a.set(1, true);
    unittest_assert(a.to_string() == "5'b11");
    a.set(0, false);
    unittest_assert(a.to_string() == "5'b10");
    a.set(2, 3, true);
    unittest_assert(a.to_string() == "5'b11110");
    a.set(0, 2, false);
    std::cout << a.to_string() << std::endl;
    unittest_assert(a.to_string() == "5'b11100");
    a.set(0, 5, false);
    unittest_assert(a.to_string() == "5'b0");
    a.set(1, 3, true);
    unittest_assert(a.to_string() == "5'b1110");
  });

  suite.test("any").run([]() {
    unittest_assert(BitString2(5, 0b10101).any(0, 5) == true);
    unittest_assert(BitString2(5, 0b10101).any(1, 3) == true);
    unittest_assert(BitString2(5, 0b10001).any(1, 3) == false);
  });

  suite.test("intersects").run([]() {
    unittest_assert(BitString2(5, 0b10101).intersects(BitString2(5, 0b10000)) == true);
    unittest_assert(BitString2(5, 0b10101).intersects(BitString2(5, 0b01010)) == false);
    unittest_assert(BitString2(5, 0b00000).intersects(BitString2(5, 0b00000)) == false);
    unittest_assert(BitString2(5, 0b11111).intersects(BitString2(5, 0b11111)) == true);
  });

  suite.test("is_fully").run([]() {
    unittest_assert(BitString2(5, 0b11111).is_fully(true) == true);
    unittest_assert(BitString2(5, 0b11110).is_fully(true) == false);
    unittest_assert(BitString2(5, 0b00000).is_fully(false) == true);
    unittest_assert(BitString2(5, 0b00001).is_fully(false) == false);
  });

  suite.test("or_inplace").run([]() {
    BitString2 a(5, 0b10101);
    BitString2 b(5, 0b01010);
    unittest_assert(a.or_inplace(b) == true);
    unittest_assert(a.to_string() == "5'b11111");
    unittest_assert(a.or_inplace(b) == false);
    unittest_assert(a.to_string() == "5'b11111");
  });

  suite.test("operator|").run([]() {
    unittest_assert((BitString2(5, 0b10101) | BitString2(5, 0b01010)).to_string() == "5'b11111");
    unittest_assert((BitString2(5, 0b10101) | BitString2(5, 0b00000)).to_string() == "5'b10101");
    unittest_assert((BitString2(5, 0b00001) | BitString2(5, 0b00010)).to_string() == "5'b11");
  });

  suite.test("operator&").run([]() {
    unittest_assert((BitString2(5, 0b10101) & BitString2(5, 0b01010)).to_string() == "5'b0");
    unittest_assert((BitString2(5, 0b10101) & BitString2(5, 0b11111)).to_string() == "5'b10101");
    unittest_assert((BitString2(5, 0b00111) & BitString2(5, 0b00010)).to_string() == "5'b10");
  });

  suite.test("operator~").run([]() {
    unittest_assert((~BitString2(5, 0b10101)).to_string() == "5'b1010");
    unittest_assert((~BitString2(5, 0b00000)).to_string() == "5'b11111");
    unittest_assert((~BitString2(5, 0b11111)).to_string() == "5'b0");
  });

  // Utilities

  suite.test("utils/mask_lo").run([]() {
    unittest_assert(mask_lo(0) == 0b0);
    unittest_assert(mask_lo(3) == 0b111);
    unittest_assert(mask_lo(LIMB_WIDTH) == ~(Limb)0);
  });
  
  suite.test("utils/mask_between").run([]() {
    unittest_assert(mask_between(0, 0) == 0b0);
    unittest_assert(mask_between(0, 3) == 0b111);
    unittest_assert(mask_between(2, 5) == 0b11100);
    unittest_assert(mask_between(0, LIMB_WIDTH) == ~(Limb)0);
    unittest_assert(mask_between(1, LIMB_WIDTH) == (~Limb(0) ^ 1));
    unittest_assert(mask_between(1, LIMB_WIDTH - 1) == (~Limb(0) ^ 1 ^ (Limb(1) << (LIMB_WIDTH - 1))));
  });

  return suite.finish();
}
