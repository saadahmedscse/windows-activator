#include "MainWindow.h"
#include "../../Vendor/ImGui/imgui.h"
#include "../../Vendor/ImGui/imgui_internal.h"
#include "../Core/AppState.h"
#include "../Core/Activator.h"
#include <math.h>
#include <windows.h>
#include <shellapi.h> // REQUIRED FOR OPENING WEB LINKS

// Helper function to draw a smooth circular spinner (Keep your existing one)
void DrawSpinner(const char* label, float radius, int thickness, const ImU32& color) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size((radius) * 2, (radius + style.FramePadding.y) * 2);
    const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
    ImGui::ItemSize(bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id)) return;

    window->DrawList->PathClear();
    int num_segments = 30;
    float start = (float)g.Time * 6.0f; // Speed
    const ImVec2 center = ImVec2(pos.x + radius, pos.y + radius + style.FramePadding.y);

    for (int i = 0; i < num_segments; i++) {
        const float a = start + ((float)i / (float)num_segments) * 2.0f * 3.14159f;
        window->DrawList->PathLineTo(ImVec2(center.x + cosf(a) * radius, center.y + sinf(a) * radius));
    }
    window->DrawList->PathStroke(color, ImDrawFlags_None, thickness);
}

void RenderUIMain() {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
    ImGui::Begin("Windows Activator", nullptr, flags);

    std::lock_guard<std::mutex> lock(g_state.stateMutex);

    ImFont* titleFont = ImGui::GetIO().Fonts->Fonts[1];
    ImFont* mediumFont = ImGui::GetIO().Fonts->Fonts[2];

    // --- ANIMATION STATE VARIABLES ---
    static float slideAnim = 1.0f;
    static int oldSelection = 0;
    static int slideDir = 1;

    // --- FADE VARIABLES ---
    bool isCorrectOS = (g_state.currentSelection == g_state.detectedOSIndex);

    // The button is ONLY clickable if it's the right OS, NOT already activated, and NOT currently checking.
    bool canActivate = isCorrectOS && !g_state.isAlreadyActivated && !g_state.isCheckingActivation;

    static float buttonAlphaAnim = 1.0f;
    float targetButtonAlpha = canActivate ? 1.0f : 0.3f;

    if (buttonAlphaAnim < targetButtonAlpha) {
        buttonAlphaAnim += ImGui::GetIO().DeltaTime * 5.0f;
        if (buttonAlphaAnim > targetButtonAlpha) buttonAlphaAnim = targetButtonAlpha;
    }
    else if (buttonAlphaAnim > targetButtonAlpha) {
        buttonAlphaAnim -= ImGui::GetIO().DeltaTime * 5.0f;
        if (buttonAlphaAnim < targetButtonAlpha) buttonAlphaAnim = targetButtonAlpha;
    }

    static float statusAlphaAnim = 1.0f;
    float targetStatusAlpha = (isCorrectOS || g_state.isActivating || g_state.isCheckingActivation || g_state.isAlreadyActivated) ? 1.0f : 0.0f;

    if (statusAlphaAnim < targetStatusAlpha) {
        statusAlphaAnim += ImGui::GetIO().DeltaTime * 5.0f;
        if (statusAlphaAnim > targetStatusAlpha) statusAlphaAnim = targetStatusAlpha;
    }
    else if (statusAlphaAnim > targetStatusAlpha) {
        statusAlphaAnim -= ImGui::GetIO().DeltaTime * 5.0f;
        if (statusAlphaAnim < targetStatusAlpha) statusAlphaAnim = targetStatusAlpha;
    }

    ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 40.0f, 10.0f));

    if (g_state.closeIconTex != nullptr) {
        ImVec2 closeIconSize = ImVec2(16.0f, 16.0f); // EXACTLY HALF SIZE!
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 8.0f)); // Tighter padding

        // Custom colors just for the Close Button
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));         // Transparent normal
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.2f)); // Red hover background
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.8f));    // Darker red on click

        if (ImGui::ImageButton("##close", (void*)g_state.closeIconTex, closeIconSize, ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1))) {
            PostQuitMessage(0); // This will now actually fire!
        }

        // Show the hand cursor
        if (ImGui::IsItemHovered()) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        }

        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar();
    }

    // --- Header ---
    ImGui::SetCursorPosY(30.0f);
    ImGui::PushFont(titleFont);
    const char* titleText = "WINDOWS ACTIVATOR";
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(titleText).x) * 0.5f);
    ImGui::TextColored(ImVec4(0.0f, 0.83f, 1.0f, 1.0f), titleText);
    ImGui::PopFont();

    // --- Custom Image Button Styles ---
    ImVec4 bgCol = ImVec4(0.0f, 0.0f, 0.0f, 0.0f); // Transparent background
    ImVec4 tintCol = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // Original PNG colors
    ImVec2 iconSize = ImVec2(30.0f, 30.0f); // Adjust if your icons are too large/small

    // --- Custom Slider Component ---
    ImGui::SetCursorPosY(120.0f);

    // Left Arrow Image Button
    ImGui::SetCursorPosX(25.0f);
    if (g_state.leftArrowTex != nullptr) {

        ImVec2 iconSize = ImVec2(24.0f, 24.0f); // The actual icon size inside the button
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 8.0f)); // 8px padding restores the background

        if (ImGui::ImageButton("##left", (void*)g_state.leftArrowTex, iconSize, ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1)) && !g_state.isActivating) {
            oldSelection = g_state.currentSelection;
            g_state.currentSelection--;
            if (g_state.currentSelection < 0) g_state.currentSelection = g_state.editions.size() - 1;

            slideAnim = 0.0f;
            slideDir = -1;
            if (g_state.statusText == "Activation Complete!") g_state.statusText = "Ready";
        }

        ImGui::PopStyleVar(); // Important: Pop it so it doesn't affect other buttons!

        if (ImGui::IsItemHovered() && !g_state.isActivating) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }

    // --- SLIDE & FADE TEXT ANIMATION ---
    ImGui::PushFont(mediumFont);
    if (slideAnim < 1.0f) {
        slideAnim += ImGui::GetIO().DeltaTime * 6.0f;
        if (slideAnim > 1.0f) slideAnim = 1.0f;
    }

    if (slideAnim < 1.0f) {
        std::string oldOS = g_state.editions[oldSelection].name;
        float oldTextWidth = ImGui::CalcTextSize(oldOS.c_str()).x;
        float oldStartX = (ImGui::GetWindowWidth() - oldTextWidth) * 0.5f;
        float oldOffset = slideDir == 1 ? (-30.0f * slideAnim) : (30.0f * slideAnim);

        ImGui::SetCursorPos(ImVec2(oldStartX + oldOffset, 128.0f));
        ImGui::TextColored(ImVec4(0.95f, 0.95f, 0.95f, 1.0f - slideAnim), "%s", oldOS.c_str());

        std::string currentOS = g_state.editions[g_state.currentSelection].name;
        float currentTextWidth = ImGui::CalcTextSize(currentOS.c_str()).x;
        float currentStartX = (ImGui::GetWindowWidth() - currentTextWidth) * 0.5f;
        float currentOffset = slideDir == 1 ? (30.0f * (1.0f - slideAnim)) : (-30.0f * (1.0f - slideAnim));

        ImGui::SetCursorPos(ImVec2(currentStartX + currentOffset, 128.0f));
        ImGui::TextColored(ImVec4(0.95f, 0.95f, 0.95f, slideAnim), "%s", currentOS.c_str());
    }
    else {
        std::string currentOS = g_state.editions[g_state.currentSelection].name;
        float currentTextWidth = ImGui::CalcTextSize(currentOS.c_str()).x;
        ImGui::SetCursorPos(ImVec2((ImGui::GetWindowWidth() - currentTextWidth) * 0.5f, 128.0f));
        ImGui::TextColored(ImVec4(0.95f, 0.95f, 0.95f, 1.0f), "%s", currentOS.c_str());
    }
    ImGui::PopFont();

    // Right Arrow Image Button
    ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 65.0f, 120.0f));
    if (g_state.rightArrowTex != nullptr) {

        ImVec2 iconSize = ImVec2(24.0f, 24.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 8.0f));

        if (ImGui::ImageButton("##right", (void*)g_state.rightArrowTex, iconSize, ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1)) && !g_state.isActivating) {
            oldSelection = g_state.currentSelection;
            g_state.currentSelection++;
            if (g_state.currentSelection >= g_state.editions.size()) g_state.currentSelection = 0;

            slideAnim = 0.0f;
            slideDir = 1;
            if (g_state.statusText == "Activation Complete!") g_state.statusText = "Ready";
        }

        ImGui::PopStyleVar();

        if (ImGui::IsItemHovered() && !g_state.isActivating) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }

    // --- Activate Button & Dynamic Loader/Checkmark ---
    ImGui::SetCursorPosY(220.0f);
    float btnWidth = 220.0f;
    float btnHeight = 50.0f;
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - btnWidth) * 0.5f);

    if (g_state.isActivating) {
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 24.0f) * 0.5f);
        DrawSpinner("Loader", 16.0f, 4, IM_COL32(0, 212, 255, 255));
    }
    // ONLY show the Checkmark if the user JUST successfully clicked activate.
    else if (g_state.showSuccessCheckmark) {
        ImVec2 cursorPos = ImGui::GetCursorScreenPos();
        ImVec2 center = ImVec2(cursorPos.x + (btnWidth * 0.5f), cursorPos.y + (btnHeight * 0.5f));
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        draw_list->AddCircleFilled(center, 24.0f, IM_COL32(40, 200, 80, 255));
        draw_list->AddLine(ImVec2(center.x - 10, center.y), ImVec2(center.x - 3, center.y + 8), IM_COL32(255, 255, 255, 255), 3.5f);
        draw_list->AddLine(ImVec2(center.x - 3, center.y + 8), ImVec2(center.x + 12, center.y - 10), IM_COL32(255, 255, 255, 255), 3.5f);

        ImGui::Dummy(ImVec2(btnWidth, btnHeight));
    }
    else {
        ImGui::PushFont(mediumFont);

        ImVec4 normalColor = ImVec4(0.12f, 0.65f, 0.25f, buttonAlphaAnim);
        ImVec4 hoverColor = canActivate ? ImVec4(0.15f, 0.75f, 0.30f, buttonAlphaAnim) : normalColor;
        ImVec4 activeColor = canActivate ? ImVec4(0.08f, 0.55f, 0.20f, buttonAlphaAnim) : normalColor;

        ImGui::PushStyleColor(ImGuiCol_Button, normalColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoverColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, activeColor);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.95f, 0.95f, buttonAlphaAnim));

        if (ImGui::Button("ACTIVATE", ImVec2(btnWidth, btnHeight))) {
            if (canActivate) StartActivationThread();
        }

        if (ImGui::IsItemHovered() && canActivate) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        ImGui::PopStyleColor(4);
        ImGui::PopFont();
    }

    // --- Status Text (Fades out when disabled) ---
    if (statusAlphaAnim > 0.01f) {
        ImGui::SetCursorPosY(300.0f);
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(g_state.statusText.c_str()).x) * 0.5f);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, statusAlphaAnim));
        ImGui::Text("%s", g_state.statusText.c_str());
        ImGui::PopStyleColor();
    }

    // --- Bottom Left Author Link ---
    ImGui::SetCursorPos(ImVec2(15.0f, ImGui::GetWindowHeight() - 30.0f));
    ImGui::TextDisabled("Author:");
    ImGui::SameLine(0, 5.0f);
    ImGui::TextColored(ImVec4(0.0f, 0.83f, 1.0f, 1.0f), "Saad Ahmed");

    if (ImGui::IsItemHovered()) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        ImVec2 min = ImGui::GetItemRectMin();
        ImVec2 max = ImGui::GetItemRectMax();
        min.y = max.y;
        ImGui::GetWindowDrawList()->AddLine(min, max, IM_COL32(0, 212, 255, 255), 1.0f);

        if (ImGui::IsItemClicked()) {
            ShellExecuteA(0, "open", "https://saadahmedev.com", 0, 0, SW_SHOWNORMAL);
            ShellExecuteA(0, "open", "https://github.com/saadahmedscse", 0, 0, SW_SHOWNORMAL);
        }
    }

    ImGui::End();
}