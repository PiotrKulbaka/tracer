#pragma once

#include <core/math.hpp>

namespace green
{

using ::green::core::fvec3;
using ::green::core::fmat4;
using ::green::core::fquat;

class camera
{
public:
                    camera() noexcept;
                    camera(const fvec3& pos, const fvec3& dir, const fvec3& up) noexcept;

    void            move(const fvec3& delta) noexcept;
    void            rotate(const fquat& q) noexcept;
    void            set_perspective_projection(float fov, float ratio, float near_plane, float far_plane) noexcept;
    void            set_orientation(const fvec3& dir, const fvec3& up) noexcept;

    void            set_direction(const fvec3& dir) noexcept;
    const fvec3 &   get_direction() const noexcept;
    void            set_up(const fvec3& up) noexcept;
    const fvec3 &   get_up() const noexcept;
    const fvec3 &   get_right() const noexcept;

    void            set_position( const fvec3& pos ) noexcept;
    const fvec3     get_position() const noexcept;
    void            set_scale( const fvec3& scale ) noexcept;
    const fvec3 &   get_scale() const noexcept;

    fmat4           operator()() noexcept;

private:
    fmat4           m_out;                      /* out camera matrix */
    fmat4           m_projection_mat;           /* ortohonal or perspective projection */
    fmat4           m_mat;                      /* transform matrix */
    fvec3 &         m_right {m_mat.x.vec3()};   /* camera right vector */
    fvec3 &         m_up {m_mat.y.vec3()};      /* right normal vector */
    fvec3 &         m_dir {m_mat.z.vec3()};     /* camera direction */
    fvec3           m_pos;                      /* camera position */
    fvec3           m_scale;                    /* scale camera */
    bool            m_need_update;              /* need update out */
};

} /* namespace engine */