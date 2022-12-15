#pragma once
#include "alloc.hpp"
template <typename TypeInteger, typename TypeString, typename TypeArray,
          typename TypeBoolean, typename TypeFloat, typename TypeBinary,
          typename Alloc>
class Object {
  using Int = TypeInteger;
  using String = TypeString;
  using Array = TypeArray;
  using Bool = TypeBoolean;
  using Folat = TypeFloat;
  using Binary = TypeBinary;

 private:
  union JsonValue {
    /* data */
    Int* m_integer;
    Object* m_object;
    String* m_string;
    Array* m_array;
    Bool* m_boolean;
    Folat* m_float;
    Binary* m_binary;

    /* function */
    JsonValue() = default;
    JsonValue(Int v) noexcept : m_integer(v){};
    JsonValue(Object v) noexcept : m_object(v){};
    JsonValue(String v) noexcept : m_string(v){};
    JsonValue(Array v) noexcept : m_array(v){};
    JsonValue(Bool v) noexcept : m_boolean(v){};
    JsonValue(Folat v) noexcept : m_float(v){};
    JsonValue(Binary v) noexcept : m_binary(v){};

    JsonValue(const Object& v) : m_object(Alloc::create<Object>(v)) {}
    JsonValue(Object&& v) : m_object(Alloc::create<Object>(std::move(v))) {}

    JsonValue(const String& v) : m_object(Alloc::create<String>(v)) {}
    JsonValue(String&& v) : m_object(Alloc::create<String>(std::move(v))) {}

    JsonValue(const Array& v) : m_object(Alloc::create<Array>(v)) {}
    JsonValue(Array&& v) : m_object(Alloc::create<Array>(std::move(v))) {}

    Destory() {}
  } m_jsonValue;
};