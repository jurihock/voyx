#pragma once

#include <voyx/Header.h>

namespace voyx
{
  template<typename T>
  class vector
  {

  public:

    // ITERATOR

    struct iterator
    {

    public:

      using iterator_category = std::forward_iterator_tag;
      using difference_type   = std::ptrdiff_t;
      using value_type        = T;
      using reference         = T&;
      using pointer           = T*;

      iterator(pointer p) : p(p) {}

      reference operator*() const { return *p; }
      pointer operator->()        { return p; }

      iterator& operator++()   { p++; return *this; }
      iterator operator++(int) { iterator i = *this; ++(*this); return i; }

      friend bool operator==(const iterator& a, const iterator& b) { return a.p == b.p; };
      friend bool operator!=(const iterator& a, const iterator& b) { return a.p != b.p; };

    private:

      pointer p;

    };

    // CONSTRUCTORS

    vector(T* data, const size_t size) :
      vector_data(data),
      vector_size(size)
    {
    }

    vector(std::span<T> other) :
      vector(other.data(), other.size())
    {
    }

    vector(std::vector<T>& other) :
      vector(other.data(), other.size())
    {
    }

    vector(voyx::vector<T>& other) :
      vector(other.data(), other.size())
    {
    }

    // CONST CONSTRUCTORS

    vector(const T* data, const size_t size) :
      vector((T*)data, size)
    {
    }

    vector(std::span<const T> other) :
      vector(other.data(), other.size())
    {
    }

    vector(const std::vector<T>& other) :
      vector(other.data(), other.size())
    {
    }

    vector(const voyx::vector<T>& other) :
      vector(other.data(), other.size())
    {
    }

    // BASICS

    iterator begin() const { return iterator(vector_data); }
    iterator end() const { return iterator(vector_data + vector_size); }

    T& front() { return vector_data[0]; }
    const T& front() const { return vector_data[0]; }

    T& back() { return vector_data[vector_size - 1]; }
    const T& back() const { return vector_data[vector_size - 1]; }

    T* const data() { return vector_data; }
    const T* data() const { return vector_data; }

    size_t size() const { return vector_size; }

    bool empty() const { return vector_size == 0; }

    // SUBSCRIPT OPERATORS

    const T& operator[](const size_t i) const
    {
      return vector_data[i];
    }

    T& operator[](const size_t i)
    {
      return vector_data[i];
    }

    // ASSIGNMENT OPERATORS (DEEP COPY)

    voyx::vector<T>& operator=(const T& other)
    {
      std::fill(this->data(), this->data() + this->size(), other);

      return *this;
    }

    voyx::vector<T>& operator=(const voyx::vector<T>& other)
    {
      if (this != &other)
      {
        std::copy(other.data(), other.data() + other.size(), this->data());
      }

      return *this;
    }

    // SCALAR OPERATORS

    voyx::vector<T>& operator+=(const T& other)
    {
      std::for_each(this->data(), this->data() + this->size(), [&](T& value) { value += other; });
      return *this;
    }

    voyx::vector<T>& operator-=(const T& other)
    {
      std::for_each(this->data(), this->data() + this->size(), [&](T& value) { value -= other; });
      return *this;
    }

    voyx::vector<T>& operator*=(const T& other)
    {
      std::for_each(this->data(), this->data() + this->size(), [&](T& value) { value *= other; });
      return *this;
    }

    voyx::vector<T>& operator/=(const T& other)
    {
      std::for_each(this->data(), this->data() + this->size(), [&](T& value) { value /= other; });
      return *this;
    }

    // VECTOR OPERATORS

    voyx::vector<T>& operator+=(const voyx::vector<T>& other)
    {
      std::transform(this->data(), this->data() + this->size(), other.data(), this->data(), std::plus<>{});
      return *this;
    }

    voyx::vector<T>& operator-=(const voyx::vector<T>& other)
    {
      std::transform(this->data(), this->data() + this->size(), other.data(), this->data(), std::minus<>{});
      return *this;
    }

    voyx::vector<T>& operator*=(const voyx::vector<T>& other)
    {
      std::transform(this->data(), this->data() + this->size(), other.data(), this->data(), std::multiplies<>{});
      return *this;
    }

    voyx::vector<T>& operator/=(const voyx::vector<T>& other)
    {
      std::transform(this->data(), this->data() + this->size(), other.data(), this->data(), std::divides<>{});
      return *this;
    }

  private:

    T* vector_data;
    size_t vector_size;

  };
}
