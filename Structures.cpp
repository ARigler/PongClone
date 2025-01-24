#include "pong.h"

void Paddle::update(float deltaTime) {
	coords.y += velocity*deltaTime * SCALING_FACTOR;
	collider.y = coords.y;
	velocity = 0;
}

void Paddle::render() {
	SDL_Rect fillRect = { coords.x,coords.y,w,h };
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderFillRect(gRenderer, &fillRect);
}

void Ball::update(float deltaTime){
	coords.x += velocity.x * deltaTime * SCALING_FACTOR;
	coords.y += velocity.y * deltaTime * SCALING_FACTOR;
	collider.x = coords.x;
	collider.y = coords.y;
}

void Ball::render() {
	SDL_Rect fillRect = { coords.x,coords.y,r,r };
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderFillRect(gRenderer, &fillRect);
}

void Wall::render() {
	SDL_Rect fillRect = { coords.x,coords.y,w,h };
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderFillRect(gRenderer, &fillRect);
}

void Player::render_score() {
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	scoreTexture.render(playerPaddle.coords.x-5, SCREEN_HEIGHT/8);
}

LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor) {
	//get rid of pre-existing texture
	free();

	SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
	if (textSurface == NULL)
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}
	else {
		//create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (mTexture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Free old surface
		SDL_FreeSurface(textSurface);
	}

	//return success
	return mTexture != NULL;
}

void LTexture::free()
{
	//Free texture if it exists
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
	//Modulate texture
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending) {
	SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha) {
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}