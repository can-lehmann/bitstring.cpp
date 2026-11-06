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
#include <sstream>

#define always_inline inline __attribute__((always_inline))

namespace bitstring {
  using Limb = uint64_t;

  template <class Self>
  class BitStringBase2 {
  private:
    always_inline const Limb* data() const { return static_cast<const Self*>(this)->data(); }
    always_inline size_t width() const { return static_cast<const Self*>(this)->width(); }
  public:
    BitStringBase2() {}

    always_inline size_t limbs() const {
      return (width() + 63) / 64;
    }

    bool operator==(const BitStringBase2& other) const {
      if (width() != other.width()) {
        return false;
      }
      for (size_t i = 0; i < limbs(); i++) {
        if (data()[i] != other.data()[i]) {
          return false;
        }
      }
      return true;
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

    always_inline const Limb* data() const {
      if (_width <= 64) {
        return &_data.small;
      } else {
        return _data.ptr;
      }
    }

    always_inline size_t width() const { return _width; }

  };

  class BitStringView2 final: public BitStringBase2<BitStringView2> {
  private:
    const Limb* _data;
    size_t _width;
  public:
    BitStringView2(const Limb* data, size_t width): _data(data), _width(width) {}

    always_inline const Limb* data() const { return _data; }
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

#undef always_inline
