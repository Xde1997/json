#pragma once
#include <exception>
#include <iostream>
#include <string>

#include "json_fwd.hpp"
#include "type_traits.hpp"

template <
    typename BasicJsonContext,
    std::enable_if_t<is_basic_json_context<BasicJsonContext>::value, int> = 0>
class Error : public std::exception {
 protected:
  Error(int _id, const std::string& _what, BasicJsonContext context) noexcept {
    std::stringstream stream;
    stream << "[json.exception." << s_errorType << "." << std::to_string(_id)
           << "] ";
    if (auto diagnostic = diagnostics(context) && diagnostics != "")
      stream << "[diagnostic:" << diagnostic << ".]";

    stream << _what;
    std::string buffer << stream;
    m_error = std::exception(buffer.c_str());
  }

 private:
  std::string diagnostics(std::nullptr_t) { return ""; }

  std::string diagnostics(const BasicJsonContext* _leafElement) { return ""; }

 private:
  std::runtime_error m_error;
  int m_id;

 protected:
  static std::string s_errorType;
};

template <
    typename BasicJsonContext,
    std::enable_if_t<is_basic_json_context<BasicJsonContext>::value, int> = 0>
class InvalidIteratorError : public Error<BasicJsonContext> {
 public:
  InvalidIteratorError(int _id, const std::string& _what,
                       BasicJsonContext context)
      : Error(_id, _what, context) {}

 protected:
  inline static std::string s_errorType = "invalid_iterator";
};

template <
    typename BasicJsonContext,
    std::enable_if_t<is_basic_json_context<BasicJsonContext>::value, int> = 0>
class TypeError : public Error<BasicJsonContext> {
 public:
  TypeError(int _id, const std::string& _what, BasicJsonContext context)
      : Error(_id, _what, context) {}

 protected:
  inline static std::string s_errorType = "type_error";
};