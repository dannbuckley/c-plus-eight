/**
 * Chip8.h
 * Copyright (c) 2020 Daniel Buckley
 *
 * Based on http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
 */

#pragma once

#include <algorithm>
#include <array>
#include <bitset>
#include <cstdint>
#include <iterator>
#include <memory>
#include <random>
#include <stack>

#include "Renderer.h"

#define OPCODE_X(op)        (op & 0x0F00) >> 8
#define OPCODE_Y(op)        (op & 0x00F0) >> 4
#define OPCODE_NIBBLE(op)   (op & 0x000F)
#define OPCODE_BYTE(op)     (op & 0x00FF)
#define OPCODE_ADDR(op)     (op & 0x0FFF)

#define NEXT_INSTRUCTION    this->pc += 2

#define FONTSET_BYTES_PER_CHAR 5

#define SCREEN_ROWS 32
#define SCREEN_COLS 64

namespace c_plus_eight {
    struct unknown_opcode_error : public std::exception {
        const char* what() const throw() {
            return "Encountered an unknown opcode. Check log for more details.";
        }
    };

    class Chip8
    {
    private:
        /* Data for system font */
        const std::array<unsigned char, 80> fontset = {
            0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
            0x20, 0x60, 0x20, 0x20, 0x70, // 1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
            0x90, 0x90, 0xF0, 0x10, 0x10, // 4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
            0xF0, 0x10, 0x20, 0x40, 0x40, // 7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
            0xF0, 0x90, 0xF0, 0x90, 0x90, // A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
            0xF0, 0x80, 0x80, 0x80, 0xF0, // C
            0xE0, 0x90, 0x90, 0x90, 0xE0, // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
            0xF0, 0x80, 0xF0, 0x80, 0x80  // F
        };

        /* Current operation */
        uint16_t opcode = 0;

        /* System memory */
        std::array<uint8_t, 4096> memory = {};

        /* General purpose registers */
        std::array<uint8_t, 16> V = {};

        /* Index register */
        uint16_t I = 0;

        /* Program counter (initialized to start of program memory) */
        uint16_t pc = 0x200;

        /* System graphics */
        std::array<uint8_t, SCREEN_ROWS * SCREEN_COLS> graphics = {};

        /* System timers */

        uint8_t delay_timer = 0;
        uint8_t sound_timer = 0;

        /* System stack */
        std::stack<uint16_t> stack;

        /* Random device for "RND Vx, byte" */
        std::random_device rd;

        /* System keypad state */
        std::bitset<16> key = 0;

        /* Flag for updating OpenGL pixel buffer */
        bool update_screen = true;

        /* OpenGL renderer object */
        std::unique_ptr<Renderer> r;

        /* Opcode functions */

        void op_cls();
        void op_ret();
        void op_jp_nnn(uint16_t nnn);
        void op_call_nnn(uint16_t nnn);
        void op_se_x_kk(uint8_t x, uint8_t kk);
        void op_sne_x_kk(uint8_t x, uint8_t kk);
        void op_se_x_y(uint8_t x, uint8_t y);
        void op_ld_x_kk(uint8_t x, uint8_t kk);
        void op_add_x_kk(uint8_t x, uint8_t kk);
        void op_ld_x_y(uint8_t x, uint8_t y);
        void op_or_x_y(uint8_t x, uint8_t y);
        void op_and_x_y(uint8_t x, uint8_t y);
        void op_xor_x_y(uint8_t x, uint8_t y);
        void op_add_x_y(uint8_t x, uint8_t y);
        void op_sub_x_y(uint8_t x, uint8_t y);
        void op_shr_x(uint8_t x);
        void op_subn_x_y(uint8_t x, uint8_t y);
        void op_shl_x(uint8_t x);
        void op_sne_x_y(uint8_t x, uint8_t y);
        void op_ld_I_nnn(uint16_t nnn);
        void op_jp_0_nnn(uint16_t nnn);
        void op_rnd_x_kk(uint8_t x, uint8_t kk);
        void op_drw_x_y_n(uint8_t x, uint8_t y, uint8_t n);
        void op_skp_x(uint8_t x);
        void op_sknp_x(uint8_t x);
        void op_ld_x_DT(uint8_t x);
        void op_ld_x_K(uint8_t x);
        void op_ld_DT_x(uint8_t x);
        void op_ld_ST_x(uint8_t x);
        void op_add_I_x(uint8_t x);
        void op_ld_F_x(uint8_t x);
        void op_ld_B_x(uint8_t x);
        void op_ld_intoI_x(uint8_t x);
        void op_ld_x_fromI(uint8_t x);

    public:
        Chip8() {
            this->r = std::make_unique<Renderer>();
            std::copy(std::begin(fontset), std::end(fontset), std::begin(this->memory));
        }

        /* Functions for controlling the system externally */

        bool load_game(const char* file_path);
        void key_press(uint8_t key_val);
        void key_release(uint8_t key_val);
        void emulate_cycle();
        void tick();
    };
}