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

#include <random>
#include <vector>

using namespace bitstring;

template <class Bits>
void check_bits(const Bits& actual, const std::vector<bool>& expected) {
  unittest_assert(actual.width() == expected.size());
  for (size_t index = 0; index < expected.size(); index++) {
    unittest_assert(actual.at(index) == expected[index]);
  }
}

void test_xor() {
  unittest_assert((BitString2(5, 0b10101) ^ BitString2(5, 0b01010)).to_string() == "5'b11111");
  unittest_assert((BitString2(5, 0b10101) ^ BitString2(5, 0b10101)).to_string() == "5'b0");
  unittest_assert((BitString2(5, 0b10101) ^ BitString2(5, 0b00111)).to_string() == "5'b10010");
}

template <class Bits>
void check_random_operations(Bits& a, const Bits& b,
                             std::vector<bool> expected_a,
                             const std::vector<bool>& expected_b,
                             std::mt19937_64& random) {
  size_t width = expected_a.size();
  check_bits(a, expected_a);
  check_bits(b, expected_b);
  std::vector<bool> expected_and(width), expected_or(width), expected_xor(width), expected_not(width);
  bool intersects = false;
  bool all_zero = true;
  bool all_one = true;
  bool changed = false;
  for (size_t index = 0; index < width; index++) {
    expected_and[index] = expected_a[index] && expected_b[index];
    expected_or[index] = expected_a[index] || expected_b[index];
    expected_xor[index] = expected_a[index] != expected_b[index];
    expected_not[index] = !expected_a[index];
    intersects |= expected_and[index];
    all_zero &= !expected_a[index];
    all_one &= expected_a[index];
    changed |= expected_or[index] != expected_a[index];
  }
  check_bits(a & b, expected_and);
  check_bits(a | b, expected_or);
  check_bits(a ^ b, expected_xor);
  check_bits(~a, expected_not);
  unittest_assert((a == b) == (expected_a == expected_b));
  unittest_assert((a != b) == (expected_a != expected_b));
  unittest_assert(a.intersects(b) == intersects);
  unittest_assert(a.is_fully(false) == all_zero);
  unittest_assert(a.is_fully(true) == all_one);
  unittest_assert(a.or_inplace(b) == changed);
  check_bits(a, expected_or);
  unittest_assert(!a.or_inplace(b));
  a |= b;
  check_bits(a, expected_or);
  expected_a = expected_or;
  for (size_t trial = 0; trial < 32; trial++) {
    size_t offset = trial == 0 ? 0 : random() % (width + 1);
    size_t count = trial == 0 ? 0 : random() % (width - offset + 1);
    bool any = false;
    for (size_t index = offset; index < offset + count; index++) {
      any |= expected_a[index];
    }
    unittest_assert(a.any(offset, count) == any);
    bool value = random() % 2;
    a.set(offset, count, value);
    for (size_t index = offset; index < offset + count; index++) {
      expected_a[index] = value;
    }
    check_bits(a, expected_a);
    if (width != 0) {
      size_t index = random() % width;
      a.set(index, !expected_a[index]);
      expected_a[index] = !expected_a[index];
      check_bits(a, expected_a);
    }
  }
}

void test_random_operations() {
  std::mt19937_64 random(0x62737472696e67);
  std::vector<size_t> widths = {0, 1, 2, 7, 8, 31, 32, 63, 64, 65, 127, 128, 129, 255, 256, 257, 1023, 1024, 1025};
  for (size_t trial = 0; trial < 32; trial++) {
    widths.push_back(random() % 2049);
  }
  for (size_t width : widths) {
    for (size_t trial = 0; trial < 32; trial++) {
      BitString2 a(width), b(width);
      for (size_t limb = 0; limb < a.limbs(); limb++) {
        a.data()[limb] = random();
        b.data()[limb] = random();
      }
      std::vector<bool> expected_a(width), expected_b(width);
      for (size_t index = 0; index < width; index++) {
        expected_a[index] = trial == 0 ? false : trial == 1 ? true : bool(random() % 2);
        expected_b[index] = trial < 3 ? expected_a[index] : trial == 3 ? !expected_a[index] : bool(random() % 2);
        a.set(index, expected_a[index]);
        b.set(index, expected_b[index]);
      }
      BitString2 view_a_storage(a), view_b_storage(b);
      BitStringView2 view_a(view_a_storage.data(), width), view_b(view_b_storage.data(), width);
      check_random_operations(a, b, expected_a, expected_b, random);
      check_random_operations(view_a, view_b, expected_a, expected_b, random);
    }
  }
}

int main(int argc, char** argv) {
  unittest::Suite suite(argc, argv);

  suite.test("operator^").run(test_xor);
  suite.test("random operations").run(test_random_operations);

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
