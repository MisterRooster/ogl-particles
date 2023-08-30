/*------------------------------------------------------------------------------------------------*\
| ogl-particles
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#pragma once

#include "GL/glew.h"
#include <stack>


namespace nhahn
{
    class Texture;

    enum RtAttachment
    {
        RT_ATTACHMENT0,
        RT_ATTACHMENT1,
        RT_ATTACHMENT2,
        RT_ATTACHMENT3,
        RT_ATTACHMENT4,
        RT_ATTACHMENT5,
        RT_ATTACHMENT6,
        RT_ATTACHMENT7
    };

    struct Viewport 
    {
        Viewport(int _x, int _y, int _w, int _h) : x(_x), y(_y), w(_w), h(_h) {}

        int x, y, w, h;
    };

    class RenderTarget
    {
    public:
        RenderTarget();
        ~RenderTarget();

        GLuint glName() const { return _glName; }

        RtAttachment attachTextureAny(const Texture& tex);
        void attachTexture(const Texture& tex, int index, int level = 0);
        void detachTexture(int index);

        void attachDepthBuffer(const Texture& tex);
        void attachDepthStencilBuffer(const Texture& tex);
        void detachDepthBuffer();
        void detachDepthStencilBuffer();

        void selectAttachments(int num);
        void selectAttachmentList(int count, ...);
        void selectAttachments(int num, const RtAttachment bufs[]);
        void setReadBuffer(RtAttachment buf);

        void bind();
        static void unbind();

        static void setViewport(int x, int y, int w, int h);
        static void getViewport(int& x, int& y, int& w, int& h);
        static void pushViewport(int x, int y, int w, int h);
        static void popViewport();
        static void resetViewport();

        static int viewportX() { return _viewportX; }
        static int viewportY() { return _viewportY; }
        static int viewportW() { return _viewportW; }
        static int viewportH() { return _viewportH; }

    private:
        bool attachmentSwapRequired(int num, const RtAttachment bufs[]);

    private:
        GLuint _glName;

        static const int MaxAttachments = 8;
        const Texture* _attachments[MaxAttachments];
        static int _attachmentCount;
        static RtAttachment _selectedAttachments[MaxAttachments];

        int _nextTicket;
        int _attachmentTicket[MaxAttachments];

        static std::stack<Viewport> _viewports;
        static int _viewportX, _viewportY;
        static int _viewportW, _viewportH;
    };
}