#pragma once

#include <core/shared_ptr.hpp>
#include <core/types.hpp>

namespace green::core
{

/* T - the type of the cell */
template<class T>
class basic_matrix
{
    static_assert(sizeof(T) >= alignof(T));
    static_assert(sizeof(T) % alignof(T) == 0);
public:
    using cell_type = T;
    using pointer_type = cell_type*;
    using deleter_type = std::function<void(pointer_type)>;

public:
                    basic_matrix() noexcept;

                    basic_matrix(int32_t columns, int32_t rows) noexcept;

                    basic_matrix(int32_t columns, int32_t rows, pointer_type matrix_storage, int32_t bytes_per_row = 0, deleter_type deleter = nullptr) noexcept;

                    basic_matrix(const basic_matrix&) = default;

                    basic_matrix(basic_matrix&&) = default;

                    ~basic_matrix() noexcept = default;

    basic_matrix&   operator=(const basic_matrix&) = default;

    basic_matrix&   operator=(basic_matrix&&) = default;

    pointer_type    get_storage_ptr() const noexcept;

    pointer_type    get_row_ptr(int32_t row) const noexcept;

    pointer_type    get_cell_ptr(int32_t row, int32_t column) const noexcept;

    int32_t         get_bytes_per_row() const noexcept;

    int32_t         get_bytes_per_cell() const noexcept;

    int32_t         get_columns() const noexcept;

    int32_t         get_rows() const noexcept;

protected:
    int32_t         m_columns;
    int32_t         m_rows;
    int32_t         m_bytes_per_row;
    shared_ptr<cell_type>   m_storage;
};



template<class T>
basic_matrix<T>::basic_matrix() noexcept
    : m_columns(0)
    , m_rows(0)
    , m_bytes_per_row(0)
    , m_storage(nullptr)
{
}

template<class T>
basic_matrix<T>::basic_matrix(int32_t columns, int32_t rows) noexcept
    : m_columns(columns)
    , m_rows(rows)
    , m_bytes_per_row(columns * sizeof(T))
{
    auto* mem_ptr = static_cast<pointer_type>(std::aligned_alloc(alignof(T), m_bytes_per_row * m_rows));
    // auto* mem_ptr = static_cast<pointer_type>(std::malloc(m_bytes_per_row * m_rows));
    m_storage = shared_ptr<cell_type>(mem_ptr, [](pointer_type ptr) {std::free(ptr);});
}

template<class T>
basic_matrix<T>::basic_matrix(int32_t columns, int32_t rows, pointer_type matrix_storage, int32_t bytes_per_row, deleter_type deleter) noexcept
    : m_columns(columns)
    , m_rows(rows)
    , m_bytes_per_row(bytes_per_row == 0 ? columns * sizeof(T) : bytes_per_row)
    , m_storage(matrix_storage, deleter)
{
}

template<class T>
inline typename basic_matrix<T>::pointer_type basic_matrix<T>::get_storage_ptr() const noexcept
{
    return m_storage.get();
}

template<class T>
inline typename basic_matrix<T>::pointer_type basic_matrix<T>::get_row_ptr(int32_t row) const noexcept
{
    const auto shift_in_bytes = m_bytes_per_row * row;
    return static_cast<pointer_type>(static_cast<void*>(static_cast<uint8_t*>(static_cast<void*>(m_storage.get())) + shift_in_bytes));
}

template<class T>
inline typename basic_matrix<T>::pointer_type basic_matrix<T>::get_cell_ptr(int32_t row, int32_t column) const noexcept
{
    const auto shift_in_bytes = m_bytes_per_row * row + sizeof(T) * column;
    return static_cast<pointer_type>(static_cast<void*>(static_cast<uint8_t*>(static_cast<void*>(m_storage.get())) + shift_in_bytes));
}

template<class T>
inline int32_t basic_matrix<T>::get_bytes_per_row() const noexcept
{
    return m_bytes_per_row;
}

template<class T>
inline int32_t basic_matrix<T>::get_columns() const noexcept
{
    return m_columns;
}

template<class T>
inline int32_t basic_matrix<T>::get_rows() const noexcept
{
    return m_rows;
}

} /* namespace green::core */
