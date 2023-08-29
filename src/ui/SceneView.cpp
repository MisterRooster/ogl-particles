/*------------------------------------------------------------------------------------------------*\
| verlet_basic
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#include "SceneView.h"

#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include "render/BufferObject.h"
#include "particles/ParticleRenderer.h"
#include "particles/ParticleSystem.h"
#include "particles/Effect.h"
#include "utility/FileSystem.h"
#include "utility/Debug.h"


namespace nhahn
{
    SceneView::SceneView(std::shared_ptr<Texture> t)
        : _srcD(t), _screenSize(400, 225)
    {
        _srcSize = glm::vec2(t->width(), t->height());
        std::string path = nhahn::FileSystem::getModuleDirectory() + "shaders/";

        // global gl stats
        glEnable(GL_DEPTH_TEST);

        // camera
        glm::mat4 viewMat = glm::lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        _camMV = viewMat * glm::mat4(1.0f);

        // create new render target
        _rt = std::make_unique<RenderTarget>();

        // gpu progs
        _quadProg = std::make_unique<Shader>(path.c_str(), "common.inc", "quad.vert", nullptr, "quad.frag", 1);
        _particleProg = std::make_unique<Shader>(path.c_str(), "common.inc", "particles.vert", nullptr, "particles.frag", 1);

        // create blank buffer object
        BufferObject* blackPbo = new BufferObject(PIXEL_UNPACK_BUFFER, _srcSize.x * _srcSize.y * sizeof(float)*4);
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
        for (int y = 0, idx = 0; y < _srcSize.y; y++) {
            for (int x = 0; x < _srcSize.x; x++, idx++) {
                data[idx] = 0.5f;
                if (x == _srcSize.x - 1 || y == _srcSize.y - 1)
                    data[idx] = 0.0f;
            }
        }
        _screen->copy(data);
        delete[] data;

        // particles
        _tunnelEffect = EffectFactory::create("tunnel");
        _tunnelEffect->initialize(IEffect::DEFAULT_PARTICLE_NUM_FLAG);
        _tunnelEffect->initializeRenderer("gl");
    
        _attractorEffect = EffectFactory::create("attractors");
        _attractorEffect->initialize(IEffect::DEFAULT_PARTICLE_NUM_FLAG);
        _attractorEffect->initializeRenderer("gl");

        _fountainEffect = EffectFactory::create("fountain");
        _fountainEffect->initialize(IEffect::DEFAULT_PARTICLE_NUM_FLAG);
        _fountainEffect->initializeRenderer("gl");

        _currentEffectID = 0;
        _currentEffect = _attractorEffect.get();

        DBG("SceneView", DebugLevel::DEBUG, "Texture memory usage: %dmb\n", (int)(Texture::memoryUsage() / (1024 * 1024)));
    }

    SceneView::~SceneView()
    {
        _screen.reset();
        _quadProg.reset();
        _particleProg.reset();

        _tunnelEffect->clean();
        _fountainEffect->clean();
        _attractorEffect->clean();

        //glDeleteTextures(1, &_particleTex);
        _rt.reset();
    }

    void SceneView::render(double dt)
    {
        _currentFPS = (int)(1.0 / dt);

        // scene Window
        ImGuiWindowFlags screenflags = ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoNavFocus;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2.0f, 2.0f));
        //ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos(), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(_screenSize.x, _screenSize.y), ImGuiCond_FirstUseEver);
        ImGui::Begin("Scene view", nullptr, screenflags);
        ImGui::PopStyleVar(3);

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        _screenSize = { viewportPanelSize.x, viewportPanelSize.y };

        // camera
        glm::mat4 projMat = glm::perspective(glm::radians(45.0f), (float)_screenSize.x / (float)_screenSize.y, 0.1f, 100.0f);

        // get relative mouse position
        ImGuiIO& io = ImGui::GetIO();
        ImVec2 scrPos = ImGui::GetCursorScreenPos();
        ImVec2 relMousePos = ImVec2((io.MousePos.x - scrPos.x)/_screenSize.x,1.0f-(io.MousePos.y - scrPos.y)/_screenSize.y);

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
        _currentEffect->update(dt);
        _currentEffect->cpuUpdate(dt);
        _currentEffect->gpuUpdate(dt);

        _rt->selectAttachmentList(1, _rt->attachTextureAny(*_screen));
        //glEnable(GL_PROGRAM_POINT_SIZE);
        _particleProg->bind();
        _particleProg->setUniformMat("modelViewMat", _camMV, false);
        _particleProg->setUniformMat("projectionMat", projMat, false);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        _currentEffect->render();
        glDisable(GL_BLEND);
        _particleProg->unbind();        
        _rt->popViewport();
        _rt->unbind();

        // add rendered texture to ImGUI scene window
        uint64_t textureID = _screen->glName();
        ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ _screenSize.x, _screenSize.y }, ImVec2{0,1}, ImVec2{1,0});
        if (ImGui::IsItemHovered())
            ImGui::SetMouseCursor(7);

        // add fps info to corner
        {
            ImVec2 newPos;
            char btnLabel[64];
            snprintf(btnLabel, sizeof btnLabel, "Particles: %i | FPS: %i", _currentEffect->numAllParticles(), _currentFPS);
            newPos.x = ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(btnLabel).x - 18.0;
            ImGui::SameLine();
            newPos.y = ImGui::GetCursorPosY() + 10.0;
            ImGui::SetCursorPos(newPos);

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.6f, 0.6f, 0.35f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.6f, 0.6f, 0.35f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.6f, 0.6f, 0.35f));
            ImGui::Button(btnLabel);
            ImGui::PopStyleColor(3);
        }

        ImGui::End();
    }
}