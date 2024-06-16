#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <vector>

namespace py = pybind11;

namespace utils {
  template <typename T> void print_1d(const std::vector<T> &v) {
    std::cout << "[";
    for (size_t i = 0; i < v.size(); i++) {
      std::cout << v[i];
      if (i < v.size() - 1) {
        std::cout << ", ";
      }
    }
    std::cout << "]";
    std::cout << std::endl;
  }

  template <typename T> void print_2d(const std::vector<std::vector<T>> &v) {
    std::cout << "[";
    for (size_t i = 0; i < v.size(); i++) {
      if (i > 0) {
        std::cout << "  ";
      }
      print_1d(v[i]);
      if (i < v.size() - 1) {
        std::cout << "\n";
      }
    }
    std::cout << "]";
    std::cout << std::endl;
  }

  // https://stackoverflow.com/a/61937194
  template <typename T> py::array_t<T> to_np_1d(const std::vector<T> &X) {
    int N = (int)X.size();
    py::array_t<int, py::array::c_style> X_np({N});
    auto ra = X_np.mutable_unchecked();
    for (int i = 0; i < N; i++) {
      ra(i) = X[i];
    };
    return X_np;
  }

  template <typename T> py::array_t<T> to_np_2d(const std::vector<std::vector<T>> &X) {
    size_t N = X.size();
    size_t M = X[0].size();
    py::array_t<int, py::array::c_style> X_np({N, M});
    auto ra = X_np.mutable_unchecked();
    for (size_t i = 0; i < N; i++) {
      for (size_t j = 0; j < M; j++) {
        ra(i, j) = X[i][j];
      };
    };
    return X_np;
  }
}  // namespace utils

#endif  // UTILS_HPP
