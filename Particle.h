#ifndef PARTICLE_H
#define PARTICLE_H

#include "splashkit.h"
#include <algorithm> // For std::remove_if
#include <vector>
#include <cmath>
#include <thread>
#include <iostream>

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
void parallel_act(std::vector<Particle>& particles);
void process_particle_chunk(std::vector<Particle>& particles, int start, int end);

Particle::Particle(point_2d pos, vector_2d vel, vector_2d acc, color c, int life, int size = 3, int type = 0)
        : position(pos), velocity(vel), acc(acc), clr(c), lifespan(life), max_lifespan(life), size(size), type(type), mass(pow(size, 2)) {}

void Particle::update() {
    acc.x = clamp(acc.x, MIN_BLOCK_ACC, MAX_BLOCK_ACC);
    acc.y = clamp(acc.y, MIN_BLOCK_ACC, MAX_BLOCK_ACC);
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

    const float G = 1.0f;

    // Calculate the mass ratio and the exponential decay factor
    float mass_ratio = mass / other.mass;
//    std::cout << "mass ratio: " << mass_ratio << cout::endl;
    float decay_factor = exp(-mass_ratio) * 3.;

    // Calculate the force with the exponential decay factor
    float force = G * (mass * other.mass) / pow(distance, 2) * decay_factor;
//    if (type == 0) {
//        force *= -1;
//    }
    float accel = force / mass;  // Use the current particle's mass

    acc.x += accel * direction.x;  // Apply the acceleration to the current particle
    acc.y += accel * direction.y;  // Apply the acceleration to the current particle
}



void Particle::draw() {
    fill_ellipse(clr, position.x - size / 2, position.y - size / 2, size, size);
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

void process_particle_chunk(std::vector<Particle>& particles, int start, int end) {
    for (int i = start; i < end; ++i) {
        for (int j = 0; j < particles.size(); ++j) {
            if (i != j && particles[i].position.x != particles[j].position.x && particles[i].position.y != particles[j].position.y) {
                particles[i].act(particles[j]);
            }
        }
    }
}

void parallel_act(std::vector<Particle>& particles) {
    const int num_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
//    std::cout << "Number of threads: " << num_threads << std::endl;
    int chunk_size = particles.size() / num_threads;

    for (int i = 0; i < num_threads; ++i) {
        int start = i * chunk_size;
        int end = (i == num_threads - 1) ? particles.size() : start + chunk_size;
        threads.emplace_back(process_particle_chunk, std::ref(particles), start, end);
    }

    for (auto& thread : threads) {
        thread.join();
    }
}

#endif // PARTICLE_H
