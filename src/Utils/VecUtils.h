#ifndef EB_UTILS_VECUTILS_H
#define EB_UTILS_VECUTILS_H

#include <glm/glm.hpp>

namespace eb {

template<typename T, typename U>
T moveVec(const T &vec, const U &x, const U &y, const U &z)
{
    return vec + T{x, y, z};
}

template<typename T, typename U>
T moveVecX(const T &vec, const U &x)
{
    return vec + T{x, 0, 0};
}

template<typename T, typename U>
T moveVecY(const T &vec, const U &y)
{
    return vec + T{0, y, 0};
}

template<typename T, typename U>
T moveVecZ(const T &vec, const U &z)
{
    return vec + T{0, 0, z};
}

} // namespace eb

#endif // EB_UTILS_VECUTILS_H
