/*------------------------------------------------------------------------------------------------*\
| ogl-particles
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#include "SceneView.h"

#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>

#include "IconFontDefines.h"
#include "render/BufferObject.h"
#include "particles/ParticleRenderer.h"
#include "particles/Effect.h"
#include "utility/FileSystem.h"
#include "utility/Debug.h"
#include "utility/Utils.h"

#ifndef M_PI
#   define M_PI 		3.1415926535897932384626433832795f
#   define M_2_PI 		6.28318530717958647692528676655901f		// PI*2
#endif


namespace nhahn
{
    Camera::Camera(glm::vec3 eye, glm::vec3 lookat, glm::vec3 upVector)
        : _eye(std::move(eye)), _lookAt(std::move(lookat)), _upVector(std::move(upVector))
    {
        updateViewMatrix();
    }

    void Camera::setCameraView(glm::vec3 eye, glm::vec3 lookat, glm::vec3 up)
    {
        _eye = std::move(eye);
        _lookAt = std::move(lookat);
        _upVector = std::move(up);
        updateViewMatrix();
    }

    void Camera::updateViewMatrix()
    {
        _viewMatrix = glm::lookAt(_eye, _lookAt, _upVector);
    }

    SceneView::SceneView(std::shared_ptr<Texture> t)
        : _srcD(t), _screenSize(400, 225)
    {
        _srcSize = glm::vec2(t->width(), t->height());
        std::string path = nhahn::FileSystem::getModuleDirectory() + "data\\shaders\\";

        // global gl stats
        glEnable(GL_DEPTH_TEST);

        // camera
        _cam = std::make_shared<Camera>(glm::vec3(0, 0.2, 0.4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

        // create new render target
        _rt = std::make_unique<RenderTarget>();

        // gpu progs
        _quadProg = std::make_unique<Shader>(path.c_str(), "common.inc", "quad.vert", nullptr, "quad.frag", 1);
        _particleProg = std::make_unique<Shader>(path.c_str(), "common.inc", "particles.vert", nullptr, "particles.frag", 1);

        // create blank buffer object
        BufferObject* blackPbo = new BufferObject(PIXEL_UNPACK_BUFFER, (GLsizei)(_srcSize.x * _srcSize.y * sizeof(float)*4));
        blackPbo->bind();
        blackPbo->map();
        memset(blackPbo->data(), 0, _srcSize.x * _srcSize.y * sizeof(float) * 4);
        blackPbo->unmap();
        blackPbo->unbind();

        _screen = std::make_unique<Texture>(TEXTURE_2D, _srcSize.x, _srcSize.y);
        _screen->setFormat(TEXEL_FLOAT, 4, 4);
        _screen->init();
        _screen->copyPbo(*blackPbo);

        float* data = new float[_srcSize.x * _srcSize.y * 4];
        for (unsigned int y = 0, idx = 0; y < _srcSize.y; y++) {
            for (unsigned int x = 0; x < _srcSize.x; x++, idx++) {
                data[idx] = 0.5f;
                if (x == _srcSize.x - 1 || y == _srcSize.y - 1)
                    data[idx] = 0.0f;
            }
        }
        _screen->copy(data);
        delete[] data;

        // particle texture
        std::string texturePath = nhahn::FileSystem::getModuleDirectory() + "data\\sprites\\scorch_02.png";
        
        int textureW, textureH, textureChannels;
        void* textureData = FileSystem::loadImageFile(texturePath.c_str(), &textureW, &textureH, &textureChannels, 4);

        // create buffer object
        _particleTex = std::make_unique<Texture>(TEXTURE_2D, textureW, textureH);
        _particleTex->setFormat(TEXEL_FLOAT, 4, 1);
        _particleTex->init();
        _particleTex->copy(textureData);
        delete[] textureData;

        _currentEffect = nullptr;

        DBG("SceneView", DebugLevel::DEBUG, "Texture memory usage: %dmb\n", (int)(Texture::memoryUsage() / (1024 * 1024)));
    }

    SceneView::~SceneView()
    {
        _screen.reset();
        _quadProg.reset();
        _particleProg.reset();
        _particleTex.reset();

        _rt.reset();
    }

    void SceneView::render(double dt)
    {
        _currentFPS = (int)std::round(1.0 / dt);

        // scene Window
        ImGuiWindowFlags screenflags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2.0f, 2.0f));

        ImGui::SetNextWindowSize(ImVec2((float)_screenSize.x, (float)_screenSize.y), ImGuiCond_FirstUseEver);
        ImGui::Begin(ICON_MDI_EYE " Scene View", nullptr, screenflags);
        ImGui::PopStyleVar(3);

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        _screenSize = { viewportPanelSize.x, viewportPanelSize.y };

        // get relative mouse position
        ImGuiIO& io = ImGui::GetIO();
        ImVec2 scrPos = ImGui::GetCursorScreenPos();
        ImVec2 relMousePos = ImVec2((io.MousePos.x - scrPos.x) / _screenSize.x, 1.0f - (io.MousePos.y - scrPos.y) / _screenSize.y);

        // camera
        updateCamera(dt);
        glm::mat4 projMat = glm::perspective(glm::radians(54.0f), (float)_screenSize.x / (float)_screenSize.y, 0.1f, 100.0f);

        // render source textures to screen texture
        _rt->bind();
        _rt->pushViewport(0, 0, _srcSize.x, _srcSize.y);
        _srcD->bindAny();
        RtAttachment dst = _rt->attachTextureAny(*_screen);
        _rt->selectAttachmentList(1, dst);
        _quadProg->bind();
        _quadProg->setUniformI("D", _srcD->boundUnit());
        _quadProg->setUniformF("mousePos", glm::vec2(relMousePos.x, relMousePos.y));
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        _quadProg->unbind();

        // render particles to screen texture
        if (_currentEffect)
        {
            _currentEffect->update(dt);
            _currentEffect->cpuUpdate(dt);
            _currentEffect->gpuUpdate(dt);

            _particleTex->bindAny();
            _rt->selectAttachmentList(1, _rt->attachTextureAny(*_screen));
            glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
            glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);
            glPointSize(40.0f);
            glEnable(GL_POINT_SPRITE);
            glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
            _particleProg->bind();
            _particleProg->setUniformI("tex", _particleTex->boundUnit());
            _particleProg->setUniformMat("modelViewMat", _cam->getViewMatrix(), false);
            _particleProg->setUniformMat("projectionMat", projMat, false);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            _currentEffect->render();
            glDisable(GL_BLEND);
            _particleProg->unbind();
        }  
        _rt->popViewport();
        _rt->unbind();

        // add rendered texture to ImGUI scene window
        uint64_t textureID = _screen->glName();
        ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ (float)_screenSize.x, (float)_screenSize.y }, ImVec2{0,1}, ImVec2{1,0});
        if (ImGui::IsItemHovered())
            ImGui::SetMouseCursor(7);

        // add stats info
        {
            char statsLabel[64];
            snprintf(statsLabel, sizeof statsLabel, ICON_MDI_POUND ICON_MDI_SHIMMER " : %i | " ICON_MDI_SPEEDOMETER " : %i fps",
                (_currentEffect) ? _currentEffect->numAllParticles() : 0, _currentFPS);
            ImVec2 labelSize = ImGui::CalcTextSize(statsLabel);

            ImGuiWindowFlags statsInfo_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking
                | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings
                | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

            const float statsInfo_margin = 10.0f;
            const ImVec2 statsInfo_pad = ImVec2(10.0f, 4.0f);
            
            ImVec2 statsInfo_size = ImVec2(labelSize.x + statsInfo_pad.x * 2, labelSize.y + statsInfo_pad.y * 2);

            ImGui::SameLine();
            ImVec2 statsInfo_pos;
            statsInfo_pos.x = ImGui::GetWindowPos().x + ImGui::GetWindowWidth() - statsInfo_margin;
            statsInfo_pos.y = ImGui::GetCursorScreenPos().y + statsInfo_margin;

            ImGui::SetNextWindowPos(statsInfo_pos, ImGuiCond_Always, ImVec2(1.0f, 0.0f));
            statsInfo_flags |= ImGuiWindowFlags_NoMove;

            ImGui::SetNextWindowBgAlpha(0.15f); // Transparent background
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, statsInfo_pad);

            if (ImGui::BeginChild("StatsInfo", statsInfo_size, true, statsInfo_flags))
            {
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1), statsLabel);
            }
            ImGui::PopStyleVar(2);
            ImGui::EndChild();
        }

        // add input info
        {
            const char* inputlabel1 = "[" ICON_MDI_MOUSE "LMB + " ICON_MDI_ARROW_ALL "] " ICON_MDI_ARROW_RIGHT_THIN " Turn Camera";
            const char* inputlabel2 = "[" ICON_MDI_MOUSE "RMB + " ICON_MDI_ARROW_UP_DOWN "] " ICON_MDI_ARROW_RIGHT_THIN " Zoom in / out";
            ImVec2 labelSize1 = ImGui::CalcTextSize(inputlabel1);
            ImVec2 labelSize2 = ImGui::CalcTextSize(inputlabel2);

            ImGuiWindowFlags inputInfo_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking
                | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings
                | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

            const float inputInfo_margin = 10.0f;
            const ImVec2 inputInfo_pad = ImVec2(10.0f, 4.0f);

            ImVec2 inputInfo_size = ImVec2(std::max(labelSize1.x, labelSize2.x) + inputInfo_pad.x * 2,
                labelSize1.y + labelSize2.y + inputInfo_pad.y * 2 + 9.0f);

            ImVec2 inputInfo_pos;
            inputInfo_pos.x = ImGui::GetWindowPos().x + ImGui::GetWindowWidth() - inputInfo_margin;
            inputInfo_pos.y = ImGui::GetWindowPos().y + ImGui::GetWindowHeight() - inputInfo_margin;

            ImGui::SetNextWindowPos(inputInfo_pos, ImGuiCond_Always, ImVec2(1.0f, 1.0f));
            inputInfo_flags |= ImGuiWindowFlags_NoMove;

            ImGui::SetNextWindowBgAlpha(0.15f); // Transparent background

            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, inputInfo_pad);

            if (ImGui::BeginChild("InputInfo", inputInfo_size, true, inputInfo_flags))
            {
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1), inputlabel1);
                ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.06f, 0.06f, 0.06f, 1.0f));
                ImGui::Separator();
                ImGui::PopStyleColor(1);
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1), inputlabel2);
            }
            ImGui::PopStyleVar(2);
            ImGui::EndChild(); 
        }

        ImGui::End();
    }

    void SceneView::updateCamera(double dt)
    {
        ImGuiIO& io = ImGui::GetIO();
        if (ImGui::IsWindowHovered())
        {
            // calculate the amount of rotation given the mouse movement.
            glm::vec4 position(_cam->getEye().x, _cam->getEye().y, _cam->getEye().z, 1);
            glm::vec4 pivot(_cam->getLookAt().x, _cam->getLookAt().y, _cam->getLookAt().z, 1);

            if (io.MouseDown[0])
            {
                float deltaAngleX = (2 * M_PI / _screenSize.x);     // a movement from left to right = 2*PI = 360 deg
                float deltaAngleY = (M_PI / _screenSize.y);         // a movement from top to bottom = PI = 180 deg

                float xAngle = (io.MouseDelta.x) * deltaAngleX;
                float yAngle = (io.MouseDelta.y) * deltaAngleY;

                // extra step to handle the problem when the camera direction is the same as the up vector
                float cosAngle = dot(_cam->getViewDir(), glm::vec3(0, 1, 0));
                if (cosAngle * Utils::sgn(deltaAngleY) > 0.99f)
                    deltaAngleY = 0;

                // rotate the camera around the pivot point on the first axis.
                glm::mat4x4 rotationMatrixX(1.0f);
                rotationMatrixX = glm::rotate(rotationMatrixX, xAngle, glm::vec3(0, 1, 0));
                position = (rotationMatrixX * (position - pivot)) + pivot;

                // rotate the camera around the pivot point on the second axis.
                glm::mat4x4 rotationMatrixY(1.0f);
                rotationMatrixY = glm::rotate(rotationMatrixY, yAngle, _cam->getRightVector());
                glm::vec3 finalPosition = (rotationMatrixY * (position - pivot)) + pivot;

                // Update the camera view (we keep the same lookat and the same up vector)
                _cam->setCameraView(finalPosition, _cam->getLookAt(), glm::vec3(0, 1, 0));
            }

            if (io.MouseDown[1])
            {
                position = position + (position - pivot) * io.MouseDelta.y * 0.002f;
                _cam->setCameraView(position, _cam->getLookAt(), _cam->getUpVector());
            }
        }        
    }
}