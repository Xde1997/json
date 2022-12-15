#pragma once
#include <memory>

template <typename TAllocatorType>
class Alloc {
 public:
  template <typename TSupportSingleType, typename... Args>
  static TSupportSingleType* create(Args&&... args) {
    using AllocatorTraits = std::allocator_traits<TAllocatorType>;

    auto deleter = [&](TSupportSingleType* obj) {
      AllocatorTraits::deallocate(s_alloc, obj, 1);
    };
    std::unique_ptr<TSupportSingleType, decltype(deleter)> obj(
        AllocatorTraits::allocate(s_alloc, 1), deleter);
    AllocatorTraits::construct(s_alloc.get(), std::forward<Args>(args)...);
    static_assert(obj != nullptr);
    return obj.release();
  }

 private:
  static TAllocatorType s_alloc;
};