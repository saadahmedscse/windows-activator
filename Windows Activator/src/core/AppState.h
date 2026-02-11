#pragma once
#include <string>
#include <vector>
#include <mutex>
#include <d3d11.h>

struct WinEdition {
    std::string name;
    std::string key;
};

struct AppState {
    std::vector<WinEdition> editions;
    int currentSelection = 0;
    int detectedOSIndex = 0;

    bool isActivating = false;
    std::string statusText = "Checking License Status...";

    std::mutex stateMutex;

    bool isCheckingActivation = true;
    bool isAlreadyActivated = false;
    bool showSuccessCheckmark = false;

    ID3D11ShaderResourceView* leftArrowTex = nullptr;
    ID3D11ShaderResourceView* rightArrowTex = nullptr;
    ID3D11ShaderResourceView* closeIconTex = nullptr;
};

extern AppState g_state;