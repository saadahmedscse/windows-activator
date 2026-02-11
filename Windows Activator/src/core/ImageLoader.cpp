#include "ImageLoader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../../Vendor/stb/stb_image.h"

bool LoadTextureFromMemory(const unsigned char* image_data_in, int image_size, ID3D11Device* d3dDevice, ID3D11DeviceContext* d3dContext, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height) {
    int image_width = 0;
    int image_height = 0;

    unsigned char* image_data = stbi_load_from_memory(image_data_in, image_size, &image_width, &image_height, NULL, 4);
    if (image_data == NULL) return false;

    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Width = image_width;
    desc.Height = image_height;
    desc.MipLevels = 0; // MUST BE 0 to tell DirectX to generate smooth Mipmaps
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET; // Render Target required for Mipmaps
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS; // Turn on Mipmap generation

    ID3D11Texture2D* pTexture = NULL;
    // Pass NULL data initially because we have to upload it differently for Mipmaps
    d3dDevice->CreateTexture2D(&desc, NULL, &pTexture);

    // Upload the base 512x512 image
    d3dContext->UpdateSubresource(pTexture, 0, NULL, image_data, image_width * 4, 0);

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = -1; // -1 allows reading all the smoothed mipmap levels
    srvDesc.Texture2D.MostDetailedMip = 0;
    d3dDevice->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
    pTexture->Release();

    // Tell the GPU to instantly generate the buttery-smooth scaled down versions
    d3dContext->GenerateMips(*out_srv);

    *out_width = image_width;
    *out_height = image_height;
    stbi_image_free(image_data);

    return true;
}