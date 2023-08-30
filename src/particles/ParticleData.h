/*------------------------------------------------------------------------------------------------*\
| ogl-particles
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#pragma once

#include <glm/vec4.hpp>
#include "utility/Types.h"


namespace nhahn
{
    class ParticleData
    {
    public:
        ParticleData() { }
        explicit ParticleData(size_t maxCount) { generate(maxCount); }
        ~ParticleData() { }

        ParticleData(const ParticleData&) = delete;
        ParticleData& operator=(const ParticleData&) = delete;

        void generate(size_t maxSize);
        void kill(size_t id);
        void wake(size_t id);
        void swapData(size_t a, size_t b);

        static void copyOnlyAlive(const ParticleData* source, ParticleData* destination);
        static size_t computeMemoryUsage(const ParticleData& p);

    public:
        std::unique_ptr<glm::vec4[]> m_pos;
        std::unique_ptr<glm::vec4[]> m_col;
        std::unique_ptr<glm::vec4[]> m_startCol;
        std::unique_ptr<glm::vec4[]> m_endCol;
        std::unique_ptr<glm::vec4[]> m_vel;
        std::unique_ptr<glm::vec4[]> m_acc;
        std::unique_ptr<glm::vec4[]> m_time;
        std::unique_ptr<bool[]>  m_alive;

        size_t m_count{ 0 };
        size_t m_countAlive{ 0 };
    };
}