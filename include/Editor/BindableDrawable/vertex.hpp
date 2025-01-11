#pragma once

struct vertex {
	float x, y, z;   // Position
	float nx, ny, nz;  // Normal

	vertex(float px, float py, float pz,
		float pnx, float pny, float pnz)
		: x(px), y(py), z(pz), nx(pnx), ny(pny), nz(pnz) {
	}

	static D3D11_INPUT_ELEMENT_DESC* getDesc(UINT* size) {
		static D3D11_INPUT_ELEMENT_DESC inElDesc[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORM", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
		*size = std::size(inElDesc);
		return inElDesc;
	}
};
