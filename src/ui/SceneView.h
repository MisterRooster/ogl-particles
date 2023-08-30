/*------------------------------------------------------------------------------------------------*\
| ogl-particles
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

    class Camera
    {
    public:
        Camera() = default;
        Camera(glm::vec3 eye, glm::vec3 lookat, glm::vec3 upVector);

        glm::mat4x4 getViewMatrix() const { return _viewMatrix; }
        glm::vec3 getEye() const { return _eye; }
        glm::vec3 getUpVector() const { return _upVector; }
        glm::vec3 getLookAt() const { return _lookAt; }

        glm::vec3 getViewDir() const { return -glm::transpose(_viewMatrix)[2]; }
        glm::vec3 getRightVector() const { return glm::transpose(_viewMatrix)[0]; }

        void setCameraView(glm::vec3 eye, glm::vec3 lookat, glm::vec3 up);

        void updateViewMatrix();

    private:
        glm::mat4x4 _viewMatrix;
        glm::vec3 _eye;
        glm::vec3 _lookAt;
        glm::vec3 _upVector;
    };

    class SceneView
    {
    public:
        SceneView(std::shared_ptr<Texture> t);
        ~SceneView();

        void render(double dt);

        void setEffect(IEffect* effect) { _currentEffect = effect; }

    private:
        void updateCamera(double dt);

    private:
        int _currentFPS = 0;
        std::shared_ptr<Camera> _cam;

        std::unique_ptr<Shader> _quadProg;
        std::unique_ptr<RenderTarget> _rt;

        std::shared_ptr<Texture> _srcD;
        std::unique_ptr<Texture> _screen;
        glm::vec2 _srcSize;
        glm::vec2 _screenSize;

        std::unique_ptr<Shader> _particleProg;
        std::unique_ptr<Texture> _particleTex;

        IEffect* _currentEffect = nullptr;
    };
}