#ifndef UTILS_SERIALISE_HPP
#define UTILS_SERIALISE_HPP

#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/typing.h>

namespace py = pybind11;

template <typename T>
py::tuple __getstate__(const T &input);

template <typename T>
T __setstate__(py::tuple t);

#endif  // UTILS_SERIALISE_HPP
