#include "camera.hpp"

#include "imgui.h"

#include <algorithm>

camera::camera(float aspectRatio, float fov):
	_position(dx::XMVectorSet(2.0f, 2.0f, 2.0f, 1.0f)),
	_target(dx::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f)),
	_up(dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)),
	_aspectRatio(aspectRatio),
	_fov(fov),
	_nearPlane(0.1f),
	_farPlane(100.0f),
	_model(dx::XMMatrixRotationX(-dx::XM_PIDIV2)),
	_view(dx::XMMatrixLookAtLH(_position, _target, _up)),
	_projection(dx::XMMatrixPerspectiveLH(_fov, _aspectRatio, _nearPlane, _farPlane)),
	_mvp(dx::XMMatrixTranspose(_model * _view * _projection)),
	_tool(toolType::NONE),
	_proj(projType::PERSPECTIVE)
{}

void camera::drawUI() noexcept
{
	const int toolCount = _countof(_toolNames);
	for (int i = toolType::NONE+1; i < toolCount; ++i)
	{
		const bool isSelected = (_tool == static_cast<toolType>(i));
		if (isSelected)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 1.0f)); // Green
		}

		if (ImGui::Button(_toolNames[i]))
		{
			_tool = static_cast<toolType>(i);
		}

		if (isSelected)
		{
			ImGui::PopStyleColor();
		}
		ImGui::SameLine();
	}
	ImGui::NewLine();
}

void camera::mouseClickDrag(float dx, float dy) noexcept
{
	switch (_tool)
	{
		case toolType::NONE:
			return;
		case toolType::ORBIT:
		{
			orbit((0.004f) * dy, -(0.004f) * dx);
			return;
		}
		case toolType::PAN:
		{
			pan(-(0.003f) * dx, (0.003f) * dy);
			return;
		}
		case toolType::LOOKAROUND:
		{
			lookAround(-(0.004f) * dx, (0.004f) * dy);
			return;
		}
		case toolType::ZOOM:
		{
			const float d = std::min(dx, dy);
			forwardBackward((0.004f) * d);
			return;
		}
	}
}

void camera::orbit(float dTheta, float dPhi) noexcept
{
	const dx::XMVECTOR offset = dx::XMVectorSubtract(_position, _target);
	const float x = dx::XMVectorGetX(offset);
	const float y = dx::XMVectorGetY(offset);
	const float z = dx::XMVectorGetZ(offset);
	const float r = dx::XMVectorGetX(dx::XMVector3LengthEst(offset));
	const float xzSquaredSum = (x * x) + (z * z);
	const float xzLength = static_cast<float>(sqrt(static_cast<double>(xzSquaredSum)));

	dx::XMVECTOR angles;
	if (fabs(x) > 0.01f) {
		angles = dx::XMVectorATan2Est(
			dx::XMVectorSet(z, y, 0.0f, 0.0f),
			dx::XMVectorSet(x, xzLength, 1.0f, 1.0f)
		);
	}
	else {
		angles = dx::XMVectorSet(
			z > 0 ? dx::XM_PIDIV2 : (z < 0 ? -dx::XM_PIDIV2 : 0.0f),
			dx::XMVectorGetX(dx::XMVectorATan2Est(
				dx::XMVectorReplicate(y),
				dx::XMVectorReplicate(xzLength)
			)),
			0.0f,
			0.0f
		);
	}

	float phi = dx::XMVectorGetX(angles) + dPhi;
	float theta = dx::XMVectorGetY(angles) + dTheta;

	phi = dx::XMScalarModAngle(phi);
	constexpr float THETA_EPSILON = 0.1f;
	theta = std::clamp(
		theta,
		-dx::XM_PIDIV2 + THETA_EPSILON,
		dx::XM_PIDIV2 - THETA_EPSILON
	);

	const float cosTheta = dx::XMScalarCosEst(theta);
	const dx::XMVECTOR newPos = dx::XMVectorSet(
		r * cosTheta * dx::XMScalarCosEst(phi),
		r * dx::XMScalarSinEst(theta),
		r * cosTheta * dx::XMScalarSinEst(phi),
		1.0f
	);

	_position = dx::XMVectorAdd(_target, newPos);
	_view = dx::XMMatrixLookAtLH(_position, _target, _up);
	_mvp = dx::XMMatrixTranspose(_model * _view * _projection);
}

void camera::pan(float dx, float dy) noexcept
{
	const dx::XMVECTOR forward = dx::XMVector3NormalizeEst(
		dx::XMVectorSubtract(_position, _target)
	);
	const dx::XMVECTOR right = dx::XMVector3NormalizeEst(
		dx::XMVector3Cross(forward, _up)
	);
	const dx::XMVECTOR actualUp = dx::XMVector3Cross(right, forward);
	const dx::XMVECTOR movement = dx::XMVectorAdd(
		dx::XMVectorScale(right, dx),
		dx::XMVectorScale(actualUp, dy)
	);
	_position = dx::XMVectorAdd(_position, movement);
	_target = dx::XMVectorAdd(_target, movement);
	_view = dx::XMMatrixLookAtLH(_position, _target, _up);
	_mvp = dx::XMMatrixTranspose(_model * _view * _projection);
}

void camera::forwardBackward(float factor) noexcept
{
	const dx::XMVECTOR offset = dx::XMVectorSubtract(_position, _target);
	_position = dx::XMVectorAdd(_target, dx::XMVectorScale(offset, 1.0f + factor));
	_view = dx::XMMatrixLookAtLH(_position, _target, _up);
	_mvp = dx::XMMatrixTranspose(_model * _view * _projection);
}

void camera::lookAround(float dYaw, float dPitch) noexcept
{
	dx::XMMATRIX yawRotation = dx::XMMatrixRotationY(dYaw);
	dx::XMMATRIX pitchRotation = dx::XMMatrixRotationAxis(dx::XMVector3Cross(_up, dx::XMVectorSubtract(_position, _target)), dPitch);

	dx::XMVECTOR direction = dx::XMVectorSubtract(_target, _position);
	direction = dx::XMVector3TransformNormal(direction, yawRotation);
	direction = dx::XMVector3TransformNormal(direction, pitchRotation);

	dx::XMVECTOR newUp = dx::XMVector3TransformNormal(_up, yawRotation);
	newUp = dx::XMVector3TransformNormal(newUp, pitchRotation);

	_target = dx::XMVectorAdd(_position, direction);
	_up = newUp;
	_view = dx::XMMatrixLookAtLH(_position, _target, _up);
	_mvp = dx::XMMatrixTranspose(_model * _view * _projection);
}

void camera::updateAspectRatio(float newAspectRatio)
{
	_aspectRatio = newAspectRatio;
	_projection = dx::XMMatrixPerspectiveFovLH(_fov, _aspectRatio, _nearPlane, _farPlane);
	_mvp = dx::XMMatrixTranspose(_model * _view * _projection);
}
