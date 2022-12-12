#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <vector>
#include <stdio.h>

enum Textures {
	PLAYERTEXTURE,
	ENEMYTEXTURE,
	BACKGROUNDTEXTURE
};
//Game window size
static const int SCREEN_WIDTH = 1920;
static const int SCREEN_HEIGHT = 1080;
//base struct for inheritence
struct Object
{
public:
	int oWidth = 0, oHeight = 0;
	SDL_Texture* oTexture = NULL;
	double oAngle = 0;
	float oX = 0, oY = 0;
	float ovelX = 0, ovelY = 0;
	void Move();
	float speedMod = 1.0f;
private:
};
//player struct with eventhandler and modifiable speedvariable
struct Player : Object
{
public:
	Player(SDL_Texture* texture)
	{
		SDL_QueryTexture(texture, NULL, NULL, &oWidth, &oHeight);
		oTexture = texture;
		speedMod = 2.0f;
	}
	void HandleEvent(SDL_Event& e);
private:
};

struct Background : Object
{
public:
	Background(SDL_Texture* texture, float x, float y)
	{
		SDL_QueryTexture(texture, NULL, NULL, &oWidth, &oHeight);
		oTexture = texture;
		bgState = {x,y };
		tileSize = 64;
	}
	void BackgroundIllusion(SDL_FPoint offset);
	SDL_FPoint bgState;
	int tileSize;
private:
};

struct Enemy : Object
{
public:
	Enemy(SDL_Texture* texture, int x, int y)
	{
		SDL_QueryTexture(texture, NULL, NULL, &oWidth, &oHeight);
		oTexture = texture;
		oX = x;
		oY = y;
	}
	void ToPlayer(Player player);
private:
};

class Game
{
public:
	//Start the window and game
	bool Start();
	//gameloop 
	void GameLoop();
	//close and free all textures
	void Close();
	//spawns enemy
	void EnemySpawn(SDL_FPoint offset);
	//renders from variables in the different structs
	template <typename T> void Render(T sprite);
	//load a texture from a filepath
	void LoadTexture(std::string path);
	//Game window
	SDL_Window* gWindow = NULL;
	//Surface of game window
	SDL_Surface* gScreenSurface = NULL;
	//Displayed game window surface
	SDL_Surface* gCurrentSurface = NULL;
	//Renderer
	SDL_Renderer* gRenderer = NULL;
private:
	//timer using int64 to avoid resets
	Uint64 msTimer = 0;
	Uint64 spawnTimer = 0;
	//space modifier
	SDL_FPoint offset;
	//to destroy textures
	std::vector<SDL_Texture*> textures;
	std::vector<Enemy> enemies;
};
