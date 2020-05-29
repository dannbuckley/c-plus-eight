/**
 * Chip8.h
 * Copyright (c) 2020 Daniel Buckley
 *
 * Based on http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
 */

#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <iterator>
#include <memory>
#include <random>

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

        uint16_t opcode = 0;
        std::array<uint8_t, 4096> memory = {};
        std::array<uint8_t, 16> V = {};
        uint16_t I = 0;
        uint16_t pc = 0x200;
        std::array<uint8_t, SCREEN_ROWS * SCREEN_COLS> graphics = {};

        uint8_t delay_timer = 0;
        uint8_t sound_timer = 0;

        std::array<uint16_t, 16> stack = {};
        uint16_t sp = 0;

        std::random_device rd;
        std::array<uint8_t, 16> key = {};

        bool update_screen = true;

        std::unique_ptr<Renderer> r;

        void display_sprite(uint8_t x, uint8_t y, uint8_t n);

    public:
        Chip8() {
            this->r = std::make_unique<Renderer>();
            std::copy(std::begin(fontset), std::end(fontset), std::begin(this->memory));
        }

        bool load_game(const char* file_path);
        void key_press(uint8_t key_val);
        void key_release(uint8_t key_val);
        void emulate_cycle();
        void tick();
    };
}