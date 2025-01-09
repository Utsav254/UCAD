#pragma once
#include <DirectXMath.h>
namespace dx = DirectX;

class camera
{
public:
	camera(float aspectRatio, float fov = dx::XM_PIDIV4);
	~camera() = default;

	void orbit(float dTheta, float dPhi) noexcept;
	void pan(float dx, float dy) noexcept;
	void lookAround(float dYaw, float dPitch) noexcept;
	void forwardBackward(float d) noexcept;

	void updateAspectRatio(float newAspectRatio);

	inline const dx::XMMATRIX* getTransformationMat() const noexcept { return &_mvp; }
	inline const dx::XMMATRIX* getViewMatrix() const noexcept { return &_view; }
private:

	// cartesian system
	dx::XMVECTOR _position;
	dx::XMVECTOR _target;
	dx::XMVECTOR _up;

	float _aspectRatio;
	float _fov;
	float _nearPlane;
	float _farPlane;

	// cached MVP matrices
	const dx::XMMATRIX _model;
	dx::XMMATRIX _view;
	dx::XMMATRIX _projection;

	dx::XMMATRIX _mvp;

};