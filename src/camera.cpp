#include "camera.hpp"
#include <algorithm>

camera::camera(float aspectRatio, float fov):
	_position(dx::XMVectorSet(2.0f, 2.0f, 2.0f, 1.0f)),
	_target(dx::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f)),
	_up(dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)),
	_aspectRatio(aspectRatio),
	_fov(fov),
	_nearPlane(0.1f),
	_farPlane(100.0f),
	_model(dx::XMMatrixIdentity()),
	_view(dx::XMMatrixLookAtLH(_position, _target, _up)),
	_projection(dx::XMMatrixPerspectiveLH(_fov, _aspectRatio, _nearPlane, _farPlane)),
	_mvp(_mvp = dx::XMMatrixTranspose(_model * _view * _projection))
{}

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

void camera::updateAspectRatio(float newAspectRatio)
{
	_aspectRatio = newAspectRatio;
	_projection = dx::XMMatrixPerspectiveFovLH(_fov, _aspectRatio, _nearPlane, _farPlane);
	_mvp = dx::XMMatrixTranspose(_model * _view * _projection);
}
