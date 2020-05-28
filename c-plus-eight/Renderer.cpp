/**
 * Renderer.cpp
 * Copyright (c) 2020 Daniel Buckley
 */

#include <iostream>
#include "Renderer.h"
#include "spdlog/spdlog.h"

namespace c_plus_eight {
	bool Renderer::start_window()
	{
		// initialize SDL video
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			spdlog::get("logger")->error("Could not initialize SDL. SDL Error: {}", SDL_GetError());
			return false;
		}

		spdlog::get("logger")->info("SDL initialized successfully.");

		// create game window
		this->game_window = SDL_CreateWindow("C+8",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			this->width,
			this->height,
			SDL_WINDOW_OPENGL);

		if (this->game_window == NULL) {
			spdlog::get("logger")->error("Could not create window. SDL Error: {}", SDL_GetError());
			return false;
		}

		spdlog::get("logger")->info("Game window created successfully.");

		// set OpenGL version to 3.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		// create context for OpenGL using SDL
		this->gl_ctx = SDL_GL_CreateContext(this->game_window);
		if (gl_ctx == NULL) {
			spdlog::get("logger")->error("Could not create OpenGL context. SDL Error: {}", SDL_GetError());
			return false;
		}

		// initialize GLEW
		glewExperimental = true;
		auto glew_err = glewInit();
		if (glew_err != GLEW_OK) {
			spdlog::get("logger")->error("Could not initialize GLEW. {}", glewGetErrorString(glew_err));
			return false;
		}

		// enable VSync
		if (SDL_GL_SetSwapInterval(1) < 0) {
			spdlog::get("logger")->error("Could not enable VSync. SDL Error: {}", SDL_GetError());
			return false;
		}

		// setup OpenGL
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport(0.0f, 0.0f, this->width, this->height);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glPixelZoom(10.0f, 10.0f);

		return true;
	}

	void Renderer::draw(std::array<uint8_t, 32 * 64>* g)
	{
		// clear framebuffer
		glClear(GL_COLOR_BUFFER_BIT);

		// update screen
		glDrawPixels(64, 32, GL_LUMINANCE, GL_UNSIGNED_BYTE, (void *)g);

		SDL_GL_SwapWindow(this->game_window);
	}

	void Renderer::quit()
	{
		SDL_DestroyWindow(this->game_window);
		this->game_window = NULL;
		SDL_Quit();
	}
}