#pragma once

#include <model/object.hpp>
#include <model/material.hpp>
#include <utils/glm.hpp>

#include <iostream>
#include <optional>
#include <exception>
#include <vector>
#include <cmath>
#include <algorithm>

namespace fcm {

  // Computes the collision forces and torques
  // here, force_1 = the force applied to obj_1, etc.,
  template <typename S, typename T>
  void collide(
      const S &obj_1, const T &obj_2,
      glm::dvec3 &force_1, glm::dvec3 &force_2,
      glm::dvec3 &torque_1, glm::dvec3 &torque_2
  ) {
    (void) obj_1;
    (void) obj_2;
    (void) force_1;
    (void) force_2;
    (void) torque_1;
    (void) torque_2;
    std::cerr << "Override this!\n";
    // using an exception since this really should never be called
    throw std::logic_error{"Collision not implemented!"};
  }

  glm::dvec3 friction_dir(const glm::dvec3 &v, const glm::dvec3 &n) {
    // we want to solve for the frictional force f
    // here, v is velocity vector and n is normal of plane
    // v - proj(v, n) is the projection of v onto the plane with normal n
    // we make it negative because friction force opposes direction of movement
    // then we normalize it to get a direction
    // TODO this divides by zero if velocity is perpendicular to normal
    return glm::normalize(glm::proj(v, n) - v);
  }

  template <>
  void collide(
      const Sphere &obj_1, const Sphere &obj_2,
      glm::dvec3 &force_1, glm::dvec3 &force_2,
      glm::dvec3 &torque_1, glm::dvec3 &torque_2
  ) {
    const auto p1 = obj_1.position;
    const auto p2 = obj_2.position;

    const double d = glm::distance(p1, p2);
    const double r1 = obj_1.radius;
    const double r2 = obj_2.radius;
    if (d > r1 + r2) {
      // no collision
      force_1 = {0, 0, 0};
      force_2 = {0, 0, 0};
      torque_1 = {0, 0, 0};
      torque_2 = {0, 0, 0};
      return;
    }
    // amount the spheres are compressed by
    const double x = r1 + r2 - d;

    const auto k1 = spring_constant(obj_1.mat);
    const auto k2 = spring_constant(obj_2.mat);

    // distance each sphere is compressed by
    // assume proportional to their spring constants
    const double x1 = x * k2 / (k1 + k2);
    const double x2 = x * k1 / (k1 + k2);
    
    // spring forces (sum of individual spring forces)
    const double spring_mag = k1 * x1 + k2 * x2;
    // normalized radius vector pointing out of 1
    const glm::dvec3 rad1 = glm::normalize(p2 - p1); // TODO this divides by 0 if p1 == p2!
    const glm::dvec3 rad2 = - rad1;
    force_1 = spring_mag * rad2;

    force_2 = - force_1;

    // calculate tangential friction forces
    const double mu = static_friction(obj_1.mat, obj_2.mat);

    const glm::dvec3 fforce_1 = mu * spring_mag * friction_dir(obj_1.velocity, force_1);
    const glm::dvec3 fforce_2 = mu * spring_mag * friction_dir(obj_2.velocity, force_1);
    
    // convert to torque
    // torque = r x F
    torque_1 = glm::cross((r1 - x1) * rad1, fforce_1);
    torque_2 = glm::cross((r2 - x2) * rad2, fforce_2);
  }
}

