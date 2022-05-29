#pragma once

#include <voyx/Header.h>

namespace voyx
{
  template<typename T>
  class matrix
  {

  public:

    // ITERATOR

    struct iterator
    {

    public:

      using iterator_category = std::forward_iterator_tag;
      using difference_type   = std::ptrdiff_t;
      using value_type        = voyx::vector<T>;
      using reference         = voyx::vector<T>;
      using pointer           = voyx::vector<T>;

      iterator(T* p, size_t s) : p(p), s(s) {}

      reference operator*() const { return voyx::vector<T>(p, s); }
      pointer operator->()        { return voyx::vector<T>(p, s); }

      iterator& operator++()   { p+=s; return *this; }
      iterator operator++(int) { iterator i = *this; ++(*this); return i; }

      friend bool operator==(const iterator& a, const iterator& b) { return a.p == b.p; };
      friend bool operator!=(const iterator& a, const iterator& b) { return a.p != b.p; };

    private:

      T* p;
      size_t s;

    };

    // CONSTRUCTORS

    matrix(T* data, const size_t size, const size_t stride) :
      matrix_data(data),
      matrix_size(size / stride),
      matrix_stride(stride)
    {
    }

    matrix(std::span<T> other, const size_t stride) :
      matrix(other.data(), other.size(), stride)
    {
    }

    matrix(std::vector<T>& other, const size_t stride) :
      matrix(other.data(), other.size(), stride)
    {
    }

    matrix(voyx::vector<T>& other, const size_t stride) :
      matrix(other.data(), other.size(), stride)
    {
    }

    matrix(voyx::matrix<T>& other) :
      matrix(other.data(), other.size() * other.stride(), other.stride())
    {
    }

    // CONST CONSTRUCTORS

    matrix(const T* data, const size_t size, const size_t stride) :
      matrix((T*)data, size, stride)
    {
    }

    matrix(std::span<const T> other, const size_t stride) :
      matrix(other.data(), other.size(), stride)
    {
    }

    matrix(const std::vector<T>& other, const size_t stride) :
      matrix(other.data(), other.size(), stride)
    {
    }

    matrix(const voyx::vector<T>& other, const size_t stride) :
      matrix(other.data(), other.size(), stride)
    {
    }

    matrix(const voyx::matrix<T>& other) :
      matrix(other.data(), other.size() * other.stride(), other.stride())
    {
    }

    // BASICS

    iterator begin() const { return iterator(matrix_data, matrix_stride); }
    iterator end() const { return iterator(matrix_data + matrix_size * matrix_stride, matrix_stride); }

    voyx::vector<T> front() { return voyx::vector<T>(matrix_data, matrix_stride); }
    const voyx::vector<T> front() const { return voyx::vector<T>(matrix_data, matrix_stride); }

    voyx::vector<T> back() { return voyx::vector<T>(matrix_data + (matrix_size - 1) * matrix_stride, matrix_stride); }
    const voyx::vector<T> back() const { return voyx::vector<T>(matrix_data + (matrix_size - 1) * matrix_stride, matrix_stride); }

    T* const data() { return matrix_data; }
    const T* data() const { return matrix_data; }

    size_t size() const { return matrix_size; }
    size_t stride() const { return matrix_stride; }

    bool empty() const { return matrix_size == 0 || matrix_stride == 0; }

    // SUBSCRIPT OPERATORS

    const vector<T> operator[](const size_t i) const
    {
      return vector<T>(matrix_data + i * matrix_stride, matrix_stride);
    }

    vector<T> operator[](const size_t i)
    {
      return vector<T>(matrix_data + i * matrix_stride, matrix_stride);
    }

    // CALL OPERATORS

    const T& operator()(const size_t i, const size_t j) const
    {
      return matrix_data[i * matrix_stride + j];
    }

    T& operator()(const size_t i, const size_t j)
    {
      return matrix_data[i * matrix_stride + j];
    }

    // ASSIGNMENT OPERATORS (DEEP COPY)

    voyx::matrix<T>& operator=(const T& other)
    {
      std::fill(this->data(), this->data() + this->size(), other);

      return *this;
    }

    voyx::matrix<T>& operator=(const voyx::matrix<T>& other)
    {
      if (this != &other)
      {
        std::copy(other.data(), other.data() + other.size() * other.stride(), this->data());
      }

      return *this;
    }

  private:

    T* matrix_data;
    size_t matrix_size;   // rows
    size_t matrix_stride; // cols

  };
}
