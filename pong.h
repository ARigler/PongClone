#pragma once
#ifndef PONG_HEADER
#define PONG_HEADER
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <vector>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const float MIN_FRAME_TIME = 16.666f;
const float SCALING_FACTOR = 0.001f;
extern SDL_Window* gWindow;
extern SDL_Renderer* gRenderer;
extern TTF_Font* gFont;
extern bool quit;

bool checkCollision(SDL_Rect a, SDL_Rect b);
bool loadMedia();
bool gameInit();
void gameClose();
void handleGameEvents();
void gameUpdate(float deltaTime);
void render();
void runFrame(float deltaTime);
bool engineInit();
void engineClose();
void game();


//Texture wrapper class
class LTexture
{
public:
	//Initializes variables
	LTexture();
	//Deallocates memory
	~LTexture();
	//Loads image at specified path
	bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
	//Deallocates texture
	void free();
	//Set color modulation
	void setColor(Uint8 red, Uint8 green, Uint8 blue);
	//Set Blending
	void setBlendMode(SDL_BlendMode blending);
	//Set Alpha Modulation
	void setAlpha(Uint8 alpha);
	//Renders texture at given point
	void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
	//Gets image dimensions
	int getWidth();
	int getHeight();

private:
	//The actual hardware texture
	SDL_Texture* mTexture;
	//Image dimensions
	int mWidth;
	int mHeight;
};

struct Pos {
	int x;
	int y;
};

struct Vel {
	int x;
	int y;
};

struct Paddle {
	Pos coords;
	int velocity;
	int w;
	int h;
	SDL_Rect collider;
	void update(float deltaTime);
	void render();
};

struct Ball {
	Pos coords;
	int r;
	Vel velocity;
	SDL_Rect collider;
	void update(float deltaTime);
	void render();
};

struct Wall {
	Pos coords;
	int w;
	int h;
	SDL_Rect collider;
	void render();
};

struct Player {
	int score;
	SDL_KeyCode upKey;
	SDL_KeyCode downKey;
	Paddle playerPaddle;
	LTexture scoreTexture;
	void render_score();
};

struct ScoreZone {
	SDL_Rect collider;
};

enum VictoryMessage {
	P1_VICTORY,
	P2_VICTORY,
	CONTINUE_GAME
};


#endif