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
	LoadTexture("src/Graphic/Player.png");
	LoadTexture("src/Graphic/Shooter.png");
	LoadTexture("src/Graphic/Grass.png");
	return true;
}

void Game::GameLoop()
{
	SDL_Event e;
	bool quit = false;
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	Player playerObj(textures[PLAYERTEXTURE]);
	float xOff = SCREEN_WIDTH / 2;
	float yOff = SCREEN_HEIGHT / 2;
	Background background(textures[BACKGROUNDTEXTURE],xOff,yOff);
	spawnTimer = SDL_GetTicks64();
	while (!quit)
	{
		//spawn enemy every 200 ms
		if (spawnTimer + 200 < SDL_GetTicks64())
		{
			EnemySpawn(offset);
			spawnTimer = SDL_GetTicks64();
		}
		//updatetime
		msTimer = SDL_GetTicks64();
		while (SDL_PollEvent(&e) != 0)
		{
			playerObj.HandleEvent(e);
			//User requests quit
			if (e.type == SDL_QUIT || e.key.keysym.sym == SDLK_ESCAPE)
			{
				quit = true;
			}
			//kills enemy that player clicks on, will change to bullet creation
			if (e.type == SDL_MOUSEBUTTONDOWN) {
				int x, y;
				SDL_GetMouseState(&x, &y);
				x += offset.x;
				y += offset.y;
				for (int i = 0; i < enemies.size(); i++)
				{
					if (x > enemies[i].oX - enemies[i].oWidth / 2 && x < enemies[i].oX + enemies[i].oWidth / 2)
						if (y > enemies[i].oY - enemies[i].oHeight / 2 && y < enemies[i].oY + enemies[i].oHeight / 2)
						{
							enemies.erase(enemies.begin() + i);
							i--;
						}
				}
			}
		}
		//move
		playerObj.Move();
		//fixes movement for all enemies
		for (int i = 0; i < enemies.size(); i++)
		{
			enemies[i].ToPlayer(playerObj);
			enemies[i].Move();
		}
		//update offset
		offset = {playerObj.oX-xOff,playerObj.oY-yOff};
		//backgroundmovement
		background.BackgroundIllusion(offset);
		//Clear screen
		SDL_RenderClear(gRenderer);
		//renders
		Render(background);
		Render(playerObj);
		for (int i = 0; i < enemies.size(); i++)
		{
			Render(enemies[i]);
		}
		//Update screen
		SDL_RenderPresent(gRenderer);
		//limits fps to 60
		if(((SDL_GetTicks64() - msTimer) < 16.666f)) SDL_Delay(16.666f - (SDL_GetTicks64() - msTimer));
	}
	Close();
}

void Game::Close()
{
	while (!textures.empty())
	{
		SDL_DestroyTexture(textures[0]);
		textures.erase(textures.begin());
	}
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	SDL_Quit();
}

template<typename T> void Game::Render(T sprite)
{
	//where to render
	SDL_Rect renderQuad = { sprite.oX-offset.x-sprite.oWidth/2, sprite.oY-offset.y-sprite.oHeight/2, sprite.oWidth, sprite.oHeight };
	//Render to screen
	SDL_RenderCopyEx(gRenderer, sprite.oTexture, NULL, &renderQuad, sprite.oAngle, NULL, SDL_FLIP_NONE);
}

void Game::LoadTexture(std::string path)
{
	//texture to return
	SDL_Texture* newTexture = NULL;
	//Load image to texture
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		IMG_GetError();
		return;
	}
	//Create texture from loaded surface
	newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
	if (newTexture == NULL)
	{
		printf(SDL_GetError());
		return;
	}
	//free the old surface
	SDL_FreeSurface(loadedSurface);
	textures.push_back(newTexture);
}

void Game::EnemySpawn(SDL_FPoint offset)
{
	int x = offset.x;
	int y = offset.y;
	int lenghtAround = rand() % (SCREEN_HEIGHT * 2 + SCREEN_WIDTH * 2);
	if (lenghtAround < SCREEN_WIDTH)
	{
		x += lenghtAround;
	}
	else if (lenghtAround < SCREEN_WIDTH + SCREEN_HEIGHT)
	{
		x += SCREEN_WIDTH;
		y += lenghtAround - SCREEN_WIDTH;
	}
	else if (lenghtAround < SCREEN_WIDTH * 2 + SCREEN_HEIGHT)
	{
		x += lenghtAround - SCREEN_HEIGHT - SCREEN_WIDTH;
		y += SCREEN_HEIGHT;
	}
	else
	{
		y += lenghtAround - SCREEN_HEIGHT - SCREEN_WIDTH*2;
	}
	Enemy enemy(textures[ENEMYTEXTURE], x, y);
	enemies.push_back(enemy);
}

void Player::HandleEvent(SDL_Event& e)
{
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: ovelY --; break;
		case SDLK_DOWN: ovelY++; break;
		case SDLK_LEFT: ovelX--; break;
		case SDLK_RIGHT: ovelX++; break;
		}
	}
	else if (e.type == SDL_KEYUP && e.key.repeat == 0)
	{
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: ovelY++; break;
		case SDLK_DOWN: ovelY--; break;
		case SDLK_LEFT: ovelX++; break;
		case SDLK_RIGHT: ovelX--; break;
		}
	}
}

void Object::Move()
{
	float tempX = ovelX;
	float tempY = ovelY;
	int diff = sqrt(pow(tempX, 2) + pow(tempY, 2));
	if (diff != 0)
	{
		tempX /= diff;
		tempY /= diff;
	}
	oX += tempX *speedMod;
	oY += tempY *speedMod;
}

void Background::BackgroundIllusion(SDL_FPoint offset)
{
	if (offset.x > bgState.x+tileSize - SCREEN_WIDTH)
	{
		bgState.x += tileSize;
		oX += tileSize;
	}
	if (offset.x < bgState.x - tileSize - SCREEN_WIDTH)
	{
		bgState.x -= tileSize;
		oX -= tileSize;
	}
	if (offset.y > bgState.y + tileSize - SCREEN_HEIGHT)
	{
		bgState.y += tileSize;
		oY += tileSize;
	}
	if (offset.y < bgState.y - tileSize - SCREEN_HEIGHT)
	{
		bgState.y -= tileSize;
		oY -= tileSize;
	}
}

void Enemy::ToPlayer(Player player)
{
	ovelX = player.oX-oX;
	ovelY = player.oY-oY;
}
