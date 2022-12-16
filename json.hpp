#pragma once
#include <inttypes.h>

#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "exceptions.hpp"
#include "macro.hpp"
enum class value_t : std::uint8_t {
  null,
  object,
  array,
  string,
  boolean,
  number_integer,
  number_unsigned,
  number_float,
};

JSON_BASIC_TEMPLATE_HEADER_DECLARATION_DEFAULT
class BasicJson {
  using basic_json_t = JSON_BASIC_TEMPLATE_HEADER;

 public:
  BasicJson(const value_t v) : m_type(v), m_value(v) {}
  BasicJson(std::nullptr_t = nullptr) : BasicJson(value_t::null) {}

 public:
  template <typename TSupportSingleType, typename... Args>
  static TSupportSingleType* create(Args&&... args) {
    TAllocatorType alloc;
    using AllocatorTraits = std::allocator_traits<TAllocatorType>;

    auto deleter = [&](TSupportSingleType* obj) {
      AllocatorTraits::deallocate(alloc, obj, 1);
    };
    std::unique_ptr<TSupportSingleType, decltype(deleter)> obj(
        AllocatorTraits::allocate(alloc, 1), deleter);
    AllocatorTraits::construct(alloc.get(), std::forward<Args>(args)...);
    static_assert(obj != nullptr);
    return obj.release();
  }

 private:
  using object_t = TypeObject<TypeString, BasicJson,
                              Allocator<std::pair<TypeString, BasicJson>>>;
  using array_t = TypeArray<BasicJson, Allocator<BasicJson>>;
  using string_t = TypeString;
  using boolean_t = TypeBoolean;
  using number_integer_t = TypeInteger;
  using number_unsigned_t = TypeUnsignedInteger;
  using number_float_t = TypeFloat;

 private:
  union JsonValue {
    /* data */
    number_integer_t m_integer;
    number_unsigned_t m_unsigned;
    object_t* m_object;
    string_t* m_string;
    array_t* m_array;
    boolean_t m_boolean;
    number_float_t m_float;

    /* function */
    JsonValue() = default;
    JsonValue(number_integer_t v) noexcept : m_integer(v){};
    JsonValue(number_unsigned_t v) noexcept : m_integer(v){};
    JsonValue(object_t v) noexcept : m_object(v){};
    JsonValue(string_t v) noexcept : m_string(v){};
    JsonValue(array_t v) noexcept : m_array(v){};
    JsonValue(boolean_t v) noexcept : m_boolean(v){};
    JsonValue(number_float_t v) noexcept : m_float(v){};
    JsonValue(value_t t) {
      switch (t) {
        case value_t::object: {
          object = create<object_t>();
          break;
        }

        case value_t::array: {
          array = create<array_t>();
          break;
        }

        case value_t::string: {
          string = create<string_t>("");
          break;
        }

        case value_t::boolean: {
          boolean = static_cast<boolean_t>(false);
          break;
        }

        case value_t::number_integer: {
          number_integer = static_cast<number_integer_t>(0);
          break;
        }

        case value_t::number_unsigned: {
          number_unsigned = static_cast<number_unsigned_t>(0);
          break;
        }

        case value_t::number_float: {
          number_float = static_cast<number_float_t>(0.0);
          break;
        }

        case value_t::null: {
          object = nullptr;  // silence warning, see #821
          break;
        }

        default: {
          object = nullptr;  // silence warning, see #821
          if (JSON_HEDLEY_UNLIKELY(t == value_t::null)) {
            JSON_THROW(other_error::create(
                500, "961c151d2e87f2686a955a9be24d316f1362bf21 3.11.2",
                nullptr));  // LCOV_EXCL_LINE
          }
          break;
        }
      }
    }

    JsonValue(const object_t& v) : m_object(create<object_t>(v)) {}
    JsonValue(object_t&& v) : m_object(create<object_t>(std::move(v))) {}

    JsonValue(const string_t& v) : m_object(create<string_t>(v)) {}
    JsonValue(string_t&& v) : m_object(create<string_t>(std::move(v))) {}

    JsonValue(const array_t& v) : m_object(create<array_t>(v)) {}
    JsonValue(array_t&& v) : m_object(create<array_t>(std::move(v))) {}

    void Destory(value_t v) {
      if (t == value_t::array || t == value_t::object) {
        std::vector<BasicJson> stack;
        // 这里默认使用了stl
        if (t == value_t::array) {
          stack.reserve(m_array.size());
          std::move(m_array.begin(), m_array.end(), std::back_inserter(stack));
        } else {
          stack.reserve(m_object.size());
          // 这里默认使用了map
          for (auto&& it : *m_object) {
            stack.push_back(std::move(it.second));
          }
        }

        while (!stack.empty()) {
          BasicJson currentItem(std::move(stack.back()));
          stack.pop_back();

          if (currentItem.is_array()) {
            std::move(currentItem.m_value.m_array->begin(),
                      currentItem.m_value.m_array->end(),
                      std::back_inserter(stack));
            currentItem.m_value.m_array->clear();
          } else if (currentItem.is_object()) {
            for (auto&& it : *currentItem.m_value.m_object) {
              stack.push_back(std::move(it.second));
            }
            currentItem.m_value.m_object->clear();
          }
        }
      }

      // common delete
      switch (t) {
        case value_t::object: {
          Allocator<object_t> alloc;
          std::allocator_traits<decltype<alloc>>::destory(alloc, m_object);
          std::allocator_traits<decltype<alloc>>::deallocate(alloc, m_object,
                                                             1);
          break;
        }
        case value_t::array: {
          AllocatorType<array_t> alloc;
          std::allocator_traits<decltype(alloc)>::destroy(alloc, array);
          std::allocator_traits<decltype(alloc)>::deallocate(alloc, array, 1);
          break;
        }

        case value_t::string: {
          AllocatorType<string_t> alloc;
          std::allocator_traits<decltype(alloc)>::destroy(alloc, string);
          std::allocator_traits<decltype(alloc)>::deallocate(alloc, string, 1);
          break;
        }
        case value_t::null:
        case value_t::boolean:
        case value_t::number_integer:
        case value_t::number_unsigned:
        default:
          break;
      }
    }
  } m_value;
  value_t m_type;

 public:
  bool IsArray() { return m_type == value_t::array; }
  bool IsObject() { return m_type == value_t::object; }
  bool IsNumber() {
    return m_type == value_t::number_integer ||
           m_type == value_t::number_unsigned;
  }
  bool IsInteger() { return m_type == value_t::number_integer; }
  bool IsUnsigned() { return m_type == value_t::number_unsigned; }
  bool IsFloat() { return m_type == value_t::number_float; }
  bool IsNull() { return m_type == value_t::null; }
  bool IsBoolean() { return m_type == value_t::boolean; }
  bool IsString() { return m_type == value_t::string; }

 public:
  template <typename PointerType,
            typename std::enable_if<std::is_pointer<PointerType>::value,
                                    int>::value = 0>
  auto GetPtr() noexcept -> decltype(std::declval<basic_json_t&>().GetimplPtr(
      std::declval<PointerType>())) {
    return GetImplPtr(static_cast<PointerType>(nullptr));
  }

  template <
      typename PointerType,
      typename std::enable_if<
          std::is_pointer_v<PointerType> &&
              std::is_const_v<typename std::remove_pointer_t<PointerType>>,
          int>::type = 0>
  constexpr auto GetPtr() const noexcept
      -> decltype(std::declval<const basid_json_t&().GetImplPtr(
                      std::declval<PointerType>())>) {
    return GetImplPtr(static_cast<PointerType>(nullptr));
  }

 public:
  // std::pair<bool, boolean_t> GetBoolValue() {
  //   return IsBoolean() ? std::make_pair(true, *m_value.m_boolean)
  //                      : std::make_pair(false, false);
  // }

  // std::pair<bool, boolean_t*> GetBoolValueRef() {
  //   return IsBoolean() ? std::make_pair(true, m_value.m_boolean)
  //                      : std::make_pair(false, false);
  // }

  // std::pair<bool, string_t> GetStringValue() {
  //   return IsString() ? std::make_pair(true, *m_value.m_string)
  //                     : std::make_pair(false, "");
  // }
 private:
  const char* GetTypeName() const noexcept {
    switch (m_type) {
      case value_t::null:
        return "null";
      case value_t::object:
        return "object";
      case value_t::array:
        return "array";
      case value_t::string:
        return "string";
      case value_t::boolean:
        return "boolean";
      case value_t::number_integer:
      case value_t::number_unsigned:
      case value_t::number_float:
      default:
        return "number";
    }
  }
  boolean_t GetImpl(boolean_t*) const {
    if (JSON_CONDITION_LIKELY(IsBoolean())) return m_value.m_boolean;
    JSON_THROW(
        TypeError(302, "type must be boolean,but is" << GetTypeName(), this));
  }

  std::optional<object_t*> GetImplPtr(object_t*) noexcept {
    return IsObject() ? m_value.m_object : std::nullopt;
  }

  constexpr std::optional<const object_t*> GetImplPtr(
      const object_t*) const noexcept {
    return IsObject() ? m_value.m_object : std::nullopt;
  }

  std::optional<array_t*> GetImplPtr(array_t*) noexcept {
    return IsArray() ? m_value.m_array : std::nullopt;
  }

  constexpr std::optional<const array_t*> GetImplPtr(
      const array_t*) const noexcept {
    return IsArray() ? m_value.m_array : std::nullopt;
  }

  std::optional<string_t*> GetImplPtr(string_t*) noexcept {
    return IsString() ? m_value.m_string : std::nullopt;
  }

  constexpr std::optional<const string_t*> GetImplPtr(
      const string_t*) const noexcept {
    return IsString() ? m_value.m_string : std::nullopt;
  }

  std::optional<boolean_t*> GetImplPtr(boolean_t*) noexcept {
    return IsBoolean() ? &m_value.m_boolean : std::nullopt;
  }

  constexpr std::optional<const boolean_t*> GetImplPtr(
      const boolean_t*) const noexcept {
    return IsBoolean() ? &m_value.m_boolean : std::nullopt;
  }

  std::optional<number_integer_t*> GetImplPtr(number_integer_t*) noexcept {
    return IsNumber() ? &m_value.m_integer : std::nullopt;
  }

  constexpr std::optional<const number_integer_t*> GetImplPtr(
      const number_integer_t*) const noexcept {
    return IsNumber() ? &m_value.m_integer : std::nullopt;
  }

  std::optional<number_unsigned_t*> GetImplPtr(number_unsigned_t*) noexcept {
    return IsUnsigned() ? &m_value.m_unsigned : std::nullopt;
  }

  constexpr std::optional<const number_unsigned_t*> GetImplPtr(
      const number_unsigned_t*) const noexcept {
    return IsUnsigned() ? &m_value.m_unsigned : std::nullopt;
  }

  std::optional<number_float_t*> GetImplPtr(number_float_t*) noexcept {
    return IsFloat() ? &m_value.m_float : std::nullopt;
  }

  constexpr std::optional<const number_float_t*> GetImplPtr(
      const number_float_t*) const noexcept {
    return IsFloat() ? &m_value.m_float : std::nullopt;
  }

  // template<typename ReferenceType,typename ThisType>
  // static ReferenceType GetRefImpl(ThisType& obj)
};