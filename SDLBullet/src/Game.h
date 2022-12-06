#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <vector>
#include <stdio.h>

struct Object
{
public:
	Object(SDL_Texture* texture, double angle, float x, float y, float velX, float velY)
	{
		SDL_QueryTexture(texture, NULL, NULL, &oWidth, &oHeight);
		oTexture = texture;
		oAngle = angle;
		oX = x;
		oY = y;
		ovelX = velX;
		ovelY = velY;
	}
	int oWidth, oHeight;
	SDL_Texture* oTexture;
	double oAngle;
	float oX, oY;
	float ovelX, ovelY;
private:
};

struct Player : Object
{
public:
	using Object::Object;
	void handleEvent(SDL_Event& e);
private:
};

class Game
{
public:
	//Start the window and game
	bool Start();
	//gameloop
	void GameLoop();
	//close and free
	void Close();
	//renders
	template <typename T> void Render(T sprite);
	//load a texture
	SDL_Texture* LoadTexture(std::string path);
	//Game window size
	static const int SCREEN_WIDTH = 720;
	static const int SCREEN_HEIGHT = 480;
	//Game window
	SDL_Window* gWindow = NULL;
	//Surface of game window
	SDL_Surface* gScreenSurface = NULL;
	//Displayed game window surface
	SDL_Surface* gCurrentSurface = NULL;
	//Renderer
	SDL_Renderer* gRenderer = NULL;
	//movement
	template <typename T> void Move(T sprite);
private:
	Uint64 msTimer = 0;
	std::vector<SDL_Texture*> sdlTextures;
};
