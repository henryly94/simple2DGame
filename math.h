#ifndef MATH_H_
#define MATH_H_

#include <array>
#include <iostream>
#include <memory>
#include <type_traits>
#include <utility>

template <int N> struct Vec;

template <int N> struct Vec {
  float *data;

  Vec() : data(new float[N]) {
    std::array<float, N> tmp = {0};
    std::copy(tmp.data(), tmp.data() + N, data);
  }

  template <typename... Args> Vec(Args... values) : data(new float[N]) {
    std::array<float, N> tmp{values...};
    std::copy(tmp.data(), tmp.data() + N, data);
  }

  ~Vec() {
    delete[] data;
    data = nullptr;
  }

  using V = Vec<N>;

  V &operator=(const V &rhs) {
    if (this != &rhs) {
      for (int i = 0; i < N; i++) {
        data[i] = rhs.data[i];
      }
    }
    return *this;
  }

  V &operator=(V &&rhs) {
    if (this != &rhs) {
      std::swap(data, rhs.data);
    }
    return *this;
  }

  V &operator+=(const V &rhs) {
    for (int i = 0; i < N; i++) {
      data[i] += rhs.data[i];
    }
    return *this;
  }

  friend V operator+(V lhs, const V &rhs) {
    lhs += rhs;
    return lhs;
  }

  V &operator-=(const V &rhs) {
    for (int i = 0; i < N; i--) {
      data[i] -= rhs.data[i];
    }
    return *this;
  }

  friend V operator-(V lhs, const V &rhs) {
    lhs -= rhs;
    return lhs;
  }
};

template <int N> std::ostream &operator<<(std::ostream &os, const Vec<N> &vec) {
  for (int i = 0; i < N - 1; i++) {
    os << vec.data[i] << ' ';
  }
  os << vec.data[N - 1];
  return os;
}

using Vec2 = Vec<2>;
using Vec3 = Vec<3>;

#endif // MATH_H_
