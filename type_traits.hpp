#pragma once

#include <type_traits>

#include "json_fwd.hpp"
#include "macro.hpp"

template <typename>
struct is_basic_json : std::false_type {};

// 得先声明
JSON_BASIC_TEMPLATE_HEADER_DECLARATION
struct is_basic_json<JSON_BASIC_TEMPLATE_HEADER> : std::true_type {};

template <typename BasicJsonContext>
struct is_basic_json_context
    : std::bool_constant<
          is_basic_json<typename std::remove_cv_t<
              typename std::remove_pointer_t<BasicJsonContext>>>::value ||
          std::is_same<BasicJsonContext, std::nullptr_t>::value> {};