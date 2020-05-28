/**
 * Renderer.h
 * Copyright (c) 2020 Daniel Buckley
 */

#pragma once

#include <array>

#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <GL/GL.h>
#include <GL/GLU.h>

namespace c_plus_eight {
	class Renderer
	{
	private:
		int width = 0;
		int height = 0;
		SDL_Window* game_window = NULL;
		SDL_GLContext gl_ctx = NULL;

		bool start_window();

	public:
		Renderer(int w = 640, int h = 320): width(w), height(h) {
			if (!this->start_window()) {
				throw;
			}
		}

		void draw(std::array<uint8_t, 32 * 64>* g);
		void quit();
	};
}