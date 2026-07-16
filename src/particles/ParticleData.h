/*------------------------------------------------------------------------------------------------*\
| ogl-particles
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#pragma once

#include <new>
#include "utility/Types.h"

#ifndef GLM_FORCE_INTRINSICS
#define GLM_FORCE_INTRINSICS
#endif // !GLM_FORCE_INTRINSICS
#include <glm/glm.hpp>

namespace nhahn
{
    class ParticleData
    {
    public:
        /** Alignment of the particle arrays. Must cover the widest SIMD access in ParticleUpdaters
            (currently __m256 = 32 bytes), or the aligned AVX loads there fault. */
        static constexpr std::align_val_t ALIGNMENT{ 32 };

        ParticleData() { }
        explicit ParticleData(size_t maxCount) { generate(maxCount); }
        ~ParticleData();

        ParticleData(const ParticleData&) = delete;
        ParticleData& operator=(const ParticleData&) = delete;

        void generate(size_t maxSize);
        void kill(size_t id);
        void wake(size_t id);
        void swapData(size_t a, size_t b);

        static void copyOnlyAlive(const ParticleData* source, ParticleData* destination);
        static size_t computeMemoryUsage(const ParticleData& p);

    public:
        glm::vec4* m_pos;
        glm::vec4* m_col;
        glm::vec4* m_startCol;
        glm::vec4* m_endCol;
        glm::vec4* m_vel;
        glm::vec4* m_acc;
        glm::vec4* m_time;
        std::unique_ptr<bool[]>  m_alive;

        size_t m_count{ 0 };
        size_t m_countAlive{ 0 };
    };
}