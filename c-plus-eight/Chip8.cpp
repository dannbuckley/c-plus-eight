/**
 * Chip8.cpp
 * Copyright (c) 2020 Daniel Buckley
 */

#include <stdio.h>
#include "Chip8.h"
#include "spdlog/spdlog.h"

namespace c_plus_eight {
	void Chip8::display_sprite(uint8_t x, uint8_t y, uint8_t n)
	{
		// set collision flag to 0
		this->V[0xF] = 0;

		// render sprite at memory location I
		for (uint8_t byte_index = 0; byte_index < n; byte_index++) {
			uint8_t byte = this->memory[I + byte_index];

			for (uint8_t bit_index = 0; bit_index < 8; bit_index++) {
				uint8_t bit = (byte >> bit_index) & 0x1;
				
				// calculate current row and column as it would appear in OpenGL
				uint8_t cur_row = SCREEN_ROWS - (((y + byte_index) % SCREEN_ROWS) + 1);
				uint8_t cur_col = (x + (7 - bit_index)) % SCREEN_COLS;

				// get pointer to current pixel
				uint8_t* pixelp = &this->graphics[(cur_row * SCREEN_COLS) + cur_col];

				if (bit) {
					// detect collision
					if (*pixelp == 0xFF) {
						this->V[0xF] = 1;
					} // end if (*pixelp == 0xFF)

					// toggle current pixel (use 0xFF for full luminance)
					*pixelp ^= 0xFF;
				} // end if (bit)

				pixelp = NULL;
			} // end for (bit_index)
		} // end for (byte_index)
	}

	bool Chip8::load_game(const char* file_path)
	{
		FILE* game;
		fopen_s(&game, file_path, "rb");

		if (game == NULL) {
			spdlog::get("logger")->error("Could not open file '{}'.", file_path);
			return false;
		} // end if

		// read in game data and store in memory at 0x200
		fread(&this->memory[0x200], 1, 4096 - 512, game);

		fclose(game);
		return true;
	}

	void Chip8::key_press(uint8_t key_val)
	{
		this->key[key_val] = 1;
	}

	void Chip8::key_release(uint8_t key_val)
	{
		this->key[key_val] = 0;
	}

	void Chip8::emulate_cycle()
	{
		// retrieve opcode from current memory position
		this->opcode = (this->memory[this->pc] << 8) | this->memory[this->pc + 1];

		// dissect opcode
		uint8_t x = OPCODE_X(this->opcode);
		uint8_t y = OPCODE_Y(this->opcode);
		uint8_t n = OPCODE_NIBBLE(this->opcode);
		uint8_t kk = OPCODE_BYTE(this->opcode);
		uint16_t nnn = OPCODE_ADDR(this->opcode);

		// decode opcode
		// instructions available at http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
		switch (this->opcode & 0xF000) {
		case 0x0000:
			switch (kk) {
			case 0xE0: // CLS
#ifdef PRINT_OPCODES
				spdlog::get("logger")->debug("CLS"); 
#endif
				// clear display
				this->graphics.fill(0);
				this->update_screen = true;
				NEXT_INSTRUCTION;
				break;

			case 0xEE: // RET
#ifdef PRINT_OPCODES
				spdlog::get("logger")->debug("RET");
#endif
				// return from a subroutine
				this->pc = this->stack[--this->sp];
				NEXT_INSTRUCTION;
				break;

			default:
				spdlog::get("logger")->error("Unknown opcode: {}", this->opcode);
				throw unknown_opcode_error();
			} // end switch (kk)
			break;
		case 0x1000: // JP addr
#ifdef PRINT_OPCODES
			spdlog::get("logger")->debug("JP {}", nnn);
#endif
			// jump to location nnn
			this->pc = nnn;
			break;

		case 0x2000: // CALL addr
#ifdef PRINT_OPCODES
			spdlog::get("logger")->debug("CALL {}", nnn);
#endif
			// call subroutine at nnn
			// place program counter at the top of the stack
			this->stack[this->sp] = this->pc;

			// increment stack pointer
			++this->sp;

			// set program counter to address
			this->pc = nnn;
			break;

		case 0x3000: // SE Vx, byte
#ifdef PRINT_OPCODES
			spdlog::get("logger")->debug("SE V{}, {}", x, kk);
#endif
			// skip next instruction if Vx == byte
			if (this->V[x] == kk) {
				NEXT_INSTRUCTION;
			} // end if

			NEXT_INSTRUCTION;
			break;

		case 0x4000: // SNE Vx, byte
#ifdef PRINT_OPCODES
			spdlog::get("logger")->debug("SNE V{}, {}", x, kk);
#endif
			// skip next instruction if Vx != byte
			if (this->V[x] != kk) {
				NEXT_INSTRUCTION;
			} // end if

			NEXT_INSTRUCTION;
			break;

		case 0x5000: // SE Vx, Vy
#ifdef PRINT_OPCODES
			spdlog::get("logger")->debug("SE V{}, V{}", x, y);
#endif
			// skip next instruction if Vx == Vy
			if (this->V[x] == this->V[y]) {
				NEXT_INSTRUCTION;
			} // end if

			NEXT_INSTRUCTION;
			break;

		case 0x6000: // LD Vx, byte
#ifdef PRINT_OPCODES
			spdlog::get("logger")->debug("LD V{}, {}", x, kk);
#endif
			// set Vx = byte
			this->V[x] = kk;
			NEXT_INSTRUCTION;
			break;

		case 0x7000: // ADD Vx, byte
#ifdef PRINT_OPCODES
			spdlog::get("logger")->debug("ADD V{}, {}", x, kk);
#endif
			// set Vx = Vx + kk
			this->V[x] += kk;
			NEXT_INSTRUCTION;
			break;

		case 0x8000:
			switch (n) {
			case 0x0: // LD Vx, Vy
#ifdef PRINT_OPCODES
				spdlog::get("logger")->debug("LD V{}, V{}", x, y);
#endif
				// set Vx = Vy
				this->V[x] = this->V[y];
				break;

			case 0x1: // OR Vx, Vy
#ifdef PRINT_OPCODES
				spdlog::get("logger")->debug("OR V{}, V{}", x, y);
#endif
				// set Vx = Vx OR Vy
				this->V[x] |= this->V[y];
				break;

			case 0x2: // AND Vx, Vy
#ifdef PRINT_OPCODES
				spdlog::get("logger")->debug("AND V{}, V{}", x, y);
#endif
				// set Vx = Vx AND Vy
				this->V[x] &= this->V[y];
				break;

			case 0x3: // XOR Vx, Vy
#ifdef PRINT_OPCODES
				spdlog::get("logger")->debug("XOR V{}, V{}", x, y);
#endif
				// set Vx = Vx XOR Vy
				this->V[x] ^= this->V[y];
				break;

			case 0x4: // ADD Vx, Vy
#ifdef PRINT_OPCODES
				spdlog::get("logger")->debug("ADD V{}, V{}", x, y);
#endif
				// set Vx = Vx + Vy, set VF = carry
				if (this->V[y] > (0xFF - this->V[x])) {
					this->V[0xF] = 1; // carry
				}
				else {
					this->V[0xF] = 0;
				} // end if

				this->V[x] += this->V[y];
				break;

			case 0x5: // SUB Vx, Vy
#ifdef PRINT_OPCODES
				spdlog::get("logger")->debug("SUB V{}, V{}", x, y);
#endif
				// set Vx = Vx - Vy, set VF = NOT borrow
				if (this->V[x] > this->V[y]) {
					this->V[0xF] = 1; // NOT borrow
				}
				else {
					this->V[0xF] = 0;
				} // end if

				this->V[x] -= this->V[y];
				break;

			case 0x6: // SHR Vx {, Vy}
#ifdef PRINT_OPCODES
				spdlog::get("logger")->debug("SHR V{}, (, V{})", x, y);
#endif
				// set Vx = Vx SHR 1
				this->V[0xF] = V[x] & 0x1;
				this->V[x] >>= 1;
				break;

			case 0x7: // SUBN Vx, Vy
#ifdef PRINT_OPCODES
				spdlog::get("logger")->debug("SUBN V{}, V{}", x, y);
#endif
				// set Vx = Vy - Vx, set VF = NOT borrow
				if (this->V[y] > this->V[x]) {
					this->V[0xF] = 1; // NOT borrow
				}
				else {
					this->V[0xF] = 0;
				} // end if

				this->V[x] = this->V[y] - this->V[x];
				break;

			case 0xE: // SHL Vx {, Vy} 
#ifdef PRINT_OPCODES
				spdlog::get("logger")->debug("SHL V{} (, V{})", x, y);
#endif
				// set Vx = Vx SHL 1
				this->V[0xF] = (this->V[x] & 0x80) >> 7;
				this->V[x] <<= 1;
				break;

			default:
				spdlog::get("logger")->error("Unknown opcode: {}", this->opcode);
				throw unknown_opcode_error();
			} // end switch (n)
			NEXT_INSTRUCTION;
			break;

		case 0x9000: // SNE Vx, Vy
#ifdef PRINT_OPCODES
			spdlog::get("logger")->debug("SNE V{}, V{}", x, y);
#endif
			// skip next instruction if Vx != Vy
			if (this->V[x] != this->V[y]) {
				NEXT_INSTRUCTION;
			} // end if

			NEXT_INSTRUCTION;
			break;

		case 0xA000: // LD I, addr
#ifdef PRINT_OPCODES
			spdlog::get("logger")->debug("LD I, {}", nnn);
#endif
			// set I = nnn
			this->I = nnn;
			NEXT_INSTRUCTION;
			break;

		case 0xB000: // JP V0, addr
#ifdef PRINT_OPCODES
			spdlog::get("logger")->debug("JP V{}, {}", this->V[0], nnn);
#endif
			// jump to location nnn + V0
			this->pc = nnn + this->V[0];
			break;

		case 0xC000: // RND Vx, byte
#ifdef PRINT_OPCODES
			spdlog::get("logger")->debug("RND V{}, {}", x, kk);
#endif
			// set Vx = random byte AND kk
			this->V[x] = (rd() % 0xFF) & kk;
			NEXT_INSTRUCTION;
			break;

		case 0xD000: // DRW Vx, Vy, nibble
#ifdef PRINT_OPCODES
			spdlog::get("logger")->debug("DRW V{}, V{}, {}", x, y, n);
#endif
			// display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
			this->display_sprite(this->V[x], this->V[y], n);
			NEXT_INSTRUCTION;
			this->update_screen = true;
			break;

		case 0xE000:
			switch (kk) {
			case 0x9E: // SKP Vx
#ifdef PRINT_OPCODES
				spdlog::get("logger")->debug("SKP V{}", x);
#endif
				// skip next instruction if key with the value of Vx is pressed
				if (this->key[this->V[x]]) {
					NEXT_INSTRUCTION;
				} // end if
				break;

			case 0xA1: // SKNP Vx
#ifdef PRINT_OPCODES
				spdlog::get("logger")->debug("SKNP V{}", x);
#endif
				// skip next instruction if key with the value of Vx is not pressed
				if (!this->key[this->V[x]]) {
					NEXT_INSTRUCTION;
				} // end if
				break;

			default:
				spdlog::get("logger")->error("Unknown opcode: {}", this->opcode);
				throw unknown_opcode_error();
			} // end switch (kk)
			NEXT_INSTRUCTION;
			break;

		case 0xF000:
			switch (kk) {
			case 0x07: // LD Vx, DT
#ifdef PRINT_OPCODES
				spdlog::get("logger")->debug("LD V{}, {}", x, this->delay_timer);
#endif
				// set Vx = delay timer value
				this->V[x] = this->delay_timer;
				NEXT_INSTRUCTION;
				break;

			case 0x0A: // LD Vx, K
#ifdef PRINT_OPCODES
				spdlog::get("logger")->debug("LD V{}, K", x);
#endif
				// wait for a key press, store the value of the key in Vx
				for (int i = 0; i < 16; i++) {
					if (this->key[i]) {
						this->V[x] = i;
						NEXT_INSTRUCTION;
						break;
					} // end if
				} // end for
				break;

			case 0x15: // LD DT, Vx
#ifdef PRINT_OPCODES
				spdlog::get("logger")->debug("LD DT, V{}", x);
#endif
				// set delay timer = Vx
				this->delay_timer = this->V[x];
				NEXT_INSTRUCTION;
				break;

			case 0x18: // LD ST, Vx
#ifdef PRINT_OPCODES
				spdlog::get("logger")->debug("LD ST, V{}", x);
#endif
				// set sound timer = Vx
				this->sound_timer = this->V[x];
				NEXT_INSTRUCTION;
				break;

			case 0x1E: /// ADD I, Vx
#ifdef PRINT_OPCODES
				spdlog::get("logger")->debug("ADD I, V{}", x);
#endif
				// set I = I + Vx
				this->I += this->V[x];
				NEXT_INSTRUCTION;
				break;

			case 0x29: // LD F, Vx
#ifdef PRINT_OPCODES
				spdlog::get("logger")->debug("LD F, {}", x);
#endif
				// set I = location of sprite for digit Vx
				this->I = FONTSET_BYTES_PER_CHAR * this->V[x];
				NEXT_INSTRUCTION;
				break;

			case 0x33: // LD B, Vx
#ifdef PRINT_OPCODES
				spdlog::get("logger")->debug("LD B, V{}", x);
#endif
				// store BCD representation of Vx in memory locations I, I+1, and I+2
				this->memory[this->I] = (this->V[x] / 100);
				this->memory[this->I + 1] = (this->V[x] / 10) % 10;
				this->memory[this->I + 2] = (this->V[x] % 10);
				NEXT_INSTRUCTION;
				break;

			case 0x55: // LD [I], Vx
#ifdef PRINT_OPCODES
				spdlog::get("logger")->debug("LD [I], V{}", x);
#endif
				// store registers V0 through Vx in memory starting at location I
				std::copy(this->V.begin(),
					std::next(this->V.begin(), x + 1),
					std::next(this->memory.begin(), I));

				// advance I by the number of bytes stored
				this->I += x + 1;
				NEXT_INSTRUCTION;
				break;

			case 0x65: // LD Vx, [I]
#ifdef PRINT_OPCODES
				spdlog::get("logger")->debug("LD V{}, [I]", x);
#endif
				// read registers V0 through Vx from memory starting at location I
				std::copy(std::next(this->memory.begin(), I),
					std::next(this->memory.begin(), I + x + 1),
					this->V.begin());

				// advance I by the number of bytes read
				this->I += x + 1;
				NEXT_INSTRUCTION;
				break;

			default:
				spdlog::get("logger")->error("Unknown opcode: {}", this->opcode);
				throw unknown_opcode_error();
			} // end switch (kk)
			break;
		default:
			spdlog::get("logger")->error("Unknown opcode: {}", this->opcode);
			throw unknown_opcode_error();
		} // end switch (opcode & 0xF000)

		if (this->update_screen) {
			r->draw(&this->graphics);
			this->update_screen = false;
		} // end if (update_screen)
	}

	void Chip8::tick()
	{
		if (this->delay_timer > 0) {
			--this->delay_timer;
		} // end if (delay_timer > 0)

		if (this->sound_timer > 0) {
			--sound_timer;
			if (this->sound_timer == 0) {
				spdlog::get("logger")->info("Sound timer reached 0.");
			} // end if (sound_timer == 0)
		} // end if (sound_timer > 0)
	}
}