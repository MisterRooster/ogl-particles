/*------------------------------------------------------------------------------------------------*\
| verlet_basic
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#pragma once

#include "glm/glm.hpp"


namespace nhahn
{
    class PropertyPanel
    {
    public:
        PropertyPanel() {}

        void render();

    private:
        glm::vec3 _color = { 1.0f, 0.0f, 0.0f };
        float _roughness = 0.2f;
        float _metallic = 0.1f;
    };
}