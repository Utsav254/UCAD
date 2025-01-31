#pragma once
#include "pch.hpp"

class camera
{
public:
	enum projType
	{
		PERSPECTIVE,
		ORTHOGRAPHIC,
	};

	enum toolType
	{
		NONE,
		ORBIT,
		PAN,
		LOOKAROUND,
		ZOOM,
	};

public:
	camera(float aspectRatio, float fov = dx::XM_PIDIV4);
	~camera() = default;

	void drawUI() noexcept;

	void mouseClickDrag(float dx, float dy) noexcept;

	void orbit(float dTheta, float dPhi) noexcept;
	void pan(float dx, float dy) noexcept;
	void lookAround(float dYaw, float dPitch) noexcept;
	void forwardBackward(float d) noexcept;

	void updateAspectRatio(float newAspectRatio);
	inline const dx::XMMATRIX* getTransformationMat() const noexcept { return &_mvp; }

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

	static inline constexpr const char* _toolNames[] = {"None", "Orbit", "Pan", "LookAround", "Zoom" };
	toolType _tool;
	projType _proj;
};