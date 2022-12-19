#pragma once

#include <functional>
#include <cstdint>

/* T - the type of the cell
* Amount - the number of elements of T in a cell */
template<class T, std::size_t Amount = 1>
class basic_matrix
{
public:
    using cell_type = T;
    using pointer_type = cell_type*;
    using deleter_type = std::function<void(pointer_type)>;

public:
                    basic_matrix(int32_t columns, int32_t rows) noexcept;

                    basic_matrix(int32_t columns, int32_t rows, pointer_type matrix_storage, int32_t bytes_per_row = 0, int32_t bytes_per_cell = 0, deleter_type deleter = nullptr) noexcept;

                    basic_matrix(const basic_matrix&&) = default;

                    ~basic_matrix() noexcept;

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
    int32_t         m_bytes_per_cell;
    pointer_type    m_storage;
    deleter_type    m_deleter;
};



template<class T, std::size_t Amount>
basic_matrix<T, Amount>::basic_matrix(int32_t columns, int32_t rows) noexcept
    : m_columns(columns)
    , m_rows(rows)
{
    m_bytes_per_cell = (alignof(T) > sizeof(T) ? alignof(T) : sizeof(T)) * Amount;
    m_bytes_per_row = m_columns * m_bytes_per_cell;
    m_storage = new T[m_bytes_per_row * m_rows];
    m_deleter = [](pointer_type ptr) {
        delete[] ptr;
    };
}

template<class T, std::size_t Amount>
basic_matrix<T, Amount>::basic_matrix(int32_t columns, int32_t rows, pointer_type matrix_storage, int32_t bytes_per_row, int32_t bytes_per_cell, deleter_type deleter) noexcept
    : m_columns(columns)
    , m_rows(rows)
    , m_bytes_per_row(bytes_per_row)
    , m_bytes_per_cell(bytes_per_cell)
    , m_storage(matrix_storage)
    , m_deleter(deleter)
{
    if (bytes_per_cell == 0) {
        m_bytes_per_cell = (alignof(T) > sizeof(T) ? alignof(T) : sizeof(T)) * Amount;
    }
    if (m_bytes_per_row == 0) {
        m_bytes_per_row = m_columns * m_bytes_per_cell;
    }
}

template<class T, std::size_t Amount>
basic_matrix<T, Amount>::~basic_matrix() noexcept
{
    if (m_deleter) {
        m_deleter(m_storage);
    }
    m_storage = nullptr;
}

template<class T, std::size_t Amount>
inline typename basic_matrix<T, Amount>::pointer_type basic_matrix<T, Amount>::get_storage_ptr() const noexcept
{
    return m_storage;
}

template<class T, std::size_t Amount>
inline typename basic_matrix<T, Amount>::pointer_type basic_matrix<T, Amount>::get_row_ptr(int32_t row) const noexcept
{
    const auto shift_in_bytes = m_bytes_per_row * row;
    return static_cast<pointer_type>(static_cast<uint8_t*>(static_cast<void*>(m_storage)) + shift_in_bytes);
}

template<class T, std::size_t Amount>
inline typename basic_matrix<T, Amount>::pointer_type basic_matrix<T, Amount>::get_cell_ptr(int32_t row, int32_t column) const noexcept
{
    const auto shift_in_bytes = m_bytes_per_row * row + m_bytes_per_cell * column;
    return static_cast<pointer_type>(static_cast<uint8_t*>(static_cast<void*>(m_storage)) + shift_in_bytes);
}

template<class T, std::size_t Amount>
inline int32_t basic_matrix<T, Amount>::get_bytes_per_row() const noexcept
{
    return m_bytes_per_row;
}

template<class T, std::size_t Amount>
inline int32_t basic_matrix<T, Amount>::get_bytes_per_cell() const noexcept
{
    return m_bytes_per_cell;
}

template<class T, std::size_t Amount>
inline int32_t basic_matrix<T, Amount>::get_columns() const noexcept
{
    return m_columns;
}

template<class T, std::size_t Amount>
inline int32_t basic_matrix<T, Amount>::get_rows() const noexcept
{
    return m_rows;
}
