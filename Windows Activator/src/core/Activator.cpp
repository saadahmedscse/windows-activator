// Activator.cpp
#include "Activator.h"
#include "AppState.h"
#include <windows.h>
#include <thread>

void RunHidden(const std::string& cmd) {
    STARTUPINFOA si = { sizeof(STARTUPINFOA) };
    PROCESS_INFORMATION pi;
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE; // Hidden from user

    std::string writableCmd = cmd;
    if (CreateProcessA(NULL, &writableCmd[0], NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

void ActivationTask() {
    std::string key;
    {
        std::lock_guard<std::mutex> lock(g_state.stateMutex);
        g_state.isActivating = true;
        g_state.statusText = "Installing Product Key...";
        key = g_state.editions[g_state.currentSelection].key;
    }

    RunHidden("cscript //nologo C:\\Windows\\System32\\slmgr.vbs /ipk " + key);

    { std::lock_guard<std::mutex> lock(g_state.stateMutex); g_state.statusText = "Connecting to KMS Server..."; }
    RunHidden("cscript //nologo C:\\Windows\\System32\\slmgr.vbs /skms kms8.msguides.com");

    { std::lock_guard<std::mutex> lock(g_state.stateMutex); g_state.statusText = "Activating Windows..."; }
    RunHidden("cscript //nologo C:\\Windows\\System32\\slmgr.vbs /ato");

    {
        std::lock_guard<std::mutex> lock(g_state.stateMutex);
        g_state.isAlreadyActivated = true;
        g_state.showSuccessCheckmark = true; // Triggers the green UI animation
        g_state.statusText = "Windows Already Activated";
        g_state.isActivating = false;
    }
}

void StartActivationThread() {
    if (g_state.isActivating) return;
    std::thread t(ActivationTask);
    t.detach();
}