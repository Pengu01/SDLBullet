#include "Game.h"

bool Game::Start()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf(SDL_GetError());
		return false;
	}
	gWindow = SDL_CreateWindow("BulletGame", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (gWindow == NULL)
	{
		printf(SDL_GetError());
		return false;
	}
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
	if (gRenderer == NULL)
	{
		printf(SDL_GetError());
		return false;
	}
	gScreenSurface = SDL_GetWindowSurface(gWindow);
	return true;
}

void Game::GameLoop()
{
	SDL_Event e;
	bool quit = false;
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	Player playerObj(LoadTexture("src/Graphic/Player.png"), 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0, 0);
	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			playerObj.handleEvent(e);
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
		}
		//updatetime
		msTimer = SDL_GetTicks64();
		//move
		Move(playerObj);
		//Clear screen
		SDL_RenderClear(gRenderer);
		//renderer
		Render(playerObj);
		//Update screen
		SDL_RenderPresent(gRenderer);
	}
	Close();
}

void Game::Close()
{
	while (!sdlTextures.empty())
	{
		SDL_DestroyTexture(sdlTextures[0]);
		sdlTextures.erase(sdlTextures.begin());
	}
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	SDL_Quit();
}

template<typename T> void Game::Render(T sprite)
{
	//where to render
	SDL_Rect renderQuad = { sprite.oX, sprite.oY, sprite.oWidth, sprite.oHeight };
	//Render to screen
	SDL_RenderCopyEx(gRenderer, sprite.oTexture, NULL, &renderQuad, sprite.oAngle, NULL, SDL_FLIP_NONE);
}

template<typename T> void Game::Move(T sprite)
{
	//x axis movement
	sprite.oX += sprite.ovelX;
	//If the Player went too far to the left set it to lowest left
	if (sprite.oX < 0)  sprite.oX = 0;
	//If the Player went too far to the right set it to lowest right
	if (sprite.oX + sprite.oWidth > SCREEN_WIDTH) sprite.oX = SCREEN_WIDTH - sprite.oWidth;
	//y axis movement
	sprite.oY += sprite.ovelY;
	//If the Player went too far up set it to lowest up
	if (sprite.oY < 0) sprite.oY = 0;
	//if the player moves too far down set it to lowest down
	if (sprite.oY + sprite.oHeight > SCREEN_HEIGHT) sprite.oY = SCREEN_HEIGHT - sprite.oHeight;
	if(sprite.oX!=360.0f)printf("before%f", sprite.oX);
}

SDL_Texture* Game::LoadTexture(std::string path)
{
	//texture to return
	SDL_Texture* newTexture = NULL;
	//Load image to texture
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		IMG_GetError();
		return newTexture;
	}
	//Create texture from loaded surface
	newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
	if (newTexture == NULL)
	{
		printf(SDL_GetError());
		return newTexture;
	}
	//free the old surface
	SDL_FreeSurface(loadedSurface);
	sdlTextures.push_back(newTexture);
	return newTexture;
}
void Player::handleEvent(SDL_Event& e)
{
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: ovelY -= 1.0f; break;
		case SDLK_DOWN: ovelY += 1.0f; break;
		case SDLK_LEFT: ovelX -= 1.0f; break;
		case SDLK_RIGHT: ovelX += 1.0f; break;
		}
	}
	else if (e.type == SDL_KEYUP && e.key.repeat == 0)
	{
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: ovelY += 1.0f; break;
		case SDLK_DOWN: ovelY -= 1.0f; break;
		case SDLK_LEFT: ovelX += 1.0f; break;
		case SDLK_RIGHT: ovelX -= 1.0f; break;
		}
	}
}
