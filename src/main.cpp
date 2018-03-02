#include <iostream>

#include "SDL.h" 

#include "../inc/Vector.hpp"
#include "../inc/Plane.hpp"
#include "../inc/Camera.hpp"
#include "../inc/World.hpp"
#include "../inc/main.hpp"

SDL_Window* window;
SDL_Renderer* renderer;

int 
SDLInit(
	bool &retflag)
{
	retflag = true;
	if ((SDL_Init(SDL_INIT_VIDEO) == -1))
	{
		std::cout << "Could not initialize SDL:" << SDL_GetError() << std::endl;
		return -1;
	}

	std::cout << "SDL initalized." << SDL_GetError() << std::endl;

	if (SDL_CreateWindowAndRenderer(
		static_cast<int>(DEFAULT_CANVAS_HEIGHT),
		static_cast<int>(DEFAULT_CANVAS_WIDTH),
		0,
		&window,
		&renderer) == -1)
	{
		std::cout << "Could not create window and renderer" << SDL_GetError() << std::endl;
		return -1;
	}

	retflag = false;
	return {};
}

int 
CreateRenderer(
	bool &retflag)
{
	retflag = true;
	renderer = SDL_CreateRenderer(window, -1, 0);

	if (renderer == nullptr)
	{
		std::cout << "Failed to create renderer : " << SDL_GetError();
		return 1;
	}

	retflag = false;
	return {};
}

void 
DrawPixel(
	const rt::Color& aColor, 
	const int& aX, 
	const int& aY)
{
	SDL_SetRenderDrawColor(renderer, aColor.Red, aColor.Green, aColor.Blue, aColor.Alpha);
	SDL_RenderDrawPoint(renderer, aX, aY);
}

int main(int argc, char* argv[])
{
	rt::Camera camera;
	rt::World world;
	rt::Plane plane1(rt::ColorName::GREY);
	rt::Plane plane2(rt::Vector<float>(-5.0f, 0.0f, 0.0f), rt::Vector<float>(150.0f, 0.0f, 0.0f), rt::ColorName::GREY);
	rt::Plane plane3(rt::Vector<float>(5.0f, 0.0f, 0.0f), rt::Vector<float>(-150.0f, 0.0f, 0.0f), rt::ColorName::GREY);
	rt::Plane plane4(rt::Vector<float>(0.0f, -1.0f, 0.0f), rt::Vector<float>(0.0f, 300.0f, 0.0f), rt::ColorName::GREY);
	world.myShapes.push_back(plane1);
	world.myShapes.push_back(plane2);
	world.myShapes.push_back(plane3);
	world.myShapes.push_back(plane4);


	bool retflag;
	int retval = SDLInit(retflag);
	if (retflag) return retval;

	bool loop = true;
	while (loop)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				loop = false;
			else if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_UP:
					camera.MoveUp(5);
				default:
					break;
				}
			}
		}

		const rt::Canvas& canvas = camera.GetCanvas();
		float distance = 0.0f;

		int width = 0;
		int height = static_cast<int>(canvas.GetHeight()) - 1;

		for (const auto& pixel : canvas.CanvasPixels)
		{
			float closestDistance = FLT_MAX;

			bool hit = false;
			for (auto& shape : world.myShapes)
			{
				if (shape.IsIntersecting(rt::Vector<float>(pixel.X, pixel.Y, pixel.Z), camera.GetPosition(), distance))
				{
					if (distance >= closestDistance)
					{
						continue;
					}

					closestDistance = distance;

					auto color = shape.GetColor() + (distance/8);
					DrawPixel(color, width, height);

					hit = true;
				}
			}

			if (!hit)
			{
				DrawPixel(rt::Color::myPalette[rt::SKY], width, height);
			}

			width++;
			if (width == canvas.GetWidth())
			{
				width = 0;
				height--;
			}
		}
	
		// TODO: Clearing
		SDL_RenderPresent(renderer);

		// Add a 16msec delay to make our game run at ~60 fps
		SDL_Delay(16);
	}

	/* Shutdown all subsystems */
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}