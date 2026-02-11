#pragma once
#include <d3d11.h>

bool LoadTextureFromMemory(const unsigned char* image_data_in, int image_size, ID3D11Device* d3dDevice, ID3D11DeviceContext* d3dContext, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);