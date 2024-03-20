#ifndef PARTICLE_H
#define PARTICLE_H

#include "splashkit.h"
#include <algorithm> // For std::remove_if
#include <vector>
#include <cmath>

struct Particle {
    point_2d position;
    vector_2d velocity;
    vector_2d acc;
    color clr;
    int lifespan;
    int max_lifespan;
    int size;
    int type; // 0 = pull, 1 = push
    int mass;

    Particle(point_2d pos, vector_2d vel, vector_2d acc, color c, int life, int size, int type);

    void update();
    void draw();
    void act(Particle& other);
};

void update_particles(std::vector<Particle>& particles);
void draw_particles(std::vector<Particle>& particles);

Particle::Particle(point_2d pos, vector_2d vel, vector_2d acc, color c, int life, int size = 3, int type = 0)
        : position(pos), velocity(vel), acc(acc), clr(c), lifespan(life), max_lifespan(life), size(size), type(type), mass(pow(size, 4)) {}

void Particle::update() {
    velocity.x += acc.x;
    velocity.y += acc.y;
    // clamp
    velocity.x = clamp(velocity.x, MIN_BLOCK_VEL, MAX_BLOCK_VEL);
    velocity.y = clamp(velocity.y, MIN_BLOCK_VEL, MAX_BLOCK_VEL);
    position.x += velocity.x;
    position.y += velocity.y;
    //wrap edges
    if (position.x > screen_width()) {
        position.x = 0;
    }
    if (position.x < 0) {
        position.x = screen_width();
    }
    if (position.y > screen_height()) {
        position.y = 0;
    }
    if (position.y < 0) {
        position.y = screen_height();
    }
}

void Particle::act(Particle& other) {
    vector_2d direction = {other.position.x - position.x, other.position.y - position.y};
    float distance = sqrt(pow(direction.x, 2) + pow(direction.y, 2));

    // Normalize the direction vector
    direction.x /= distance;
    direction.y /= distance;

    const float G = 0.3;

    float force = G * (mass * other.mass) / pow(distance, 2);
    if (type == 0) {
        force *= -1;
    }
    float accel = force / other.mass;

    other.acc.x += accel * direction.x;
    other.acc.y += accel * direction.y;
    other.acc.x = clamp(other.acc.x, MIN_BLOCK_ACC, MAX_BLOCK_ACC);
    other.acc.y = clamp(other.acc.y, MIN_BLOCK_ACC, MAX_BLOCK_ACC);
}

void Particle::draw() {
    fill_ellipse(clr, position.x, position.y, size, size);
}

void update_particles(std::vector<Particle>& particles) {
    for (auto& p : particles) {
        p.update();
    }
}

void draw_particles(std::vector<Particle>& particles) {
    for (auto& p : particles) {
        p.draw();
    }
}

#endif // PARTICLE_H
