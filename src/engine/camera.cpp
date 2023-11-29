#include "camera.hpp"

namespace green
{

/* camera::camera */
camera::camera() noexcept
    : m_out(1.0)
    , m_projection_mat(0.0)
    , m_mat()
    , m_right{m_mat.x.vec3()}
    , m_up{m_mat.y.vec3()}
    , m_dir{m_mat.z.vec3()}
    , m_pos(0.0)
    , m_scale(1.0)
    , m_need_update(true)
{
    m_projection_mat.x.x = 1.0;
    m_projection_mat.y.z = 1.0;
    m_projection_mat.z.y = 1.0;
    m_projection_mat.w.w = 1.0;
}

/* camera::camera */
camera::camera(const fvec3& pos, const fvec3& dir, const fvec3& up) noexcept
    : camera()
{
    set_position(pos);
    set_direction(dir);
    set_up(up);
}

/* camera::move */
void camera::move(const fvec3& delta) noexcept
{
    m_pos -= delta;
    m_need_update = true;
}

/* camera::rotate */
void camera::rotate(const fquat &q) noexcept
{
    set_direction(q * get_direction());
    set_up(q * get_up());
    m_need_update = true;
}

/* camera::set_perspective_projection */
void camera::set_perspective_projection(float fov, float ratio, float near_plane, float far_plane) noexcept
{
    m_projection_mat = fmat4::perspective(fov, ratio, near_plane, far_plane);
    m_need_update = true;
}

/* camera::set_orientation */
void camera::set_orientation(const fvec3 &dir, const fvec3 &up) noexcept
{
    m_dir = dir;
    m_dir.normalize();
    m_up = up;
    m_up.normalize();
    m_right = m_dir.cross(m_up);
    m_up = m_right.cross(m_dir);
    m_need_update = true;
}

/* camera::set_direction */
void camera::set_direction(const fvec3 &dir) noexcept
{
    m_dir = dir;
    m_dir.normalize();
    /* update right */
    m_right = m_dir.cross( m_up );
    /* update up */
    m_up = m_right.cross( m_dir );
    m_need_update = true;
}

/* camera::get_direction */
const fvec3& camera::get_direction() const noexcept
{
    return m_dir;
}

/* camera::set_up */
void camera::set_up(const fvec3& up) noexcept
{
    m_up = up;
    m_up.normalize();
    /* update right */
    m_right = m_dir.cross( m_up );
    /* update up */
    m_up = m_right.cross( m_dir );
    m_need_update = true;
}

/* camera::get_up */
const fvec3& camera::get_up() const noexcept
{
    return m_up;
}

/* camera::get_right */
const fvec3& camera::get_right() const noexcept
{
    return m_right;
}

/* camera::set_position */
void camera::set_position(const fvec3& pos) noexcept
{
    m_pos = -pos;
    m_need_update = true;
}

/* camera::get_position */
const fvec3 camera::get_position() const noexcept
{
    return -m_pos;
}

/* camera::set_scale */
void camera::set_scale(const fvec3& scale) noexcept
{
    m_scale = scale;
    m_need_update = true;
}

/* camera::get_scale */
const fvec3& camera::get_scale() const noexcept
{
    return m_scale;
}

/* camera::get_scale */
fmat4 camera::operator()() noexcept
{
    if (m_need_update) {
        m_out = m_mat;
        /* scaling */
        m_out.x.x *= m_scale.x;
        m_out.y.y *= m_scale.y;
        m_out.z.z *= m_scale.z;
        /* optimization (multiply camera matrix on position matrix) */
        m_out.x.w = m_right.x * m_pos.x + m_right.y * m_pos.y + m_right.z * m_pos.z;
        m_out.y.w = m_up.x * m_pos.x + m_up.y * m_pos.y + m_up.z * m_pos.z;
        m_out.z.w = m_dir.x * m_pos.x + m_dir.y * m_pos.y + m_dir.z * m_pos.z;
        m_out = m_projection_mat * m_out;
        m_need_update = false;
    }
    return m_out;
}

} /* namespace green */
