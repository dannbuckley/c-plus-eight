/**
 * Chip8.cpp
 * Copyright (c) 2020 Daniel Buckley
 */

#include <stdio.h>
#include "Chip8.h"
#include "spdlog/spdlog.h"

namespace c_plus_eight {
	// Clear display
	void Chip8::op_cls()
	{
#ifdef PRINT_OPCODES
		spdlog::get("logger")->debug("CLS");
#endif
		this->graphics.fill(0);
		this->update_screen = true;
		NEXT_INSTRUCTION;
	} // end Chip8::op_cls()

	// Return from a subroutine
	void Chip8::op_ret()
	{
#ifdef PRINT_OPCODES
		spdlog::get("logger")->debug("RET");
#endif
		// retrieve previous address from top of stack
		this->pc = this->stack.top();

		// "decrement stack pointer" by popping top address off of the stack
		this->stack.pop();
		NEXT_INSTRUCTION;
	} // end Chip8::op_ret()

	// Jump to location nnn
	void Chip8::op_jp_nnn(uint16_t nnn)
	{
#ifdef PRINT_OPCODES
		spdlog::get("logger")->debug("JP {}", nnn);
#endif
		this->pc = nnn;
	} // end Chip8::op_jp_nnn()

	// Call subroutine at nnn
	void Chip8::op_call_nnn(uint16_t nnn)
	{
#ifdef PRINT_OPCODES
		spdlog::get("logger")->debug("CALL {}", nnn);
#endif
		// place program counter at the top of the stack
		// and simultaneously "increment the stack pointer"
		this->stack.push(this->pc);

		// set program counter to address
		this->pc = nnn;
	} // end Chip8::op_call_nnn()

	// Skip next instruction if Vx == byte
	void Chip8::op_se_x_kk(uint8_t x, uint8_t kk)
	{
#ifdef PRINT_OPCODES
		spdlog::get("logger")->debug("SE V{}, {}", x, kk);
#endif
		if (this->V[x] == kk) {
			NEXT_INSTRUCTION;
		}
		NEXT_INSTRUCTION;
	} // end Chip8::op_se_x_kk()

	// Skip next instruction if Vx != byte
	void Chip8::op_sne_x_kk(uint8_t x, uint8_t kk)
	{
#ifdef PRINT_OPCODES
		spdlog::get("logger")->debug("SNE V{}, {}", x, kk);
#endif
		if (this->V[x] != kk) {
			NEXT_INSTRUCTION;
		}
		NEXT_INSTRUCTION;
	} // end Chip8::op_sne_x_kk()

	// Skip next instruction if Vx == Vy
	void Chip8::op_se_x_y(uint8_t x, uint8_t y)
	{
#ifdef PRINT_OPCODES
		spdlog::get("logger")->debug("SE V{}, V{}", x, y);
#endif
		if (this->V[x] == this->V[y]) {
			NEXT_INSTRUCTION;
		}
		NEXT_INSTRUCTION;
	} // end Chip8::op_se_x_y()

	// Set Vx = byte
	void Chip8::op_ld_x_kk(uint8_t x, uint8_t kk)
	{
#ifdef PRINT_OPCODES
		spdlog::get("logger")->debug("LD V{}, {}", x, kk);
#endif
		this->V[x] = kk;
		NEXT_INSTRUCTION;
	} // end Chip8::op_ld_x_kk()

	// Set Vx = Vx + kk
	void Chip8::op_add_x_kk(uint8_t x, uint8_t kk)
	{
#ifdef PRINT_OPCODES
		spdlog::get("logger")->debug("ADD V{}, {}", x, kk);
#endif
		this->V[x] += kk;
		NEXT_INSTRUCTION;
	} // end Chip8::op_add_x_kk()

	// Set Vx = Vy
	void Chip8::op_ld_x_y(uint8_t x, uint8_t y)
	{
#ifdef PRINT_OPCODES
		spdlog::get("logger")->debug("LD V{}, V{}", x, y);
#endif
		this->V[x] = this->V[y];
		NEXT_INSTRUCTION;
	} // end Chip8::op_ld_x_y()

	// Set Vx = Vx OR Vy
	void Chip8::op_or_x_y(uint8_t x, uint8_t y)
	{
#ifdef PRINT_OPCODES
		spdlog::get("logger")->debug("OR V{}, V{}", x, y);
#endif
		this->V[x] |= this->V[y];
		NEXT_INSTRUCTION;
	} // end Chip8::op_or_x_y()

	// Set Vx = Vx AND Vy
	void Chip8::op_and_x_y(uint8_t x, uint8_t y)
	{
#ifdef PRINT_OPCODES
		spdlog::get("logger")->debug("AND V{}, V{}", x, y);
#endif
		this->V[x] &= this->V[y];
		NEXT_INSTRUCTION;
	} // end Chip8::op_and_x_y()

	// Set Vx = Vx XOR Vy
	void Chip8::op_xor_x_y(uint8_t x, uint8_t y)
	{
#ifdef PRINT_OPCODES
		spdlog::get("logger")->debug("XOR V{}, V{}", x, y);
#endif
		this->V[x] ^= this->V[y];
		NEXT_INSTRUCTION;
	} // end Chip8::op_xor_x_y()

	// Set Vx = Vx + Vy, set VF = carry
	void Chip8::op_add_x_y(uint8_t x, uint8_t y)
	{
#ifdef PRINT_OPCODES
		spdlog::get("logger")->debug("ADD V{}, V{}", x, y);
#endif
		if (this->V[y] > (0xFF - this->V[x])) {
			this->V[0xF] = 1; // carry
		}
		else {
			this->V[0xF] = 0;
		}

		this->V[x] += this->V[y];
		NEXT_INSTRUCTION;
	} // end Chip8::op_add_x_y()

	// Set Vx = Vx - Vy, set VF = NOT borrow
	void Chip8::op_sub_x_y(uint8_t x, uint8_t y)
	{
#ifdef PRINT_OPCODES
		spdlog::get("logger")->debug("SUB V{}, V{}", x, y);
#endif
		if (this->V[x] > this->V[y]) {
			this->V[0xF] = 1; // NOT borrow
		}
		else {
			this->V[0xF] = 0;
		}

		this->V[x] -= this->V[y];
		NEXT_INSTRUCTION;
	} // end Chip8::op_sub_x_y()

	// Set Vx = Vx SHR 1
	void Chip8::op_shr_x(uint8_t x)
	{
#ifdef PRINT_OPCODES
		spdlog::get("logger")->debug("SHR V{}, (, V{})", x, y);
#endif
		this->V[0xF] = V[x] & 0x1;
		this->V[x] >>= 1;
		NEXT_INSTRUCTION;
	} // end Chip8::op_shr_x()

	// Set Vx = Vy - Vx, set VF = NOT borrow
	void Chip8::op_subn_x_y(uint8_t x, uint8_t y)
	{
#ifdef PRINT_OPCODES
		spdlog::get("logger")->debug("SUBN V{}, V{}", x, y);
#endif
		if (this->V[y] > this->V[x]) {
			this->V[0xF] = 1; // NOT borrow
		}
		else {
			this->V[0xF] = 0;
		}

		this->V[x] = this->V[y] - this->V[x];
		NEXT_INSTRUCTION;
	} // end Chip8::op_subn_x_y()

	// Set Vx = Vx SHL 1
	void Chip8::op_shl_x(uint8_t x)
	{
#ifdef PRINT_OPCODES
		spdlog::get("logger")->debug("SHL V{} (, V{})", x, y);
#endif
		this->V[0xF] = (this->V[x] & 0x80) >> 7;
		this->V[x] <<= 1;
		NEXT_INSTRUCTION;
	} // end Chip8::op_shl_x()

	// Skip next instruction if Vx != Vy
	void Chip8::op_sne_x_y(uint8_t x, uint8_t y)
	{
#ifdef PRINT_OPCODES
		spdlog::get("logger")->debug("SNE V{}, V{}", x, y);
#endif
		if (this->V[x] != this->V[y]) {
			NEXT_INSTRUCTION;
		}

		NEXT_INSTRUCTION;
	} // end Chip8::op_sne_x_y()

	// Set I = nnn
	void Chip8::op_ld_I_nnn(uint16_t nnn)
	{
#ifdef PRINT_OPCODES
		spdlog::get("logger")->debug("LD I, {}", nnn);
#endif
		this->I = nnn;
		NEXT_INSTRUCTION;
	} // end Chip8::op_ld_I_nnn()

	// Jump to location nnn + V0
	void Chip8::op_jp_0_nnn(uint16_t nnn)
	{
#ifdef PRINT_OPCODES
		spdlog::get("logger")->debug("JP V{}, {}", this->V[0], nnn);
#endif
		this->pc = nnn + this->V[0];
	} // end Chip8::op_jp_0_nnn()

	// Set Vx = random byte AND kk
	void Chip8::op_rnd_x_kk(uint8_t x, uint8_t kk)
	{
#ifdef PRINT_OPCODES
		spdlog::get("logger")->debug("RND V{}, {}", x, kk);
#endif
		this->V[x] = (rd() % 0xFF) & kk;
		NEXT_INSTRUCTION;
	} // end Chip8::op_rnd_x_kk()

	// Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
	void Chip8::op_drw_x_y_n(uint8_t x, uint8_t y, uint8_t n)
	{
#ifdef PRINT_OPCODES
		spdlog::get("logger")->debug("DRW V{}, V{}, {}", x, y, n);
#endif
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

		NEXT_INSTRUCTION;

		// update OpenGL pixel buffer
		this->update_screen = true;
	} // end Chip8::op_drw_x_y_n()

	// Skip next instruction if key with the value of Vx is pressed
	void Chip8::op_skp_x(uint8_t x)
	{
#ifdef PRINT_OPCODES
		spdlog::get("logger")->debug("SKP V{}", x);
#endif
		if (this->key[this->V[x]]) {
			NEXT_INSTRUCTION;
		}
	} // end Chip8::op_skp_x()

	// Skip next instruction if key with the value of Vx is not pressed
	void Chip8::op_sknp_x(uint8_t x)
	{
#ifdef PRINT_OPCODES
		spdlog::get("logger")->debug("SKNP V{}", x);
#endif
		if (!this->key[this->V[x]]) {
			NEXT_INSTRUCTION;
		}
	} // end Chip8::op_sknp_x()

	// Set Vx = delay timer value
	void Chip8::op_ld_x_DT(uint8_t x)
	{
#ifdef PRINT_OPCODES
		spdlog::get("logger")->debug("LD V{}, {}", x, this->delay_timer);
#endif
		this->V[x] = this->delay_timer;
		NEXT_INSTRUCTION;
	} // end Chip8::op_ld_x_DT()

	// Wait for a key press, store the value of the key in Vx
	void Chip8::op_ld_x_K(uint8_t x)
	{
#ifdef PRINT_OPCODES
		spdlog::get("logger")->debug("LD V{}, K", x);
#endif
		for (int i = 0; i < 16; i++) {
			if (this->key[i]) {
				this->V[x] = i;
				NEXT_INSTRUCTION;
				break;
			}
		}
	} // end Chip8::op_ld_x_K()

	// Set delay timer = Vx
	void Chip8::op_ld_DT_x(uint8_t x)
	{
#ifdef PRINT_OPCODES
		spdlog::get("logger")->debug("LD DT, V{}", x);
#endif
		this->delay_timer = this->V[x];
		NEXT_INSTRUCTION;
	} // end Chip8::op_ld_DT_x()

	// Set sound timer = Vx
	void Chip8::op_ld_ST_x(uint8_t x)
	{
#ifdef PRINT_OPCODES
		spdlog::get("logger")->debug("LD ST, V{}", x);
#endif
		this->sound_timer = this->V[x];
		NEXT_INSTRUCTION;
	} // end Chip8::op_ld_ST_x()

	// Set I = I + Vx
	void Chip8::op_add_I_x(uint8_t x)
	{
#ifdef PRINT_OPCODES
		spdlog::get("logger")->debug("ADD I, V{}", x);
#endif
		this->I += this->V[x];
		NEXT_INSTRUCTION;
	} // end Chip8::op_add_I_x()

	// Set I = location of sprite for digit Vx
	void Chip8::op_ld_F_x(uint8_t x)
	{
#ifdef PRINT_OPCODES
		spdlog::get("logger")->debug("LD F, {}", x);
#endif
		this->I = FONTSET_BYTES_PER_CHAR * this->V[x];
		NEXT_INSTRUCTION;
	} // end Chip8::op_ld_F_x()

	// Store BCD representation of Vx in memory locations I, I+1, and I+2
	void Chip8::op_ld_B_x(uint8_t x)
	{
#ifdef PRINT_OPCODES
		spdlog::get("logger")->debug("LD B, V{}", x);
#endif
		this->memory[this->I] = (this->V[x] / 100);
		this->memory[this->I + 1] = (this->V[x] / 10) % 10;
		this->memory[this->I + 2] = (this->V[x] % 10);
		NEXT_INSTRUCTION;
	} // end Chip8::op_ld_B_x()

	// Store registers V0 through Vx in memory starting at location I
	void Chip8::op_ld_intoI_x(uint8_t x)
	{
#ifdef PRINT_OPCODES
		spdlog::get("logger")->debug("LD [I], V{}", x);
#endif
		std::copy(this->V.begin(),
			std::next(this->V.begin(), x + 1),
			std::next(this->memory.begin(), I));

		// advance I by the number of bytes stored
		this->I += x + 1;
		NEXT_INSTRUCTION;
	} // end Chip8::op_ld_intoI_x()

	// Read registers V0 through Vx from memory starting at location I
	void Chip8::op_ld_x_fromI(uint8_t x)
	{
#ifdef PRINT_OPCODES
		spdlog::get("logger")->debug("LD V{}, [I]", x);
#endif
		std::copy(std::next(this->memory.begin(), I),
			std::next(this->memory.begin(), I + x + 1),
			this->V.begin());

		// advance I by the number of bytes read
		this->I += x + 1;
		NEXT_INSTRUCTION;
	} // end Chip8::op_ld_x_fromI()

	// Load game data from given file and store in system memory
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
	} // end Chip8::load_game()

	// Set given key as "pressed"
	void Chip8::key_press(uint8_t key_val)
	{
		this->key[key_val] = 1;
	} // end Chip8::key_press()

	// Set given key as "released"
	void Chip8::key_release(uint8_t key_val)
	{
		this->key[key_val] = 0;
	} // end Chip8::key_release()

	// Perform current operation and update screen if necessary
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
			case 0xE0:
				this->op_cls();
				break;
			case 0xEE:
				this->op_ret();
				break;
			default:
				spdlog::get("logger")->error("Unknown opcode: {}", this->opcode);
				throw unknown_opcode_error();
			} // end switch (kk)
			break;
		case 0x1000:
			this->op_jp_nnn(nnn);
			break;
		case 0x2000:
			this->op_call_nnn(nnn);
			break;
		case 0x3000:
			this->op_se_x_kk(x, kk);
			break;
		case 0x4000:
			this->op_sne_x_kk(x, kk);
			break;
		case 0x5000:
			this->op_se_x_y(x, y);
			break;
		case 0x6000:
			this->op_ld_x_kk(x, kk);
			break;
		case 0x7000:
			this->op_add_x_kk(x, kk);
			break;
		case 0x8000:
			switch (n) {
			case 0x0:
				this->op_ld_x_y(x, y);
				break;
			case 0x1:
				this->op_or_x_y(x, y);
				break;
			case 0x2:
				this->op_and_x_y(x, y);
				break;
			case 0x3:
				this->op_xor_x_y(x, y);
				break;
			case 0x4:
				this->op_add_x_y(x, y);
				break;
			case 0x5:
				this->op_sub_x_y(x, y);
				break;
			case 0x6:
				this->op_shr_x(x);
				break;
			case 0x7:
				this->op_subn_x_y(x, y);
				break;
			case 0xE:
				this->op_shl_x(x);
				break;
			default:
				spdlog::get("logger")->error("Unknown opcode: {}", this->opcode);
				throw unknown_opcode_error();
			} // end switch (n)
			break;
		case 0x9000:
			this->op_sne_x_y(x, y);
			break;
		case 0xA000:
			this->op_ld_I_nnn(nnn);
			break;
		case 0xB000:
			this->op_jp_0_nnn(nnn);
			break;
		case 0xC000:
			this->op_rnd_x_kk(x, kk);
			break;
		case 0xD000:
			this->op_drw_x_y_n(this->V[x], this->V[y], n);
			break;
		case 0xE000:
			switch (kk) {
			case 0x9E:
				this->op_skp_x(x);
				break;
			case 0xA1:
				this->op_sknp_x(x);
				break;
			default:
				spdlog::get("logger")->error("Unknown opcode: {}", this->opcode);
				throw unknown_opcode_error();
			} // end switch (kk)
			NEXT_INSTRUCTION;
			break;
		case 0xF000:
			switch (kk) {
			case 0x07:
				this->op_ld_x_DT(x);
				break;
			case 0x0A:
				this->op_ld_x_K(x);
				break;
			case 0x15:
				this->op_ld_DT_x(x);
				break;
			case 0x18:
				this->op_ld_ST_x(x);
				break;
			case 0x1E:
				this->op_add_I_x(x);
				break;
			case 0x29:
				this->op_ld_F_x(x);
				break;
			case 0x33:
				this->op_ld_B_x(x);
				break;
			case 0x55:
				this->op_ld_intoI_x(x);
				break;
			case 0x65:
				this->op_ld_x_fromI(x);
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
	} // end Chip8::emulate_cycle()

	// Decrement system timers
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
	} // end Chip8::tick()
}