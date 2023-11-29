#pragma once

#include <chrono>

namespace green::core
{

class timer
{
public:
                    timer() noexcept;
    void            reset() noexcept;
    double          get_elapsed_msec() const noexcept;
    double          get_elapsed_sec() const noexcept;

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_last_time;
}; /* class timer */



inline timer::timer() noexcept
    : m_last_time(std::chrono::high_resolution_clock::now())
{
}

/* timer::start */
inline void timer::reset() noexcept
{
    m_last_time = std::chrono::high_resolution_clock::now();
}

/* timer::time_msec */
inline double timer::get_elapsed_msec() const noexcept
{
    using milisecond_type = std::chrono::duration<double, std::milli>;
    return std::chrono::duration_cast<milisecond_type>(std::chrono::high_resolution_clock::now() - m_last_time).count();
}

/* timer::time_sec */
inline double timer::get_elapsed_sec() const noexcept
{
    using second_type = std::chrono::duration<double, std::ratio<1>>;
    return std::chrono::duration_cast<second_type>(std::chrono::high_resolution_clock::now() - m_last_time).count();
}

} /* namespace green::core */
