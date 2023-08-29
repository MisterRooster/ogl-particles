/*------------------------------------------------------------------------------------------------*\
| verlet_basic
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#pragma once

#include <memory>
#include "glm/glm.hpp"
#include "render/Shader.h"
#include "render/Texture.h"
#include "render/RenderTarget.h"


namespace nhahn
{
    class Texture;
    class Shader;
    class RenderTarget;
    class IEffect;

    class SceneView
    {
    public:
        SceneView(std::shared_ptr<Texture> t);
        ~SceneView();

        void render(double dt);

    private:
        int _currentFPS = 0;
        glm::mat4 _camMV;

        std::unique_ptr<Shader> _quadProg;
        std::unique_ptr<RenderTarget> _rt;

        std::shared_ptr<Texture> _srcD;
        std::unique_ptr<Texture> _screen;
        glm::vec2 _srcSize;
        glm::vec2 _screenSize;

        std::unique_ptr<Shader> _particleProg;
        std::unique_ptr<Texture> _particleTex;
        std::shared_ptr<IEffect> _tunnelEffect;
        std::shared_ptr<IEffect> _attractorEffect;
        std::shared_ptr<IEffect> _fountainEffect;

        IEffect* _currentEffect = nullptr;
        int _currentEffectID = 0;
    };
}