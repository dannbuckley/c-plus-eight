// c-plus-eight.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <memory>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include "Chip8.h"

int main(int argc, char* argv[])
{
    try {
        auto logger = spdlog::stdout_color_mt("logger");
    }
    catch (spdlog::spdlog_ex& e) {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }

#ifdef PRINT_OPCODES
    spdlog::set_level(spdlog::level::debug);
#endif

    std::unique_ptr<c_plus_eight::Chip8> emu = std::make_unique<c_plus_eight::Chip8>();
    if (!emu->load_game("c8games/INVADERS")) {
        return EXIT_FAILURE;
    }

    // main display loop
    try {
        bool active = true;
        SDL_Event e;
        auto s_time = SDL_GetTicks();
        while (active) {
            // handle SDL events
            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
                    active = false;
                }
                else if (e.type == SDL_KEYDOWN) {
                    uint8_t key = 0xFF;
                    switch (e.key.keysym.sym) {
                        /* row 1 key pressed */
                    case SDLK_1:
                        key = 0x1;
                        break;
                    case SDLK_2:
                        key = 0x2;
                        break;
                    case SDLK_3:
                        key = 0x3;
                        break;
                    case SDLK_4:
                        key = 0xC;
                        break;

                        /* row 2 key pressed */
                    case SDLK_q:
                        key = 0x4;
                        break;
                    case SDLK_w:
                        key = 0x5;
                        break;
                    case SDLK_e:
                        key = 0x6;
                        break;
                    case SDLK_r:
                        key = 0xD;
                        break;

                        /* row 3 key pressed */
                    case SDLK_a:
                        key = 0x7;
                        break;
                    case SDLK_s:
                        key = 0x8;
                        break;
                    case SDLK_d:
                        key = 0x9;
                        break;
                    case SDLK_f:
                        key = 0xE;
                        break;

                        /* row 4 key pressed */
                    case SDLK_z:
                        key = 0xA;
                        break;
                    case SDLK_x:
                        key = 0x0;
                        break;
                    case SDLK_c:
                        key = 0xB;
                        break;
                    case SDLK_v:
                        key = 0xF;
                        break;
                    default:
                        // invalid key pressed, ignore event
                        break;
                    }

                    if (key != 0xFF) {
                        emu->key_press(key);
                    }
                }
                else if (e.type == SDL_KEYUP) {
                    uint8_t key = 0xFF;
                    switch (e.key.keysym.sym) {
                        /* row 1 key released */
                    case SDLK_1:
                        key = 0x1;
                        break;
                    case SDLK_2:
                        key = 0x2;
                        break;
                    case SDLK_3:
                        key = 0x3;
                        break;
                    case SDLK_4:
                        key = 0xC;
                        break;

                        /* row 2 key released */
                    case SDLK_q:
                        key = 0x4;
                        break;
                    case SDLK_w:
                        key = 0x5;
                        break;
                    case SDLK_e:
                        key = 0x6;
                        break;
                    case SDLK_r:
                        key = 0xD;
                        break;

                        /* row 3 key released */
                    case SDLK_a:
                        key = 0x7;
                        break;
                    case SDLK_s:
                        key = 0x8;
                        break;
                    case SDLK_d:
                        key = 0x9;
                        break;
                    case SDLK_f:
                        key = 0xE;
                        break;

                        /* row 4 key released */
                    case SDLK_z:
                        key = 0xA;
                        break;
                    case SDLK_x:
                        key = 0x0;
                        break;
                    case SDLK_c:
                        key = 0xB;
                        break;
                    case SDLK_v:
                        key = 0xF;
                        break;
                    default:
                        // invalid key released, ignore event
                        break;
                    }

                    if (key != 0xFF) {
                        emu->key_release(key);
                    }
                }
            }

            emu->emulate_cycle();

            // 60 Hz tick rate
            auto n_time = SDL_GetTicks();
            if ((n_time - s_time) >= 17) {
                emu->tick();
            }
        }
    }
    catch (c_plus_eight::unknown_opcode_error& e) {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    emu->stop_emulation();

    return EXIT_SUCCESS;
}