#include "OSDetector.h"
#include "AppState.h"
#include <windows.h>
#include <string>
#include <thread>

void CheckActivationTask() {
    HANDLE hRead, hWrite;
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    if (!CreatePipe(&hRead, &hWrite, &sa, 0)) return;

    STARTUPINFOA si = { sizeof(STARTUPINFOA) };
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE; // Completely hidden execution
    si.hStdOutput = hWrite;
    si.hStdError = hWrite;

    PROCESS_INFORMATION pi;
    // Bulletproof WMI Query to check Windows License Status
    char cmd[] = "powershell -noprofile -command \"(Get-WmiObject SoftwareLicensingProduct -Filter 'ApplicationID=''55c92734-d682-4d71-983e-d6ec3f16059f'' AND PartialProductKey IS NOT NULL').LicenseStatus\"";

    if (CreateProcessA(NULL, cmd, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        CloseHandle(hWrite);

        char buffer[128];
        DWORD bytesRead;
        std::string output = "";
        while (ReadFile(hRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
            buffer[bytesRead] = '\0';
            output += buffer;
        }
        CloseHandle(hRead);
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        std::lock_guard<std::mutex> lock(g_state.stateMutex);
        // Status '1' means Licensed (Activated)
        if (output.find("1") != std::string::npos) {
            g_state.isAlreadyActivated = true;
            g_state.statusText = "Windows Already Activated";
        }
        else {
            g_state.isAlreadyActivated = false;
            g_state.statusText = "Ready to Activate";
        }
        g_state.isCheckingActivation = false;
    }
    else {
        CloseHandle(hRead);
        CloseHandle(hWrite);
        std::lock_guard<std::mutex> lock(g_state.stateMutex);
        g_state.isCheckingActivation = false;
        g_state.statusText = "Ready to Activate";
    }
}

void InitializeOSData() {
    std::string prefix = "Windows 10 ";
    std::string currentEditionID = "Professional"; // Default fallback

    // Query Registry for OS Build and Edition
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {

        // Check for Windows 11
        char build[64] = { 0 };
        DWORD size = sizeof(build);
        if (RegQueryValueExA(hKey, "CurrentBuild", NULL, NULL, (LPBYTE)build, &size) == ERROR_SUCCESS) {
            if (std::stoi(build) >= 22000) prefix = "Windows 11 ";
        }

        // Check Windows Edition (e.g., "Core", "Professional", "Enterprise")
        char edition[128] = { 0 };
        size = sizeof(edition);
        if (RegQueryValueExA(hKey, "EditionID", NULL, NULL, (LPBYTE)edition, &size) == ERROR_SUCCESS) {
            currentEditionID = edition;
        }
        RegCloseKey(hKey);
    }

    std::lock_guard<std::mutex> lock(g_state.stateMutex);

    // FULL LIST
    g_state.editions = {
        { prefix + "Home", "TX9XD-98N7V-6WMQ6-BX7FG-H8Q99" },                               // 0
        { prefix + "Home N", "3KHY7-WNT83-DGQKR-F7HPR-844BM" },                             // 1
        { prefix + "Home Single Language", "7HNRX-D7KGG-3K4RQ-4WPJ4-YTDFH" },               // 2
        { prefix + "Home Country Specific", "PVMJN-6DFY6-9CCP6-7BKTT-D3WVR" },              // 3
        { prefix + "Pro", "W269N-WFGWX-YVC9B-4J6C9-T83GX" },                                // 4
        { prefix + "Pro N", "MH37W-N47XK-V7XM9-C7227-GCQG9" },                              // 5
        { prefix + "Pro for Workstations", "NRG8B-VKK3Q-CXVCJ-9G2XF-6Q84J" },               // 6
        { prefix + "Pro for Workstations N", "9FNHH-K3HBT-3W4TD-6383H-6XYWF" },             // 7
        { prefix + "Pro Education", "6TP4R-GNPTD-KYYHQ-7B7DP-J447Y" },                      // 8
        { prefix + "Pro Education N", "YVWGF-BXNMC-HTQYQ-CPQ99-66QFC" },                    // 9
        { prefix + "Education", "NW6C2-QMPVW-D7KKK-3GKT6-VCFB2" },                          // 10
        { prefix + "Education N", "2WH4N-8QGBV-H22JP-CT43Q-MDWWJ" },                        // 11
        { prefix + "Enterprise", "NPPR9-FWDCX-D2C8J-H872K-2YT43" },                         // 12
        { prefix + "Enterprise N", "DPH2V-TTNVB-4X9Q3-TJR4H-KHJW4" },                       // 13
        { prefix + "Enterprise G", "YYVX9-NTFWV-6MDM3-9PT4T-4M68B" },                       // 14
        { prefix + "Enterprise G N", "44RPN-FTY23-9VTTB-MP9BX-T84FV" },                     // 15
        { prefix + "Enterprise LTSC 2019", "M7XTQ-FN8P6-TTKYV-9D4CC-J462D" },               // 16
        { prefix + "Enterprise N LTSC 2019", "92NFX-8DJQP-P6BBQ-THF9C-7CG2H" }              // 17
    };

    g_state.detectedOSIndex = 0;
    if (currentEditionID == "Core") g_state.detectedOSIndex = 0;
    else if (currentEditionID == "CoreN") g_state.detectedOSIndex = 1;
    else if (currentEditionID == "CoreSingleLanguage") g_state.detectedOSIndex = 2;
    else if (currentEditionID == "CoreCountrySpecific") g_state.detectedOSIndex = 3;
    else if (currentEditionID == "Professional") g_state.detectedOSIndex = 4;
    else if (currentEditionID == "ProfessionalN") g_state.detectedOSIndex = 5;
    else if (currentEditionID == "ProfessionalWorkstation") g_state.detectedOSIndex = 6;
    else if (currentEditionID == "ProfessionalWorkstationN") g_state.detectedOSIndex = 7;
    else if (currentEditionID == "ProfessionalEducation") g_state.detectedOSIndex = 8;
    else if (currentEditionID == "ProfessionalEducationN") g_state.detectedOSIndex = 9;
    else if (currentEditionID == "Education") g_state.detectedOSIndex = 10;
    else if (currentEditionID == "EducationN") g_state.detectedOSIndex = 11;
    else if (currentEditionID == "Enterprise") g_state.detectedOSIndex = 12;
    else if (currentEditionID == "EnterpriseN") g_state.detectedOSIndex = 13;
    else if (currentEditionID == "EnterpriseG") g_state.detectedOSIndex = 14;
    else if (currentEditionID == "EnterpriseGN") g_state.detectedOSIndex = 15;
    else if (currentEditionID == "EnterpriseS") g_state.detectedOSIndex = 16;
    else if (currentEditionID == "EnterpriseSN") g_state.detectedOSIndex = 17;
    g_state.currentSelection = g_state.detectedOSIndex;

    std::thread t(CheckActivationTask);
    t.detach();
}