#include "splashkit.h"
#include "XOR.h"
#include "Settings.h"
#include "Util.h"
#include "Block.h"
#include <chrono>

using std::chrono::steady_clock;
using std::chrono::time_point;
using std::chrono::duration_cast;
using std::chrono::seconds;

enum GameState {
    MENU,
    PLAYING,
    GAME_OVER
};

#include <iostream>

void print_particle_info(const Particle& particle1, const Particle& particle2) {
    std::cout << "Particle 1 Position: (" << particle1.position.x << ", " << particle1.position.y << ")\n";
    std::cout << "Particle 2 Position: (" << particle2.position.x << ", " << particle2.position.y << ")\n";
    std::cout << "Particle 1 Acceleration: (" << particle1.acc.x << ", " << particle1.acc.y << ")\n";
    std::cout << "Particle 2 Acceleration: (" << particle2.acc.x << ", " << particle2.acc.y << ")\n";
}

int get_fps(int& frames, time_point<steady_clock>& start_time) {
    auto current_time = steady_clock::now();
    auto elapsed_time = duration_cast<seconds>(current_time - start_time).count();

    if (elapsed_time >= 1) {
        int fps = frames / elapsed_time;
        frames = 0;
        start_time = current_time;

        return fps;
    }
    return -1;
}

int main()
{
    // instantiate game state, values, input controls, and random number generator
    open_window("Particle_Gravity", 1000, 1000);
    hide_mouse();
    GameState state = PLAYING;
    XOR rng = XOR();
    vector<Particle> particles;
    Block player = Block({screen_width() / 2, screen_height() - 20}, {0, 0}, {0, 0}, COLOR_BLACK, 5);
    int frames = 0;
    int fps = 0;
    int p_size = 0;
    bool m_down = false;
    time_point<steady_clock> start_time = steady_clock::now();

    // game loop
    while (!quit_requested()) {
        process_events();
        clear_screen(color_from_hex("#474143"));
        if (state == PLAYING) {
            frames++;
            int new_fps = get_fps(frames, start_time);
            if (new_fps != -1) {
                fps = new_fps;
            }
            draw_text("Particles: " + std::to_string(particles.size()), COLOR_BLACK, 20, 20);
            draw_text("Frame rate: " + std::to_string(fps), COLOR_BLACK, 20, 40);
            player.position.x = mouse_x();
            player.position.y = mouse_y();
            // hold to grow particle size, release to spawn
            if (mouse_down(LEFT_BUTTON)) {
                p_size++;
                m_down = true;
            }
            if (mouse_up(LEFT_BUTTON) && m_down) {
                particles.push_back(Particle({player.position.x, player.position.y}, {0, 0}, {0, 0}, choose(space_colors, 7), 100, (p_size/10) | 0 + 2, 0)); //rng.randomInt(0,1)));
                p_size = 0;
                m_down = false;
            }
            // hold to continuously spawn random particles
            if (mouse_down(RIGHT_BUTTON)) {
                particles.push_back(Particle({player.position.x, player.position.y}, {0, 0}, {0, 0}, choose(space_colors, 7), 100, rng.randomInt(1, 6), 0)); //rng.randomInt(0,1)));
            }
            if (key_down(SPACE_KEY)) {
                particles.clear();
            }
            update_particles(particles);
            if (particles.size() == 2) {
                print_particle_info(particles[0], particles[1]);
            }
            draw_particles(particles);
            for (auto& p : particles) {
                for (auto& other : particles) {
                    if (p.position.x != other.position.x && p.position.y != other.position.y) {
                        p.act(other);
                    }
                }
            }
            player.draw();
        }
        refresh_screen(120);
    }
    return 0;
}
