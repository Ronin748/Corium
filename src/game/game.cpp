//
// Created by Roninkoi on 18.12.2015.
//

#include "game.h"
#include "util/timeUtil.h"
#include "util/cfgParser.h"

Game::Game()
{
	text_buffer.resize(15);
}

void Game::main()
{
	auto time = getTime();
	auto fps_time = getTime();

	while (running) {
		++frame_ticks;

		if (getTime() - fps_time >= 1000) {
			fps_time = getTime();
			fps = frame_ticks;

			std::string fpsString = "FPS: " + toString(fps) + ", rt: "
									+ toString((renderTime) / (float) frame_ticks) + " ms, draws: " +
									toString(renderer.drawsPerCycle) + ", batches: " +
									toString(renderer.batchesPerCycle) + ", tt: " +
									toString((tickTime) / (float) game_ticks) +
									" ms";
			print(fpsString + "\n");
			print("lights: " + toString(renderer.max_lights) + "\n");

			print("Player pos: " + toString(thisPlayer->phys.pos.x) + ", " + toString(thisPlayer->phys.pos.y) +
				  ", " + toString(thisPlayer->phys.pos.z) + "\n");

			renderTime = 0;
			tickTime = 0;

			frame_ticks = 0;
			game_ticks = 0;
		}

		auto renderTimeStart = getTime();

		// GAME RENDERING
		gameRender();

		renderTime += getTime() - renderTimeStart;

		ticking = getTime() - time >= 15;
		if (ticking) {
			time = getTime();

			auto tickTimeStart = getTime();

			// GAME LOGIC
			tick();

			tickTime += getTime() - tickTimeStart;
		}

		//UPDATE WINDOW
		window.update(&running);
	}
	gameQuit();
}

int Game::getTime()
{
	long long int et = std::chrono::duration_cast<std::chrono::milliseconds>
			(timer.now().time_since_epoch() - start_time).count();

	int returns = (int) et;

	return returns;
}

void Game::refresh()
{
	renderer.flushBatchFBO();
	window.update(&running); // update window to keep it from freezing
}

void Game::init()
{
	printf("CORIUM %s RONINKOI %s\n", CORIUM_VERSION, CORIUM_YEAR);

	thisPlayer = map.getPlayer();
	thisCamera = map.getCamera();

	loadGameCfg();

	if (fileoutput) {
		outputfile = freopen("output.log", "current_w", stdout);
	}

	print("===========================================================================\n");
	print("---------------------- CORIUM ENGINE " + toString(CORIUM_VERSION) + " BY RONINKOI ----------------------\n");
	print("--------------------------- " + toString(currentDateTime()) + " ---------------------------\n");
	print("===========================================================================\n\n");

	int glfw_err = glfwInit();
	if (!glfw_err) {
		printf("Failed to init GLFW %i%s", glfw_err, "\n");
	}

	window.createWindow();

	glewExperimental = GL_TRUE;
	int glew_err = glewInit();
	if (glew_err != GLEW_OK) {
		printf("Failed to init GLEW %i%s", glew_err, "\n");
	}

	print("max vertices: " + toString(GL_MAX_ELEMENTS_VERTICES) + "\n");
	print("max indices: " + toString(GL_MAX_ELEMENTS_INDICES) + "\n");
	print("batch size: " + toString(BATCH_SIZE) + "\n");
	print("max texture size: " + toString(GL_MAX_RECTANGLE_TEXTURE_SIZE) + "\n");

	print("OpenGL version: " + toString(glGetString(GL_VERSION)) + "\n");
	print("GL_RENDERER: " + toString(glGetString(GL_RENDERER)) + "\n");

	audio.initAudio();

	renderer.init();

	screen.loadScreen();

	audio.loadAudio();

	if (startFullScr)
		fullScr();
}

void Game::start()
{
	start_time = timer.now().time_since_epoch();

	running = true;

	if (window.swapInterval == 0) {
		print("Vsync disabled!\n");
	} else {
		print("Vsync enabled!\n");
	}

	map.start(&renderer, &audio, &running);

	window.update(&running); // correct window dimensions

	// update main renderer so screen renderer can initialize itself
	rendererUpdate();

	GLFWimage icons[1];
	icons[0].pixels = loadGLTex("gfx/misc/Corium_logo.png", &icons[0].width, &icons[0].height);
	glfwSetWindowIcon(window.window, 1, icons);

	// loading screen
	loadingScreen();

	map.load(&renderer);

	while (getTime() < 2000) {
		window.update(NULL);
	}

	if (music) audio.playMusic();

	main();
}

void Game::print(std::string s)
{
	printf(&s[0]);

	for (int i = text_buffer.size() - 1; i > 0; --i)
		text_buffer[i] = text_buffer[i - 1];

	text_buffer[0] = s;
}

void Game::loadGameCfg()
{
	loadSettings("settings.cfg", this);
}

void Game::fullScr()
{
	window.fullScr();

	input.winFocus = false;
	input.focused = true;
	glfwSetInputMode(window.window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	glfwSetInputMode(window.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Game::gameQuit()
{
	map.quit();

	window.destroyWindow();

	glfwTerminate();

	if (fileoutput) {
		fclose(outputfile);
	}
}
