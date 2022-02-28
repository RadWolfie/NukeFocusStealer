/* SPDX-License-Identifier: GPL-3.0-or-later */
/* SPDX-FileCopyrightText: Copyright contributors to the NukeFocusStealer project. */

#include <cstring>

#include <imgui.h>
#include <backends/imgui_impl_sdl.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_opengl3_loader.h>
#include <stdio.h>
#include <SDL.h>
#include <SimpleIni.h>
//#include <cmrc/cmrc.hpp>
//CMRC_DECLARE(rc);

#include "ui.hpp"

int main(int, char**)
{
	// Setup SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		return 1;
	}

	// Extend Begin
	// We don't have drag and drop text/file support. Disabled to prevent misleading end-user.
	SDL_EventState(SDL_DROPFILE, SDL_DISABLE);
	SDL_EventState(SDL_DROPTEXT, SDL_DISABLE);
	// Extend End

	// Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
	// GL ES 2.0 + GLSL 100
	const char* glsl_version = "#version 100";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
	// GL 3.2 Core + GLSL 150
	const char* glsl_version = "#version 150";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

	// Create window with graphics context // necessary?
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	// Common usage:
	Uint32 window_flags = SDL_WINDOW_ALLOW_HIGHDPI;

	window_flags |= SDL_WINDOW_OPENGL; // Can't use SDL_WINDOW_FULLSCREEN_DESKTOP, otherwise get unintended black background.
	SDL_Window* window = SDL_CreateWindow("NukeFocusStealer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 430, 360, window_flags);

	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, gl_context);
	SDL_GL_SetSwapInterval(1); // Enable vsync


	// Extend Begin
#ifdef CMRC
	auto rc_fs = cmrc::rc::get_filesystem();
	auto icon_file = rc_fs.open("icon.bmp");
	SDL_RWops* icon_mem = SDL_RWFromConstMem(icon_file.begin(), icon_file.size());
	SDL_Surface* icon = SDL_LoadBMP_RW(icon_mem, SDL_TRUE);
	SDL_SetWindowIcon(window, icon);
	SDL_FreeSurface(icon);
#endif
	// Extend End


	//FRAMEBUFFER

	// Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
	bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
	bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
	bool err = gladLoadGL() == 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
	bool err = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress) == 0; // glad2 recommend using the windowing library loader instead of the (optionally) bundled one.
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
	bool err = false;
	glbinding::Binding::initialize();
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
	bool err = false;
	glbinding::initialize([](const char* name) { return (glbinding::ProcAddress)SDL_GL_GetProcAddress(name); });
#else
	bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	const GLfloat clearColor[4] = {};

	// Extend Begin
	if (!UI::Initialize(window)) {
		return 0;
	}
	// Extend End

	// Setup Platform/Render backends
	ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Main loop
	bool is_terminated = false;
	while (!is_terminated) {
		SDL_Delay(10);
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL2_ProcessEvent(&event);

			switch (event.type) {

				case SDL_QUIT:
					is_terminated = true;
					break;
				case SDL_WINDOWEVENT:
					if (event.window.windowID == SDL_GetWindowID(window)) {
						switch (event.window.event) {
							case SDL_WINDOWEVENT_CLOSE:
								is_terminated = true;
								break;
							case SDL_WINDOWEVENT_ENTER:
								UI::WindowEnter();
								break;
							case SDL_WINDOWEVENT_LEAVE:
								UI::WindowLeave();
								break;
							case SDL_WINDOWEVENT_FOCUS_GAINED:
								UI::WindowFocusGain();
								break;
							case SDL_WINDOWEVENT_FOCUS_LOST:
								UI::WindowFocusLost();
								break;
						}
					}
					break;
				// language change
				case SDL_LOCALECHANGED:
					UI::UpdateLanguage();
					break;
				case SDL_USEREVENT:
					if (event.user.code = UI::language_event) {
						UI::UpdateLanguage();
						ImGui_ImplOpenGL3_DestroyFontsTexture();
						ImGui_ImplOpenGL3_CreateFontsTexture();
					}
					break;
			}
		}

		if (is_terminated) {
			break;
		}

		// Start Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		// Start drawing stuff here
		UI::DrawWidgets();

		// Rendering
		ImGui::Render();

		glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
		glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(window);

		is_terminated = UI::IsExit();
	}

	// Extend Clean up Begin
	UI::Shutdown();
	// Extend Clean up End

	// Clean up
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();

	ImGui::DestroyContext();
	SDL_GL_DeleteContext(gl_context);

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
