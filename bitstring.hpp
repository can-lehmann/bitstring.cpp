#pragma once

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

#include <inttypes.h>
#include <assert.h>
#include <sstream>

#define always_inline inline __attribute__((always_inline))

namespace bitstring {
  using Limb = uint64_t;
  static constexpr size_t LIMB_WIDTH = sizeof(Limb) * 8;

  always_inline Limb mask_lo(size_t width) {
    if (width >= LIMB_WIDTH) {
      return ~(Limb)0;
    } else {
      return ((Limb)1 << (Limb)width) - 1;
    }
  }

  always_inline Limb mask_between(size_t lower, size_t upper) {
    if (lower != 0) {
      return mask_lo(upper) & ~mask_lo(lower);
    } else {
      return mask_lo(upper);
    }
  }

  always_inline Limb shl(Limb self, Limb lo, size_t inner_count) {
    if (inner_count == 0) {
      return self;
    } else {
      return (self << inner_count) | (lo >> (LIMB_WIDTH - inner_count));
    }
  }

  always_inline Limb shr(Limb hi, Limb self, size_t inner_count) {
    if (inner_count == 0) {
      return self;
    } else {
      return (self >> inner_count) | (hi << (LIMB_WIDTH - inner_count));
    }
  }

  class BitString2;
  class BitStringView2;

  template <class Self>
  class BitStringBase2 {
  private:
    always_inline const Limb* data() const { return static_cast<const Self*>(this)->data(); }
    always_inline Limb* data() { return static_cast<Self*>(this)->data(); }
    always_inline size_t width() const { return static_cast<const Self*>(this)->width(); }

    always_inline Limb masked_limb_with(size_t index, bool upper) const {
      if (index < limbs()) {
        if (index + 1 == limbs()) {
          Limb limb = data()[index] & upper_mask();
          return upper ? limb | ~upper_mask() : limb;
        } else {
          return data()[index];
        }
      } else {
        return upper ? ~(Limb)0 : 0;
      }
    }
  public:
    BitStringBase2() {}

    always_inline size_t limbs() const {
      return (width() + LIMB_WIDTH - 1) / LIMB_WIDTH;
    }

    always_inline Limb upper_mask() const {
      size_t n = width() % LIMB_WIDTH;
      if (n == 0) {
        return ~(Limb)0;
      } else {
        return ((Limb)1 << n) - 1;
      }
    }

    bool operator==(const BitStringBase2& other) const {
      if (width() != other.width()) {
        return false;
      }
      if (limbs() > 0) {
        for (size_t it = 0; it + 1 < limbs(); it++) {
          if (data()[it] != other.data()[it]) {
            return false;
          }
        }
        if ((data()[limbs() - 1] & upper_mask()) != (other.data()[limbs() - 1] & other.upper_mask())) {
          return false;
        }
      }
      return true;
    }

    bool operator!=(const BitStringBase2& other) const {
      return !(*this == other);
    }

    bool at(size_t index) const {
      assert(index < width());
      return (data()[index / LIMB_WIDTH] >> (index % LIMB_WIDTH)) & 1;
    }

    bool intersects(const BitStringBase2& other) const {
      assert(width() == other.width());
      if (limbs() > 0) {
        for (size_t it = 0; it + 1 < limbs(); it++) {
          if ((data()[it] & other.data()[it]) != 0) {
            return true;
          }
        }
        if ((data()[limbs() - 1] & other.data()[limbs() - 1] & upper_mask()) != 0) {
          return true;
        }
      }
      return false;
    }

    bool is_fully(bool value) const {
      if (limbs() > 0) {
        Limb expected = value ? ~(Limb)0 : 0;
        for (size_t it = 0; it + 1 < limbs(); it++) {
          if (data()[it] != expected) {
            return false;
          }
        }
        if ((data()[limbs() - 1] & upper_mask()) != (expected & upper_mask())) {
          return false;
        }
      }
      return true;
    }

    void set(size_t index, bool value) {
      assert(index < width());
      if (value) {
        data()[index / LIMB_WIDTH] |= ((Limb)1 << (index % LIMB_WIDTH));
      } else {
        data()[index / LIMB_WIDTH] &= ~((Limb)1 << (index % LIMB_WIDTH));
      }
    }

    void set(size_t offset, size_t width, bool value) {
      assert(offset + width <= this->width());
      if (width == 0) {
        return;
      }
      for (size_t it = offset / LIMB_WIDTH; it <= (offset + width - 1) / LIMB_WIDTH; it++) {
        size_t limb_offset = it * LIMB_WIDTH;
        size_t limb_width = std::min(LIMB_WIDTH, offset + width - limb_offset);
        Limb mask = mask_between(offset > limb_offset ? offset - limb_offset : 0, limb_width);
        if (value) {
          data()[it] |= mask;
        } else {
          data()[it] &= ~mask;
        }
      }
    }

    bool any(size_t offset, size_t width) const {
      assert(offset + width <= this->width());
      if (width == 0) {
        return false;
      }
      for (size_t it = offset / LIMB_WIDTH; it <= (offset + width - 1) / LIMB_WIDTH; it++) {
        size_t limb_offset = it * LIMB_WIDTH;
        size_t limb_width = std::min(LIMB_WIDTH, offset + width - limb_offset);
        Limb mask = mask_between(offset > limb_offset ? offset - limb_offset : 0, limb_width);
        if (data()[it] & mask) {
          return true;
        }
      }
      return false;
    }

    bool any() const {
      return !is_fully(false);
    }

    bool or_inplace(const BitStringBase2& other) {
      assert(width() == other.width());
      bool changed = false;
      if (limbs() > 0) {
        for (size_t it = 0; it + 1 < limbs(); it++) {
          Limb res = data()[it] | other.data()[it];
          changed |= res != data()[it];
          data()[it] = res;
        }
        Limb res = data()[limbs() - 1] | (other.data()[limbs() - 1] & other.upper_mask());
        changed |= res != data()[limbs() - 1];
        data()[limbs() - 1] = res;
      }
      return changed;
    }

    always_inline Self& operator|=(const BitStringBase2& other) {
      or_inplace(other);
      return *static_cast<Self*>(this);
    }

    always_inline BitString2 operator|(const BitStringBase2& other) const;
    always_inline BitString2 operator&(const BitStringBase2& other) const;
    always_inline BitString2 operator^(const BitStringBase2& other) const;
    always_inline BitString2 operator~() const;

    BitString2 shl(size_t count) const;
    BitString2 shr_u(size_t count) const;
    BitString2 shr_s(size_t count) const;

    BitString2 operator+(const BitStringBase2& other) const;
    BitString2 operator-(const BitStringBase2& other) const;
    BitString2 operator*(const BitStringBase2& other) const;

    void write(std::ostream& stream) const {
      stream << width() << "'b";
      size_t it = width();
      while (it > 0 && !at(it - 1)) {
        it--;
      }
      if (it == 0 && width() > 0) {
        stream << '0';
      } else {
        while (it-- > 0) {
          stream << (at(it) ? '1' : '0');
        }
      }
    }

    std::string to_string() const {
      std::ostringstream stream;
      write(stream);
      return stream.str();
    }
  };

  class BitString2 final: public BitStringBase2<BitString2> {
  private:
    size_t _width = 0;
    union {
      Limb* ptr;
      uint64_t small;
    } _data;
  public:
    BitString2() {}
    BitString2(size_t width): _width(width) {
      if (_width <= 64) {
        _data.small = 0;
      } else {
        _data.ptr = new Limb[limbs()]();
      }
    }

    BitString2(size_t width, uint64_t value): BitString2(width) {
      _Static_assert(sizeof(Limb) == sizeof(uint64_t), "Limb size must be 64 bits");
      data()[0] = value;
    }

    ~BitString2() {
      if (_width > 64) {
        delete[] _data.ptr;
      }
    }

    BitString2(const BitString2& other): _width(other._width) {
      if (_width <= 64) {
        _data.small = other._data.small;
      } else {
        _data.ptr = new Limb[limbs()]();
        std::copy(other._data.ptr, other._data.ptr + limbs(), _data.ptr);
      }
    }

    BitString2(BitString2&& other): _width(other._width) {
      if (_width <= 64) {
        _data.small = other._data.small;
      } else {
        _data.ptr = other._data.ptr;
        other._data.ptr = nullptr;
      }
    }

    BitString2& operator=(const BitString2& other) {
      if (this != &other) {
        this->~BitString2();
        new (this) BitString2(other);
      }
      return *this;
    }

    BitString2& operator=(BitString2&& other) {
      if (this != &other) {
        this->~BitString2();
        new (this) BitString2(std::move(other));
      }
      return *this;
    }

    always_inline const Limb* data() const {
      if (_width <= 64) {
        return &_data.small;
      } else {
        return _data.ptr;
      }
    }

    always_inline Limb* data() {
      if (_width <= 64) {
        return &_data.small;
      } else {
        return _data.ptr;
      }
    }

    always_inline size_t width() const { return _width; }

    static BitString2 zeros(size_t width) {
      return BitString2(width);
    }

    static BitString2 ones(size_t width) {
      BitString2 result(width);
      for (size_t it = 0; it < result.limbs(); it++) {
        result.data()[it] = ~(Limb)0;
      }
      return result;
    }

    static BitString2 fully(bool value, size_t width) {
      if (value) {
        return ones(width);
      } else {
        return zeros(width);
      }
    }
  };

  template <class Self>
  always_inline BitString2 BitStringBase2<Self>::operator|(const BitStringBase2& other) const {
    BitString2 result(width());
    for (size_t it = 0; it < limbs(); it++) {
      result.data()[it] = data()[it] | other.data()[it];
    }
    return result;
  }

  template <class Self>
  always_inline BitString2 BitStringBase2<Self>::operator&(const BitStringBase2& other) const {
    BitString2 result(width());
    for (size_t it = 0; it < limbs(); it++) {
      result.data()[it] = data()[it] & other.data()[it];
    }
    return result;
  }

  template <class Self>
  always_inline BitString2 BitStringBase2<Self>::operator^(const BitStringBase2& other) const {
    assert(width() == other.width());
    BitString2 result(width());
    for (size_t it = 0; it < limbs(); it++) {
      result.data()[it] = data()[it] ^ other.data()[it];
    }
    return result;
  }

  template <class Self>
  always_inline BitString2 BitStringBase2<Self>::operator~() const {
    BitString2 result(width());
    for (size_t it = 0; it < limbs(); it++) {
      result.data()[it] = ~data()[it];
    }
    return result;
  }

  template <class Self>
  BitString2 BitStringBase2<Self>::shl(size_t count) const {
    BitString2 result(width());
    if (count >= width()) {
      return result;
    }
    size_t inner_count = count % LIMB_WIDTH;
    size_t outer_count = count / LIMB_WIDTH;
    for (size_t it = 0; it < limbs(); it++) {
      Limb lo = it > outer_count ? data()[it - outer_count - 1] : 0;
      Limb self = it >= outer_count ? data()[it - outer_count] : 0;
      result.data()[it] = bitstring::shl(self, lo, inner_count);
    }
    return result;
  }

  template <class Self>
  BitString2 BitStringBase2<Self>::shr_u(size_t count) const {
    BitString2 result(width());
    if (count >= width()) {
      return result;
    }
    size_t inner_count = count % LIMB_WIDTH;
    size_t outer_count = count / LIMB_WIDTH;
    for (size_t it = 0; it < limbs(); it++) {
      Limb self = masked_limb_with(it + outer_count, false);
      Limb hi = masked_limb_with(it + outer_count + 1, false);
      result.data()[it] = bitstring::shr(hi, self, inner_count);
    }
    return result;
  }

  template <class Self>
  BitString2 BitStringBase2<Self>::shr_s(size_t count) const {
    BitString2 result(width());
    if (count >= width()) {
      return result;
    }
    size_t inner_count = count % LIMB_WIDTH;
    size_t outer_count = count / LIMB_WIDTH;
    bool sign = at(width() - 1);
    for (size_t it = 0; it < limbs(); it++) {
      Limb self = masked_limb_with(it + outer_count, sign);
      Limb hi = masked_limb_with(it + outer_count + 1, sign);
      result.data()[it] = bitstring::shr(hi, self, inner_count);
    }
    return result;
  }

  template <class Self>
  BitString2 BitStringBase2<Self>::operator+(const BitStringBase2& other) const {
    assert(width() == other.width());
    BitString2 result(width());
    Limb carry = 0;
    for (size_t it = 0; it < limbs(); it++) {
      bool overflow = false;
      overflow |= __builtin_add_overflow(data()[it], other.data()[it], &result.data()[it]);
      overflow |= __builtin_add_overflow(result.data()[it], carry, &result.data()[it]);
      carry = overflow ? 1 : 0;
    }
    return result;
  }

  template <class Self>
  BitString2 BitStringBase2<Self>::operator-(const BitStringBase2& other) const {
    assert(width() == other.width());
    BitString2 result(width());
    Limb borrow = 0;
    for (size_t it = 0; it < limbs(); it++) {
      bool overflow = false;
      overflow |= __builtin_sub_overflow(data()[it], other.data()[it], &result.data()[it]);
      overflow |= __builtin_sub_overflow(result.data()[it], borrow, &result.data()[it]);
      borrow = overflow ? 1 : 0;
    }
    return result;
  }

  template <class Self>
  BitString2 BitStringBase2<Self>::operator*(const BitStringBase2& other) const {
    assert(width() == other.width());
    BitString2 result(width());
    if (limbs() == 1) {
      result.data()[0] = data()[0] * other.data()[0];
    } else {
      for (size_t it = 0; it < width(); it++) {
        if (other.at(it)) {
          result = result + shl(it);
        }
      }
    }
    return result;
  }

  class BitStringView2 final: public BitStringBase2<BitStringView2> {
  private:
    Limb* _data;
    size_t _width;
  public:
    BitStringView2(Limb* data, size_t width): _data(data), _width(width) {}

    always_inline const Limb* data() const { return _data; }
    always_inline Limb* data() { return _data; }
    always_inline size_t width() const { return _width; }
  };

  template <class T>
  struct ABVal {
    T a;
    T b;
  };

  template <class Self>
  class BitStringBase4 {
  private:
    always_inline ABVal<const Limb*> data() const { return static_cast<const Self*>(this)->data(); }
    always_inline size_t width() const { return static_cast<const Self*>(this)->width(); }
  public:
    BitStringBase4() {}

    always_inline size_t limbs() const {
      return (width() + 63) / 64;
    }

    always_inline BitStringView2 a_val() const {
      return BitStringView2(data().a, width());
    }

    always_inline BitStringView2 b_val() const {
      return BitStringView2(data().b, width());
    }

    bool operator==(const BitStringBase4& other) const {
      if (width() != other.width()) {
        return false;
      }
      for (size_t i = 0; i < limbs(); i++) {
        if (data().a[i] != other.data().a[i]) {
          return false;
        }
        if (data().b[i] != other.data().b[i]) {
          return false;
        }
      }
      return true;
    }

    bool operator!=(const BitStringBase4& other) const {
      return !(*this == other);
    }

    void write(std::ostream& stream) const {
      // TODO
    }
  };

  class BitString4 final: public BitStringBase4<BitString4> {
  private:
    size_t _width = 0;
    union {
      ABVal<Limb*> ptr;
      ABVal<Limb> small;
    } _data;
  public:
    BitString4() {}
    BitString4(size_t width): _width(width) {
      if (_width <= 64) {
        _data.small = {0, 0};
      } else {
        _data.ptr = {new Limb[limbs()](), new Limb[limbs()]()};
      }
    }

    ~BitString4() {
      if (_width > 64) {
        delete[] _data.ptr.a;
        delete[] _data.ptr.b;
      }
    }

    BitString4(const BitString4& other): _width(other._width) {
      if (_width <= 64) {
        _data.small = other._data.small;
      } else {
        _data.ptr = {new Limb[limbs()](), new Limb[limbs()]()};
        std::copy(other._data.ptr.a, other._data.ptr.a + limbs(), _data.ptr.a);
        std::copy(other._data.ptr.b, other._data.ptr.b + limbs(), _data.ptr.b);
      }
    }

    BitString4(BitString4&& other): _width(other._width) {
      if (_width <= 64) {
        _data.small = other._data.small;
      } else {
        _data.ptr = other._data.ptr;
        other._data.ptr = {nullptr, nullptr};
      }
    }

    BitString4& operator=(const BitString4& other) {
      if (this != &other) {
        this->~BitString4();
        new (this) BitString4(other);
      }
      return *this;
    }

    BitString4& operator=(BitString4&& other) {
      if (this != &other) {
        this->~BitString4();
        new (this) BitString4(std::move(other));
      }
      return *this;
    }

    always_inline ABVal<const Limb*> data() const {
      if (_width <= 64) {
        return {&_data.small.a, &_data.small.b};
      } else {
        return {_data.ptr.a, _data.ptr.b};
      }
    }

    always_inline size_t width() const { return _width; }
  };

  class BitStringView4 final: public BitStringBase4<BitStringView4> {
  private:
    ABVal<const Limb*> _data;
    size_t _width;
  public:
    BitStringView4(ABVal<const Limb*> data, size_t width): _data(data), _width(width) {}
    
    always_inline ABVal<const Limb*> data() const { return _data; }
    always_inline size_t width() const { return _width; }
  };
}

template <class Self>
std::ostream& operator<<(std::ostream& stream, const bitstring::BitStringBase2<Self>& value) {
  value.write(stream);
  return stream;
}

template <class Self>
std::ostream& operator<<(std::ostream& stream, const bitstring::BitStringBase4<Self>& value) {
  value.write(stream);
  return stream;
}

#undef always_inline
