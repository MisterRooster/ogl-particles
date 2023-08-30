/*------------------------------------------------------------------------------------------------*\
| ogl-particles
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#include "PropertyPanel.h"

#include <memory>
#include "imgui.h"
#include "utility/Debug.h"


namespace nhahn
{
    PropertyPanel::PropertyPanel()
    {
        _effectMap = MapOfEffects{};
        _currEffKey = "";
    }

    void PropertyPanel::addEffect(std::string name, std::shared_ptr<IEffect> eff)
    {
        _effectMap[name] = eff;
        if (_currEffKey.empty())
            _currEffKey = name;
    }

    void PropertyPanel::setEffectSwitchedCallback(std::function<void(std::shared_ptr<IEffect>)> func)
    {
        _effectSwitchedCB = func;
        DBG("PropertyPanel", DebugLevel::DEBUG, "changed onEffectSwitch callback\n");
    }

    void PropertyPanel::render()
    {
        ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos(), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Properties"))
        {
            ImGuiStyle& style = ImGui::GetStyle();
            float w = ImGui::CalcItemWidth();
            float spacing = style.ItemInnerSpacing.x;

            ImGui::SeparatorText("Scene:");

            ImGui::AlignTextToFramePadding();
            ImGui::TextColored(ImVec4(1.0f, 0.628f, 0.311f, 1.0f), "Current Effect:");

            ImGui::SameLine();
            ImGui::PushItemWidth(w - spacing * 2.0f);
            if (ImGui::BeginCombo("##cb_effect", _currEffKey.c_str()))
            {
                for (auto const& [key, val] : _effectMap)
                {
                    bool is_selected = (_currEffKey == key);
                    if (ImGui::Selectable(key.c_str(), is_selected))
                    {
                        _currEffKey = key;
                        if (_effectSwitchedCB)
                            _effectSwitchedCB(_effectMap[_currEffKey]);
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::PopItemWidth();

            _effectMap[_currEffKey]->renderUI();
        }
        ImGui::End();
    }
}