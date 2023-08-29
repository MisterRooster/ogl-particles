/*------------------------------------------------------------------------------------------------*\
| verlet_basic
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#pragma once

#include <map>
#include <string>
#include <functional>
#include "glm/glm.hpp"
#include "particles/Effect.h"


namespace nhahn
{
    class IEffect;

    class PropertyPanel
    {
        using MapOfEffects = std::map<std::string, std::shared_ptr<IEffect>>;

    public:
        PropertyPanel();

        void render();

        void addEffect(std::string name, std::shared_ptr<IEffect> eff);
        void setEffectSwitchedCallback(std::function<void(std::shared_ptr<IEffect>)> func);

    protected:
        MapOfEffects _effectMap;
        std::string _currEffKey;

    private:
        std::function<void(std::shared_ptr<IEffect>)> _effectSwitchedCB = {};
    };
}