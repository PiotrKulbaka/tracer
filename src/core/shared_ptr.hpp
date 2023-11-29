#pragma once

#include <memory>

namespace green::core
{

template <typename T>
class shared_ptr : public std::shared_ptr<T>
{
public:
    /* inherit all constructors */
    using std::shared_ptr<T>::shared_ptr;
}; /* class shared_ptr */

} /* namespace green::core */
