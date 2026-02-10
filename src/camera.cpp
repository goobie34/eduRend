#include "Camera.h"

using namespace linalg;

void Camera::MoveTo(const vec3f& position) noexcept
{
	m_position = position;
}

void Camera::Move(const vec3f& direction) noexcept
{
	m_position += direction;
}

void Camera::Rotate(const vec2f& rotation) noexcept
{
	m_rotation += rotation;
}

void Camera::RotateWithConstraint(const linalg::vec2f& rotation, const float constraint_upper, const float constraint_lower) noexcept
{
	float new_rotation_y = m_rotation.y + rotation.y;

	if (new_rotation_y < constraint_upper && new_rotation_y > constraint_lower) {
		m_rotation.y += rotation.y;
	}

	m_rotation.x += rotation.x;
}

mat4f Camera::RotationMatrix() const noexcept {
	return mat4f::rotation(0, m_rotation.x, m_rotation.y);
}

mat4f Camera::WorldToViewMatrix() const noexcept
{
	// Assuming a camera's position and rotation is defined by matrices T(p) and R,
	// the View-to-World transform is T(p)*R (for a first-person style camera).
	//
	// World-to-View then is the inverse of T(p)*R;
	//		inverse(T(p)*R) = inverse(R)*inverse(T(p)) = transpose(R)*T(-p)
	// Since now there is no rotation, this matrix is simply T(-p)

	mat4f rotation_matrix_transposed = RotationMatrix();
	rotation_matrix_transposed.transpose();

	return rotation_matrix_transposed * mat4f::translation(-m_position);
}

mat4f Camera::ViewToWorldMatrix() const noexcept
{
	mat4f view_to_world = mat4f::translation(m_position) * RotationMatrix();

	return view_to_world;

	// Assuming a camera's position and rotation is defined by matrices T(p) and R,
	// the View-to-World transform is T(p)*R (for a first-person style camera).
	//
	// World-to-View then is the inverse of T(p)*R;
	//		inverse(T(p)*R) = inverse(R)*inverse(T(p)) = transpose(R)*T(-p)
	// Since now there is no rotation, this matrix is simply T(-p)
}

mat4f Camera::ProjectionMatrix() const noexcept
{
	return mat4f::projection(m_vertical_fov, m_aspect_ratio, m_near_plane, m_far_plane);
}

vec3f Camera::Position() const noexcept {
	return m_position;
}