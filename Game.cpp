#include "pong.h"

Player player1;
Player player2;
Wall walls[4];
Ball ball;
ScoreZone p1ScoreZone;
ScoreZone p2ScoreZone;
LTexture p1ScoreText;
LTexture p2ScoreText;
bool quit = false;
VictoryMessage currentVictory;
int ballR = 10;
bool scored = false;
Mix_Chunk* gHit = NULL;
Mix_Chunk* gVictory = NULL;

bool checkCollision(SDL_Rect a, SDL_Rect b) {
	//The sides of the rectangles
	int leftA, leftB;
	int rightA, rightB;
	int topA, topB;
	int bottomA, bottomB;

	//Calculate the sides of rect A
	leftA = a.x;
	rightA = a.x + a.w;
	topA = a.y;
	bottomA = a.y + a.h;

	//Calculate the sides of rect B
	leftB = b.x;
	rightB = b.x + b.w;
	topB = b.y;
	bottomB = b.y + b.h;

	//If any of the sides from A are outside of B
	if (bottomA <= topB)
	{
		return false;
	}

	if (topA >= bottomB)
	{
		return false;
	}

	if (rightA <= leftB)
	{
		return false;
	}

	if (leftA >= rightB)
	{
		return false;
	}

	//If none of the sides from A are outside B
	return true;
}

bool loadMedia() {
	bool success = true;

	gFont = TTF_OpenFont("assets/lucon.ttf", 28);
	if (gFont == NULL) {
		printf("Failed to load lucida console font! SDL_ttf Error: %s\n", TTF_GetError());
		success = false;
	}
	else
	{
		//make texture from font
		SDL_Color textColor = { 0xFF,0xFF,0xFF,0xFF };
		if (!player1.scoreTexture.loadFromRenderedText("0", textColor))
		{
			printf("Failed to render p1 score texture!\n");
			success = false;
		}
		if (!player2.scoreTexture.loadFromRenderedText("0", textColor))
		{
			printf("Failed to render p2 score texture!\n");
			success = false;
		}
	}

	gHit = Mix_LoadWAV("assets/tone1.wav");
	if (gHit == NULL) {
		printf("Failed to load hit sound effect! SDL_mixer Error %s\n", Mix_GetError());
		success = false;
	}
	gVictory = Mix_LoadWAV("assets/Rise01.wav");
	if (gVictory == NULL) {
		printf("Failed to load victory sound effect! SDL_mixer Error %s\n", Mix_GetError());
		success = false;
	}

	return success;
}


//SDL callback functions

Uint32 startBall(Uint32, void*) {
	ball.velocity.x = int((rand() % 3)-1)*350 + int(rand() % 100);
	ball.velocity.y = int((rand()%600)-300);
	//sanity clamping of horizontal ball velocity
	if (ball.velocity.x > 0 && ball.velocity.x < 100) { ball.velocity.x = 100; }
	if (ball.velocity.x < 0 && ball.velocity.x > -100) { ball.velocity.x = -100; }
	return 0;
}

Uint32 reinitGame(Uint32, void*) {
	player1.playerPaddle.coords.y = (SCREEN_HEIGHT / 2) - 5 * ballR;
	player2.playerPaddle.coords.y = (SCREEN_HEIGHT / 2) - 5 * ballR;
	ball = { {(SCREEN_WIDTH - ballR) / 2,(SCREEN_HEIGHT - ballR) / 2},ballR,{0,0},{(SCREEN_WIDTH - ballR) / 2,(SCREEN_HEIGHT - ballR) / 2,ballR,ballR} };
	currentVictory = CONTINUE_GAME;
	scored = false;
	SDL_AddTimer(3000, startBall, nullptr);
	return 0;
}


bool gameInit() {
	bool success = true;
	quit = false;
	ball = { {(SCREEN_WIDTH - ballR) / 2,(SCREEN_HEIGHT - ballR) / 2},ballR,{0,0},{(SCREEN_WIDTH - ballR) / 2,(SCREEN_HEIGHT - ballR) / 2,ballR,ballR} };
	walls[0] = { {-ballR,-ballR}, SCREEN_WIDTH + (2 * ballR), 2 * ballR, {-ballR,-ballR,SCREEN_WIDTH + (2 * ballR), 2 * ballR}};
	walls[1] = { {-ballR,SCREEN_HEIGHT-ballR}, SCREEN_WIDTH + (2 * ballR), 2 * ballR, {-ballR,SCREEN_HEIGHT - ballR, SCREEN_WIDTH + (2 * ballR), 2 * ballR} };
	walls[2] = { {-ballR, ballR}, 2 * ballR, SCREEN_HEIGHT - ballR, {-ballR, ballR, 2 * ballR, SCREEN_HEIGHT - ballR}};
	walls[3] = { {SCREEN_WIDTH-ballR, ballR}, 2 * ballR, SCREEN_HEIGHT - ballR, {SCREEN_WIDTH - ballR, ballR, 2 * ballR, SCREEN_HEIGHT - ballR } };
	p2ScoreZone.collider = { 0,0,(SCREEN_WIDTH / 8)-(2*ballR),SCREEN_HEIGHT};
	p1ScoreZone.collider = { ((7 * SCREEN_WIDTH) / 8) + (2 * ballR),0,SCREEN_WIDTH / 8,SCREEN_HEIGHT };
	player1 = { 0, SDLK_w, SDLK_s,{ {SCREEN_WIDTH / 8,(SCREEN_HEIGHT / 2) - (5 * ballR)}, 0, ballR, 10 * ballR,{SCREEN_WIDTH / 8,(SCREEN_HEIGHT / 2) - (5 * ballR),ballR, 10 * ballR}}, p1ScoreText };
	player2 = { 0, SDLK_UP, SDLK_DOWN,{ {7 * (SCREEN_WIDTH / 8),(SCREEN_HEIGHT / 2) - (5 * ballR)}, 0, ballR, 10 * ballR,{7*SCREEN_WIDTH / 8,(SCREEN_HEIGHT / 2) - (5 * ballR),ballR, 10 * ballR}}, p2ScoreText };
	currentVictory = CONTINUE_GAME;

	if (!loadMedia()) {
		printf("Loading assets failed!");
		success = false;
	}

	SDL_AddTimer(3000, startBall, nullptr);

	return success;
}

void gameClose() {

	p1ScoreText.free();
	p2ScoreText.free();
	gFont = NULL;

	Mix_FreeChunk(gHit);
	Mix_FreeChunk(gVictory);
	gHit = NULL;
	gVictory = NULL;
}

void handleGameEvents() {
	SDL_Event e;
	int paddleVel = 1000;
	//handle/poll events on queue
	while (SDL_PollEvent(&e) != 0) {
		//user requests quit
		if (e.type == SDL_QUIT) {
			quit = true;
		}
		else if (e.type == SDL_KEYDOWN) {
			if (e.key.keysym.sym == player1.upKey) {
				player1.playerPaddle.velocity = -paddleVel;
			}
			if (e.key.keysym.sym == player1.downKey) {
				player1.playerPaddle.velocity = paddleVel;

			}
			if (e.key.keysym.sym == player2.upKey) {
				player2.playerPaddle.velocity = -paddleVel;
			}
			if (e.key.keysym.sym == player2.downKey) {
				player2.playerPaddle.velocity = paddleVel;

			}
		}
	}
}

void gameUpdate(float deltaTime) {
	switch (currentVictory) {
		case P1_VICTORY:
			if (!scored)
			{
				player1.score += 1;
				player1.scoreTexture.loadFromRenderedText(std::to_string(player1.score), { 0xFF,0xFF,0xFF,0xFF });
				SDL_AddTimer(3000, reinitGame, nullptr);
				currentVictory = CONTINUE_GAME;
				scored = true;
				Mix_PlayChannel(-1, gVictory, 0);
			}
			break;
		case P2_VICTORY:
			if (!scored) {
				player2.score += 1;
				player2.scoreTexture.loadFromRenderedText(std::to_string(player2.score), { 0xFF,0xFF,0xFF,0xFF });
				SDL_AddTimer(3000, reinitGame, nullptr);
				currentVictory = CONTINUE_GAME;
				scored = true;
				Mix_PlayChannel(-1, gVictory, 0);
			}
			break;
		default:
			break;
	}

	player1.playerPaddle.update(deltaTime);
	player2.playerPaddle.update(deltaTime);
	ball.update(deltaTime);
}

void render() {
	//Clear screen
	SDL_SetRenderDrawColor(gRenderer, 0x33, 0x33, 0x33, 0xFF);
	SDL_RenderClear(gRenderer);

	for (int i = 0; i < 4; i++) {
		walls[i].render();
	}
	player1.playerPaddle.render();
	player2.playerPaddle.render();
	ball.render();
	player1.render_score();
	player2.render_score();

	//Update screen
	SDL_RenderPresent(gRenderer);

}

void collideRoutine() {
	if (ball.coords.y < walls[0].coords.y) {
		ball.coords.y = walls[0].coords.y + 2 * ball.r;
		ball.velocity.y = -ball.velocity.y;
		ball.collider.y = ball.coords.y;
	}
	else if (ball.coords.y > walls[1].coords.y) {
		ball.coords.y = walls[1].coords.y - 2 * ball.r;
		ball.velocity.y = -ball.velocity.y;
		ball.collider.y = ball.coords.y;
	}
	else if (ball.coords.x < walls[2].coords.x) {
		ball.coords.x = walls[2].coords.x + 2 * ball.r;
		ball.velocity.x = -ball.velocity.x;
		ball.collider.x = ball.coords.x;
	}
	else if (ball.coords.x > walls[3].coords.x) {
		ball.coords.x = walls[3].coords.x - 2 * ball.r;
		ball.velocity.x = -ball.velocity.x;
		ball.collider.x = ball.coords.x;
	}
	if (player1.playerPaddle.collider.y<walls[0].collider.y+walls[0].collider.h) {
		player1.playerPaddle.coords.y = walls[0].collider.y + walls[0].collider.h + 1;
		player1.playerPaddle.velocity = 0;
		player1.playerPaddle.collider.y = player1.playerPaddle.coords.y;
	}
	else if (player1.playerPaddle.collider.y + player1.playerPaddle.collider.h > walls[1].collider.y) {
		player1.playerPaddle.coords.y = walls[1].collider.y - (player1.playerPaddle.h + 1);
		player1.playerPaddle.velocity = 0;
		player1.playerPaddle.collider.y = player1.playerPaddle.coords.y;
	}
	if (player2.playerPaddle.collider.y<walls[0].collider.y+walls[0].collider.h) {
		player2.playerPaddle.coords.y = walls[0].collider.y + walls[0].collider.h + 1;
		player2.playerPaddle.velocity = 0;
		player2.playerPaddle.collider.y = player2.playerPaddle.coords.y;
	}
	if (player2.playerPaddle.collider.y + player2.playerPaddle.collider.h > walls[1].collider.y) {
		player2.playerPaddle.coords.y = walls[1].collider.y - (player2.playerPaddle.h + 1);
		player2.playerPaddle.velocity = 0;
		player2.playerPaddle.collider.y = player2.playerPaddle.coords.y;
	}
	if (checkCollision(ball.collider, player1.playerPaddle.collider)) {
		ball.coords.x = player1.playerPaddle.coords.x + player1.playerPaddle.w + 1;
		ball.collider.x = ball.coords.x;
		ball.velocity.x = -ball.velocity.x;
		Mix_PlayChannel(-1, gHit, 0);
	}
	else if (checkCollision(ball.collider, player2.playerPaddle.collider)) {
		ball.coords.x = player2.playerPaddle.coords.x-player2.playerPaddle.w;
		ball.collider.x = ball.coords.x;
		ball.velocity.x = -ball.velocity.x;
		Mix_PlayChannel(-1, gHit, 0);
	}
	else if (ball.collider.x>p1ScoreZone.collider.x || checkCollision(ball.collider, p1ScoreZone.collider)) {
		currentVictory = P1_VICTORY;
	}
	else if (ball.collider.x < p2ScoreZone.collider.x || checkCollision(ball.collider, p2ScoreZone.collider)) {
		currentVictory = P2_VICTORY;
	}
}

void runFrame(float deltaTime) {
	handleGameEvents();
	gameUpdate(deltaTime);
	//collision subroutine
	collideRoutine();
	render();
}


void game() {
	gameInit();

	//run the first frame with a dummy timestep so can calculate deltaTime
	float tempTimeStep = 16.6667;
	Uint64 start = SDL_GetPerformanceCounter();
	runFrame(tempTimeStep);
	Uint64 end = SDL_GetPerformanceCounter();
	float deltaTime = ((end - start) * 1000.0f) / (float)SDL_GetPerformanceFrequency();
	float timeStep = floor(MIN_FRAME_TIME - deltaTime);

	while (!quit) {
		start = SDL_GetPerformanceCounter();
		runFrame(timeStep);
		end = SDL_GetPerformanceCounter();
		deltaTime = ((end - start)*1000.0f) / (float)SDL_GetPerformanceFrequency();
		timeStep = floor(MIN_FRAME_TIME - deltaTime);
		SDL_Delay(timeStep);
	}

	gameClose();
}