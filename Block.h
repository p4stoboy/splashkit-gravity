#ifndef BLOCK_H
#define BLOCK_H

#include "splashkit.h"
#include "Particle.h"
#include "XOR.h"
#include <vector>
#include <algorithm>
#include "splashkit.h"
#include "Settings.h"
#include "Util.h"

struct Block {
    point_2d position;
    vector_2d velocity;
    vector_2d acceleration;
    color clr;
    float size;
    bool is_destroyed;

    Block(point_2d pos, vector_2d vel, vector_2d accel, color c, float s);

    void update(std::vector<Particle>& particles, XOR& rng);
    void draw();
    bool check_collision(const Block& other) const;
    void destroy(std::vector<Particle>& particles, XOR& rng);
};

void update_blocks(vector<Block> &blocks, std::vector<Particle>& particles, XOR& rng);
void draw_blocks(std::vector<Block>& blocks);

Block::Block(point_2d pos, vector_2d vel, vector_2d accel, color c, float s)
        : position(pos), velocity(vel), acceleration(accel), clr(c), size(s), is_destroyed(false) {}

void Block::update(std::vector<Particle>& particles, XOR& rng) {
//    velocity.x += acceleration.x;
    velocity.y += acceleration.y;
    velocity.y = clamp(velocity.y, MIN_BLOCK_VEL, MAX_BLOCK_VEL);
//    position.x += velocity.x;
    position.y += velocity.y;
    if (position.y > screen_height() - size) {
        destroy(particles, rng);
        is_destroyed = true;
    }
}

void Block::draw() {
    if (!is_destroyed) {
        fill_rectangle(clr, position.x, position.y, size, size);
    }
}

bool Block::check_collision(const Block& other) const {
    return (position.x < other.position.x + other.size &&
            position.x + size > other.position.x &&
            position.y < other.position.y + other.size &&
            position.y + size > other.position.y);
}

void Block::destroy(std::vector<Particle>& particles, XOR& rng) {
    for (int i = 0; i < size; ++i) {
        vector_2d particle_vel = {rng.randomFloat(-2, 2), rng.randomFloat(-4, 0)}; // can't have downward trajectory
        particles.push_back(Particle(position, particle_vel, {0.2, 0.2}, clr, 300, rng.randomInt(2, 6)));
    }
}

void update_blocks(std::vector<Block>& blocks, std::vector<Particle>& particles, XOR& rng) {
    for (auto& block : blocks) {
        block.update(particles, rng);
    }

    blocks.erase(std::remove_if(blocks.begin(), blocks.end(),
                                [](const Block& block) { return block.is_destroyed; }),
                 blocks.end());
}

void draw_blocks(std::vector<Block>& blocks) {
    for (auto& block : blocks) {
        block.draw();
    }
}
#endif // BLOCK_H
