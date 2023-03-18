#pragma once

#include <memory>
#include <stdexcept>
#include <iterator>

class VectorOutOfRange : public std::out_of_range {
 public:
  VectorOutOfRange() : std::out_of_range("VectorOutOfRange") {
  }
};

template <typename T>
class Vector {
  const size_t k_capacity_multiplier_ = 2;
  std::unique_ptr<T[]> mas_;
  size_t size_;
  size_t capacity_;

 public:
  using ValueType = T;
  using Pointer = T*;
  using ConstPointer = const T*;
  using Reference = T&;
  using ConstReference = const T&;
  using SizeType = size_t;

  class Iterator {
   private:
    ValueType* ptr_;

   public:
    typedef std::random_access_iterator_tag iterator_category;  // NOLINT
    typedef T value_type;                                       // NOLINT
    typedef size_t difference_type;                             // NOLINT
    typedef T* pointer;                                         // NOLINT
    typedef T& reference;                                       // NOLINT

    Iterator(T* ptr) : ptr_(ptr) {  // NOLINT
    }

    T& operator*() {
      return *ptr_;
    }

    T* operator->() {
      return ptr_;
    }

    Iterator& operator++() {
      ++ptr_;
      return *this;
    }

    Iterator& operator--() {
      --ptr_;
      return *this;
    }

    Iterator operator+(difference_type value) const {
      return Iterator(ptr_ + value);
    }

    Iterator operator-(difference_type value) const {
      return Iterator(ptr_ - value);
    }

    bool operator==(const Iterator& other) const {
      return ptr_ == other.ptr_;
    }

    bool operator!=(const Iterator& other) const {
      return ptr_ != other.ptr_;
    }
  };

  class ConstIterator {
   private:
    ValueType* ptr_;

   public:
    typedef std::random_access_iterator_tag iterator_category;  // NOLINT
    typedef T value_type;                                       // NOLINT
    typedef size_t difference_type;                             // NOLINT
    typedef T* pointer;                                         // NOLINT
    typedef T& reference;                                       // NOLINT

    ConstIterator(T* ptr) : ptr_(ptr) {  // NOLINT
    }

    T& operator*() const {
      return *ptr_;
    }

    T* operator->() const {
      return ptr_;
    }

    ConstIterator& operator++() {
      ++ptr_;
      return *this;
    }

    ConstIterator& operator--() {
      --ptr_;
      return *this;
    }

    ConstIterator operator+(difference_type value) const {
      return Iterator(ptr_ + value);
    }

    ConstIterator operator-(difference_type value) const {
      return Iterator(ptr_ - value);
    }

    bool operator==(const ConstIterator& other) const {
      return ptr_ == other.ptr_;
    }

    bool operator!=(const ConstIterator& other) const {
      return ptr_ != other.ptr_;
    }
  };
  using ReverseIterator = std::reverse_iterator<Iterator>;
  using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

  Iterator begin() {  // NOLINT
    return Iterator(mas_.get());
  }

  ConstIterator begin() const {  // NOLINT
    return ConstIterator(mas_.get());
  }

  Iterator end() {  // NOLINT
    return Iterator(mas_.get() + size_);
  }

  ConstIterator end() const {  // NOLINT
    return ConstIterator(mas_.get() + size_);
  }

  ConstIterator cbegin() const {  // NOLINT
    return ConstIterator(mas_.get());
  }

  ConstIterator cend() const {  // NOLINT
    return ConstIterator(mas_.get() + size_);
  }

  ReverseIterator rbegin() {  // NOLINT
    return ReverseIterator(mas_.get() + size_);
  }

  ConstReverseIterator rbegin() const {  // NOLINT
    return ConstReverseIterator(mas_.get() + size_);
  }

  ReverseIterator rend() {  // NOLINT
    return ReverseIterator(mas_.get());
  }

  ConstReverseIterator rend() const {  // NOLINT
    return ConstReverseIterator(mas_.get());
  }

  ConstReverseIterator crbegin() const {  // NOLINT
    return ConstReverseIterator(mas_.get() + size_);
  }

  ConstReverseIterator crend() const {  // NOLINT
    return ConstReverseIterator(mas_.get());
  }

 public:
  Vector() noexcept : mas_(nullptr), size_(0), capacity_(0) {  // NOLINT
  }

  explicit Vector(const SizeType n) {  // NOLINT
    size_ = n;
    capacity_ = n;

    if (n == 0) {
      return;
    }
    try {
      mas_ = std::make_unique<T[]>(n);
    } catch (...) {
      mas_.reset();
      throw;
    }
  }

  Vector(const SizeType n, ValueType value) {  // NOLINT
    size_ = n;
    capacity_ = n;

    if (n == 0) {
      return;
    }
    try {
      mas_ = std::make_unique<T[]>(n);
      for (SizeType i = 0; i < n; ++i) {
        mas_[i] = value;
      }
    } catch (...) {
      mas_.reset();
      throw;
    }
  }

  template <class Iterator,
            class = std::enable_if_t<std::is_base_of_v<
                std::forward_iterator_tag, typename std::iterator_traits<Iterator>::iterator_category>>>  // NOLINT
  Vector(Iterator first, Iterator last) {                                                                 // NOLINT
    size_ = last - first;
    capacity_ = size_;

    if (size_ == 0) {
      return;
    }
    try {
      mas_ = std::make_unique<T[]>(size_);
      std::copy(first, last, mas_.get());
    } catch (...) {
      mas_.reset();
      throw;
    }
  }

  Vector(const std::initializer_list<ValueType>& init) : size_(init.size()), capacity_(init.size()) {  // NOLINT
    if (init.size() == 0) {
      return;
    }
    try {
      mas_ = std::make_unique<T[]>(init.size());
      SizeType i = 0;
      for (auto it = init.begin(); it != init.end(); ++it, ++i) {
        mas_[i] = *it;
      }
    } catch (...) {
      mas_.reset();
      throw;
    }
  }

  Vector(const Vector<ValueType>& other) {  // NOLINT
    try {
      size_ = other.size_;
      capacity_ = other.capacity_;
      if (size_ != 0) {
        mas_ = std::make_unique<T[]>(size_);
        for (size_t i = 0; i < size_; ++i) {
          mas_[i] = other.mas_[i];
        }
      }
    } catch (...) {
      mas_.reset();
      throw;
    }
  }

  Vector(Vector<ValueType>&& other) {  // NOLINT
    try {
      size_ = other.size_;
      capacity_ = other.capacity_;

      if (size_ != 0) {
        mas_ = std::move(other.mas_);
      }

      other.mas_.reset();
      other.size_ = 0;
      other.capacity_ = 0;
    } catch (...) {
      mas_.reset();
      throw;
    }
  }

  Vector<ValueType>& operator=(const Vector<ValueType>& other) {
    if (this == &other) {
      return *this;
    }

    size_ = other.size_;
    capacity_ = other.capacity_;
    if (size_ != 0) {
      mas_ = std::make_unique<T[]>(size_);
      for (size_t i = 0; i < size_; ++i) {
        mas_[i] = other.mas_[i];
      }
    }

    return *this;
  }

  Vector<ValueType>& operator=(Vector<ValueType>&& other) {
    try {
      if (this == &other) {
        return *this;
      }

      size_ = other.size_;
      capacity_ = other.capacity_;

      if (size_ != 0) {
        mas_ = std::move(other.mas_);
      }

      other.mas_.reset();
      other.size_ = 0;
      other.capacity_ = 0;
      return *this;
    } catch (...) {
      mas_.reset();
      throw;
    }
  }

  void Reserve(SizeType n) {
    if (n == 0 || capacity_ >= n) {
      return;
    }

    auto temp = std::make_unique<T[]>(n);

    for (size_t i = 0; i < size_; ++i) {
      if constexpr (std::is_move_assignable_v<T>) {
        temp[i] = std::move(mas_[i]);
      } else {
        temp[i] = mas_[i];
      }
    }
    capacity_ = n;
    mas_ = std::move(temp);
  }

  SizeType Size() const noexcept {
    return size_;
  }

  SizeType Capacity() const noexcept {
    return capacity_;
  }

  bool Empty() const noexcept {
    return size_ == 0;
  }

  Reference operator[](size_t index) noexcept {
    return mas_[index];
  }
  const ValueType& operator[](size_t index) const noexcept {
    return mas_[index];
  }

  Reference At(size_t index) {
    if (index >= size_ || index < 0) {
      throw VectorOutOfRange{};
    }
    return mas_[index];
  }

  ConstReference At(size_t index) const {
    if (index >= size_ || index < 0) {
      throw VectorOutOfRange{};
    }
    return mas_[index];
  }

  Reference Front() {
    return mas_[0];
  }

  ConstReference Front() const noexcept {
    return mas_[0];
  }

  Reference Back() noexcept {
    return mas_[size_ - 1];
  }

  ConstReference Back() const noexcept {
    return mas_[size_ - 1];
  }

  Pointer Data() noexcept {
    return mas_.get();
  }

  ConstPointer Data() const noexcept {
    return mas_.get();
  }

  void Swap(Vector<ValueType>& other) {
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
    std::swap(mas_, other.mas_);
  }

  void Resize(const SizeType n) {
    if (n == 0 || capacity_ >= n) {
      size_ = n;
      return;
    }

    try {
      auto temp = std::make_unique<T[]>(n);
    } catch (...) {
      throw;
    }

    auto temp = std::make_unique<T[]>(n);
    for (SizeType i = 0; i < std::min(n, size_); ++i) {
      if constexpr (std::is_move_assignable_v<T>) {
        temp[i] = std::move(mas_[i]);
      } else {
        temp[i] = mas_[i];
      }
    }

    size_ = n;
    capacity_ = n;
    mas_ = std::move(temp);
    temp.reset();
  }

  void Resize(const SizeType n, const ValueType value) {
    if (n == 0) {
      return;
    }
    if (capacity_ >= n) {
      for (SizeType i = size_; i < n; ++i) {
        if constexpr (std::is_move_assignable_v<T>) {
          mas_[i] = std::move(value);
        } else {
          mas_[i] = value;
        }
      }
      size_ = n;
      return;
    }

    try {
      auto temp = std::make_unique<T[]>(n);
    } catch (...) {
      throw;
    }

    auto temp = std::make_unique<T[]>(n);
    for (SizeType i = 0; i < std::min(n, size_); ++i) {
      if constexpr (std::is_move_assignable_v<T>) {
        temp[i] = std::move(mas_[i]);
      } else {
        temp[i] = mas_[i];
      }
    }
    for (SizeType i = size_; i < n; ++i) {
      if constexpr (std::is_move_assignable_v<T>) {
        temp[i] = std::move(value);
      } else {
        temp[i] = value;
      }
    }
    mas_ = std::move(temp);
    size_ = n;
    capacity_ = n;
  }

  void ShrinkToFit() {
    try {
      auto temp = std::make_unique<T[]>(size_);
    } catch (...) {
      throw;
    }
    auto temp = std::make_unique<T[]>(size_);
    for (size_t i = 0; i < size_; ++i) {
      if constexpr (std::is_move_assignable_v<T>) {
        temp[i] = std::move(mas_[i]);
      } else {
        temp[i] = mas_[i];
      }
    }
    mas_ = std::move(temp);
    capacity_ = size_;
    if (capacity_ == 0) {
      mas_.reset();
    }
  }

  void Clear() {
    try {
      while (size_ != 0) {
        PopBack();
      }
    } catch (...) {
      mas_.reset();
      throw;
    }
  }

  void PopBack() {
    try {
      mas_[size_ - 1] = ValueType();
      --size_;
    } catch (...) {
      mas_.reset();
      throw;
    }
  }

  void PushBack(const ValueType& value) {
    if (size_ == capacity_) {
      size_t n = std::max(static_cast<SizeType>(1), size_ * k_capacity_multiplier_);
      try {
        auto temp = std::make_unique<T[]>(n);
      } catch (...) {
        throw;
      }
      auto temp = std::make_unique<T[]>(n);
      for (size_t i = 0; i < size_; ++i) {
        if constexpr (std::is_move_assignable_v<T>) {
          temp[i] = std::move(mas_[i]);
        } else {
          temp[i] = mas_[i];
        }
      }
      capacity_ = n;
      mas_ = std::move(temp);
    }
    mas_[size_] = value;
    ++size_;
  }

  void PushBack(ValueType&& value) {
    if (size_ == capacity_) {
      size_t n = std::max(static_cast<SizeType>(1), size_ * k_capacity_multiplier_);
      try {
        auto temp = std::make_unique<T[]>(n);
      } catch (...) {
        throw;
      }
      auto temp = std::make_unique<T[]>(n);
      for (size_t i = 0; i < size_; ++i) {
        if constexpr (std::is_move_assignable_v<T>) {
          temp[i] = std::move(mas_[i]);
        } else {
          temp[i] = mas_[i];
        }
      }
      capacity_ = n;
      mas_ = std::move(temp);
    }
    mas_[size_] = std::move(value);
    ++size_;
  }
};

template <typename T>
bool operator==(const Vector<T>& lhs, const Vector<T>& rhs) noexcept {
  if (lhs.Size() != rhs.Size()) {
    return false;
  }
  for (size_t i = 0; i < std::min(lhs.Size(), rhs.Size()); ++i) {
    if (lhs[i] != rhs[i]) {
      return false;
    }
  }
  return true;
}

template <typename T>
bool operator!=(const Vector<T>& lhs, const Vector<T>& rhs) noexcept {
  return !(lhs == rhs);
}

template <typename T>
bool operator<(const Vector<T>& lhs, const Vector<T>& rhs) noexcept {
  for (size_t i = 0; i < std::min(lhs.Size(), rhs.Size()); ++i) {
    if (lhs[i] != rhs[i]) {
      return lhs[i] < rhs[i];
    }
  }
  return lhs.Size() < rhs.Size();
}

template <typename T>
bool operator<=(const Vector<T>& lhs, const Vector<T>& rhs) noexcept {
  for (size_t i = 0; i < std::min(lhs.Size(), rhs.Size()); ++i) {
    if (lhs[i] != rhs[i]) {
      return lhs[i] < rhs[i];
    }
  }
  return lhs.Size() <= rhs.Size();
}

template <typename T>
bool operator>(const Vector<T>& lhs, const Vector<T>& rhs) noexcept {
  for (size_t i = 0; i < std::min(lhs.Size(), rhs.Size()); ++i) {
    if (lhs[i] != rhs[i]) {
      return lhs[i] > rhs[i];
    }
  }
  return lhs.Size() > rhs.Size();
}

template <typename T>
bool operator>=(const Vector<T>& lhs, const Vector<T>& rhs) noexcept {
  for (size_t i = 0; i < std::min(lhs.Size(), rhs.Size()); ++i) {
    if (lhs[i] != rhs[i]) {
      return lhs[i] >= rhs[i];
    }
  }
  return lhs.Size() >= rhs.Size();
}

template <typename _Iterator>                                       // NOLINT
struct iterator_traits {                                            // NOLINT
  typedef typename _Iterator::iterator_category iterator_category;  // NOLINT
  typedef typename _Iterator::value_type value_type;                // NOLINT
  typedef typename _Iterator::difference_type difference_type;      // NOLINT
  typedef typename _Iterator::pointer pointer;                      // NOLINT
  typedef typename _Iterator::reference reference;                  // NOLINT
};
