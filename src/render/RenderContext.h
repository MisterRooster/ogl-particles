/*------------------------------------------------------------------------------------------------*\
| ogl-particles
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#pragma once

#include "glm/glm.hpp"


namespace nhahn
{
    class Window;

    class RenderContext
    {
    public:
        RenderContext() : _window(nullptr) {}

        virtual bool init(Window* window)
        {
            _window = window;
            return true;
        }

        virtual void preRender() = 0;
        virtual void postRender() = 0;
        virtual void end() = 0;

    protected:
        Window* _window;
    };

    class GLContext : public RenderContext
    {
    public:
        bool init(Window* window) override;

        void setClearColor(glm::vec4 color) { _clearColor = color; }

        void preRender() override;
        void postRender() override;
        void end() override;

    private:
        glm::vec4 _clearColor = glm::vec4(0.19, 0.24f, 0.26f, 1.0f);
    };

    class UIContext : public RenderContext
    {
    public:
        bool init(Window* window) override;

        void preRender() override;
        void postRender() override;
        void end() override;

        bool disableTitlebar();

    private:
        void attemptDragWindow();

        void setStyleDarkOrange();

    private:
        // for custom titlebar
        double s_xpos = 0, s_ypos = 0;
        int w_xsiz = 0, w_ysiz = 0;
        int dragState = 0;
    };
}