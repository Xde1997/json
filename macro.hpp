#pragma once

#include <inttypes.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

#define JSON_CONDITION_LIKELY(expr) (!!(expr))
#define JSON_CONDITION_UNLIKELY(expr) (!!(expr))

#define JSON_BASIC_TEMPLATE_HEADER_DECLARATION_DEFAULT                        \
  template <template <typename KeyType, typename ValueType, typename... Args> \
            typename TypeObject = std::map,                                   \
            typename TypeInteger = std::int64_t,                              \
            typename TypeUnsignedInteger = std::uint64_t,                     \
            typename TypeString = std::string,                                \
            template <typename ValueType, typename... Args>                   \
            typename TypeArray = std::vector,                                 \
            typename TypeBoolean = bool, typename TypeFloat = double,         \
            template <typename AllocatorType> typename Allocator =            \
                std::allocator>

#define JSON_BASIC_TEMPLATE_HEADER_DECLARATION                                 \
  template <                                                                   \
      template <typename KeyType, typename ValueType, typename... Args>        \
      typename TypeObject,                                                     \
      typename TypeInteger, typename TypeUnsignedInteger, typename TypeString, \
      template <typename ValueType, typename... Args> typename TypeArray,      \
      typename TypeBoolean, typename TypeFloat,                                \
      template <typename AllocatorType> typename Allocator>

#define JSON_BASIC_TEMPLATE_HEADER                                    \
  BasicJson<TypeObject, TypeInteger, TypeUnsignedInteger, TypeString, \
            TypeArray, TypeBoolean, TypeFloat, Allocator>

#define THROW_ERROR 1
#if THROW_ERROR
#define JSON_TRY try {
#define JSON_END_TRY }
#define JSON_CATCH catch (Error e) {
#include <iostream> \
std::cout <<e.what()<<std::endl; \
}
#define JSON_THROW(expr) throw(expr)
#else
#define JSON_TRY
#define JSON_END_TRY
#define JSON_CATCH
#define JSON_THROW(expr)
#endif