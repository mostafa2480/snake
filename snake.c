#include <raylib.h>

#define WIDTH 800
#define HEIGHT 800
#define SQUARE_SIZE 40
#define FPS 10
#define TOTAL_SQUARES (WIDTH * HEIGHT) / (SQUARE_SIZE * SQUARE_SIZE)
#define GAME_STATE_RUNNING 0
#define GAME_STATE_PAUSED 1
#define GAME_STATE_WON 2
#define GAME_STATE_LOST 3

Vector2 snake[TOTAL_SQUARES];
int length;
int head_index;
int tail_index;
int xdir;
int ydir;
Vector2 apple;
int apple_on_snake;
int game_state;

void respawn_apple();
void initialize_game();
void get_snake_dir_from_key_input();
void move_snake();
int has_crashed();
int has_won();
void draw_grid();
void draw_snake();
void draw_game();
void update_game_state();

int main()
{
	InitWindow(WIDTH, HEIGHT, "Snake");
	SetExitKey(KEY_NULL);
	SetTargetFPS(FPS);
	initialize_game();

	while (!WindowShouldClose())
	{
		update_game_state();
		if (game_state == GAME_STATE_RUNNING)
		{
			get_snake_dir_from_key_input();
			move_snake();
		}
		BeginDrawing();
		draw_game();
		EndDrawing();
	}

	return 0;
}

void respawn_apple()
{
	apple_on_snake = 1;
	while (apple_on_snake)
	{
		apple.x = SQUARE_SIZE * (GetRandomValue(0, WIDTH / SQUARE_SIZE - 1)); 
		apple.y = SQUARE_SIZE * (GetRandomValue(0, HEIGHT / SQUARE_SIZE - 1));
		apple_on_snake = 0;
		for (int i = tail_index; i <= head_index; i++)
		{
			if (apple.x == snake[i].x && apple.y == snake[i].y)
			{
				apple_on_snake = 1;
				break;
			}
		}
	}
}

void initialize_game()
{
	length = 1;
	head_index = 0;
	tail_index = 0;
	snake[0].x = 0;
	snake[0].y = 0;
	xdir = 0;
	ydir = 0;	
	game_state = GAME_STATE_RUNNING;
	respawn_apple();
}

void get_snake_dir_from_key_input()
{
	if (IsKeyPressed(KEY_RIGHT) && xdir != -1)
	{
		xdir = 1;
		ydir = 0;
	}
	else if (IsKeyPressed(KEY_LEFT) && xdir != 1)
	{
		xdir = -1;
		ydir = 0;
	}
	else if (IsKeyPressed(KEY_DOWN) && ydir != -1)
	{
		ydir = 1;
		xdir = 0;
	}
	else if (IsKeyPressed(KEY_UP) && ydir != 1)
	{
		ydir = -1;
		xdir = 0;
	}
}

void move_snake()
{
	if (head_index == TOTAL_SQUARES - 1) 
	{
		for (int i = 0; i < length; i++)
		{
			snake[i].x = snake[tail_index + i].x;
			snake[i].y = snake[tail_index + i].y;
		}
		tail_index = 0;
		head_index = length - 1;
	}

	if (snake[head_index].x == apple.x && snake[head_index].y == apple.y)
	{
		respawn_apple();

		length++;
		head_index++;
		snake[head_index].x = snake[head_index - 1].x + SQUARE_SIZE * xdir;
		snake[head_index].y = snake[head_index - 1].y + SQUARE_SIZE * ydir;	
	}
	else
	{
		head_index++;
		tail_index++;
		snake[head_index].x = snake[head_index - 1].x + SQUARE_SIZE * xdir;
		snake[head_index].y = snake[head_index - 1].y + SQUARE_SIZE * ydir;	
	}
}

int has_crashed()
{
	if (snake[head_index].x < 0 || snake[head_index].x > WIDTH - SQUARE_SIZE || snake[head_index].y < 0 || snake[head_index].y > HEIGHT - SQUARE_SIZE) return 1;
	for (int i = tail_index; i < head_index; i++)
	{
		if (snake[head_index].x == snake[i].x && snake[head_index].y == snake[i].y) return 1;
	}
	return 0;
}

int has_won()
{
	return length == (WIDTH * HEIGHT) / (SQUARE_SIZE * SQUARE_SIZE); 
}

void update_game_state()
{
	if (has_won())
	{
		game_state = GAME_STATE_WON;
		return;
	}
	if (has_crashed())
	{
		game_state = GAME_STATE_LOST;
		return;
	}
	if (IsKeyPressed(KEY_ESCAPE))
	{
		if (game_state == GAME_STATE_RUNNING) game_state = GAME_STATE_PAUSED;
		else if (game_state == GAME_STATE_PAUSED) game_state = GAME_STATE_RUNNING;
	}
}

void draw_grid()
{
	for (int i = SQUARE_SIZE; i < WIDTH; i += SQUARE_SIZE)
	{
		DrawLine(i, 0, i, HEIGHT, DARKGRAY);
		DrawLine(0, i, WIDTH, i, DARKGRAY);
	}
}

void draw_snake()
{
	for (int i = tail_index, j = 1; i <= head_index; i++, j++)
	{
		Color snake_color = ColorLerp(DARKGREEN, YELLOW, (float) j / length);
		DrawRectangle(snake[i].x, snake[i].y, SQUARE_SIZE, SQUARE_SIZE, snake_color);
	}
}

void draw_game()
{
	ClearBackground(BLACK);
	draw_snake();
	draw_grid();
	DrawRectangle(apple.x, apple.y, SQUARE_SIZE, SQUARE_SIZE, RED);

	if (game_state == GAME_STATE_PAUSED)
	{
		DrawText("PAUSED", WIDTH / 2 - MeasureText("PAUSED", SQUARE_SIZE) / 2, SQUARE_SIZE * 5, SQUARE_SIZE, WHITE);

		Rectangle button_rectangle = {WIDTH / 2 - SQUARE_SIZE * 3, SQUARE_SIZE * 6, SQUARE_SIZE * 6, SQUARE_SIZE * 2};
		Color button_color = MAROON;

		if (CheckCollisionPointRec(GetMousePosition(), button_rectangle))
		{
			if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
			{
				button_color = ORANGE;
			}
			else
			{
				button_color = RED;
			}
			if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) CloseWindow();
		}
		DrawRectangleRec(button_rectangle, button_color);
		DrawText("EXIT", WIDTH / 2 - MeasureText("EXIT", SQUARE_SIZE) / 2, button_rectangle.y + button_rectangle.height / 2 - SQUARE_SIZE / 2, SQUARE_SIZE, LIGHTGRAY);
	}
	if (game_state == GAME_STATE_WON)
	{
		DrawRectangle(0, 0, WIDTH, HEIGHT, Fade(BLACK, 0.7f));
		DrawText("YOU WON", WIDTH / 2 - MeasureText("YOU WON", SQUARE_SIZE) / 2, HEIGHT / 2, SQUARE_SIZE, GREEN);
		DrawText("PRESS R TO RESTART", WIDTH / 2 - MeasureText("PRESS R TO RESTART", SQUARE_SIZE) / 2, HEIGHT / 2 + SQUARE_SIZE, SQUARE_SIZE, GREEN);
		if (IsKeyPressed(KEY_R))
		{
			initialize_game();
		}
	}
	if (game_state == GAME_STATE_LOST)
	{
		DrawRectangle(0, 0, WIDTH, HEIGHT, Fade(BLACK, 0.7f));
		DrawText("YOU LOST", WIDTH / 2 - MeasureText("YOU LOST", SQUARE_SIZE) / 2, HEIGHT / 2, SQUARE_SIZE, RED);
		DrawText("PRESS R TO RESTART", WIDTH / 2 - MeasureText("PRESS R TO RESTART", SQUARE_SIZE) / 2, HEIGHT / 2 + SQUARE_SIZE, SQUARE_SIZE, RED);
		if (IsKeyPressed(KEY_R))
		{
			initialize_game();
		}
	}
}
