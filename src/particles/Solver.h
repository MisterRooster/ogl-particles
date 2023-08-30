/*------------------------------------------------------------------------------------------------*\
| ogl-particles
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#pragma once

#include <vector>
#include "glm/glm.hpp"


namespace nhahn
{
    struct VerletObject
    {
    public:
    	VerletObject(glm::vec2 pos, float radius)
            : pos(pos), pos_last(pos), accel(glm::vec2(0.0f)), radius(radius) {}
    
        void update(float dt)
        {
            // Compute how much we moved
            const glm::vec2 displacement = pos - pos_last;
    
            // Update position
            pos_last = pos;
            pos = pos + displacement + accel * (dt * dt);
            // Reset acceleration
            accel = {};
        }
    
        void accelerate(glm::vec2 a) { accel += a; }
    
        void setVelocity(glm::vec2 v, float dt) { pos_last = pos - (v * dt); }
        void addVelocity(glm::vec2 v, float dt) { pos_last -= v * dt; }
    
        [[nodiscard]]
        glm::vec2 getVelocity(float dt) const { return (pos - pos_last) / dt; }
    
    	glm::vec2 pos;
    	glm::vec2 pos_last;
    	glm::vec2 accel;
    	float radius = 10.0f;
    	glm::u8vec4 color = glm::u8vec4(255);
    };

    class Solver
    {
    public:
        Solver() = default;

        VerletObject& addObject(glm::vec2 position, float radius) { return _objects.emplace_back(position, radius); }

        void update(double dt);

        void setSimulationUpdateRate(double rate) { _frame_dt = rate; }

        void setConstraint(glm::vec2 position, float radius)
        {
            _constraint_center = position;
            _constraint_radius = radius;
        }

        void setSubStepsCount(uint32_t sub_steps) { _sub_steps = sub_steps; }

        void setObjectVelocity(VerletObject& object, glm::vec2 v) { object.setVelocity(v, getStepDt()); }

        [[nodiscard]]
        const std::vector<VerletObject>& getObjects() const { return _objects; }

        [[nodiscard]]
        glm::vec3 getConstraint() const { return { _constraint_center.x, _constraint_center.y, _constraint_radius }; }

        [[nodiscard]]
        uint64_t getObjectsCount() const { return _objects.size(); }

        [[nodiscard]]
        double getTime() const { return _time; }

        [[nodiscard]]
        double getStepDt() const { return _frame_dt / static_cast<double>(_sub_steps); }

    private:
        void applyGravity();

        void checkCollisions(float dt);

        void applyConstraint();

        void updateObjects(float dt);

    private:
        std::vector<VerletObject> _objects;

        uint32_t    _sub_steps = 1;
        glm::vec2   _gravity = { 0.0f, 1000.0f };
        glm::vec2   _constraint_center;
        float       _constraint_radius = 100.0f;
        double      _time = 0.0f;
        double      _frame_dt = 0.0f;
    };
}