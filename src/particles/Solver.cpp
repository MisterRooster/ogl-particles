/*------------------------------------------------------------------------------------------------*\
| ogl-particles
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#include "Solver.h"
#include <algorithm>

namespace nhahn
{
    void Solver::update(double dt)
    {
        _time += dt;
        const float step_dt = getStepDt();
        for (uint32_t i{ _sub_steps }; i--;) {
            applyGravity();
            checkCollisions(step_dt);
            applyConstraint();
            updateObjects(step_dt);
        }
    }

    void Solver::applyGravity()
    {
        std::for_each(_objects.begin(), _objects.end(), [this](auto& obj) {
            obj.accelerate(_gravity);
        });
    }

    void Solver::checkCollisions(float dt)
    {
        const float    response_coef = 0.75f;
        const uint64_t objects_count = _objects.size();

        // Iterate on all objects
        for (uint64_t i{ 0 }; i < objects_count; ++i) {
            VerletObject& object_1 = _objects[i];

            // Iterate on object involved in new collision pairs
            for (uint64_t k{ i + 1 }; k < objects_count; ++k) {
                VerletObject& object_2 = _objects[k];
                const glm::vec2 v = object_1.pos - object_2.pos;
                const float        dist2 = v.x * v.x + v.y * v.y;
                const float        min_dist = object_1.radius + object_2.radius;

                // Check overlapping
                if (dist2 < min_dist * min_dist) {
                    const float        dist = sqrt(dist2);
                    const glm::vec2 n = v / dist;
                    const float mass_ratio_1 = object_1.radius / (object_1.radius + object_2.radius);
                    const float mass_ratio_2 = object_2.radius / (object_1.radius + object_2.radius);
                    const float delta = 0.5f * response_coef * (dist - min_dist);

                    // Update positions
                    object_1.pos -= n * (mass_ratio_2 * delta);
                    object_2.pos += n * (mass_ratio_1 * delta);
                }
            }
        }
    }

    void Solver::applyConstraint()
    {
        for (auto& obj : _objects) {
            const glm::vec2 v = _constraint_center - obj.pos;
            const float        dist = sqrt(v.x * v.x + v.y * v.y);
            if (dist > (_constraint_radius - obj.radius)) {
                const glm::vec2 n = v / dist;
                obj.pos = _constraint_center - n * (_constraint_radius - obj.radius);
            }
        }
    }

    void Solver::updateObjects(float dt)
    {
        std::for_each(_objects.begin(), _objects.end(), [dt](auto& obj) {
            obj.update(dt);
        });
    }
}