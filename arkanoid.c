#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <SDL2/SDL.h>

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 875
#define MAX_DELAY 500
#define MAX_FPS 60
#define PADDLE_WIDTH 250
#define PADDLE_HEIGHT 20
#define PADDLE_SPEED 15
#define PADDLE_Y 800
#define BALL_DIAMETER 25
#define BALL_SPEED 15
#define MAX_ROW 9
#define MAX_COL 14
#define BRICK_HEIGHT 20

SDL_Renderer *renderer;
SDL_Window *window;
SDL_Rect paddle, ball, brick;

int frameCount, timerFPS, lastFrame, is_running, blocks_on;
int bricks[MAX_ROW][MAX_COL];
int map[MAX_ROW][MAX_COL];

float mvX, mvY;

void resetGame(void)
{
    blocks_on = 0;
    paddle.w = PADDLE_WIDTH;
    paddle.h = PADDLE_HEIGHT;
    paddle.x = (SCREEN_WIDTH / 2) - (PADDLE_WIDTH / 2);
    paddle.y = PADDLE_Y;
    ball.w = ball.h = BALL_DIAMETER;
    ball.x = (SCREEN_WIDTH / 2) - (BALL_DIAMETER / 2);
    ball.y = PADDLE_Y - 200;
    brick.w = (SCREEN_WIDTH - 10 - (10 * MAX_COL)) / MAX_COL;
    brick.h = BRICK_HEIGHT;
    mvX = 0;
    mvY = BALL_SPEED;
    for (int i = 0; i < MAX_ROW; i++)
    {
        for (int j = 0; j < MAX_COL; j++)
        {
            bricks[i][j] = map[i][j];
            if (bricks[i][j])
                blocks_on++;
        }
    }
    return;
}

void putBrick(int i, int j)
{
    brick.x = (SCREEN_WIDTH - MAX_COL * brick.w - (MAX_COL - 1) * 10) / 2 + j * brick.w + j * 10;
    brick.y = 20 + i + i * brick.h + i * 10;
}

void defaultMapInit(void)
{
    for (int i = 0; i < MAX_ROW; i++)
    {
        for (int j = 0; j < MAX_COL; j++)
        {
            map[i][j] = 1;
        }
    }
    return;
}

void customMapInit(FILE *file)
{
    int row = 0, col = 0;
    int c;
    for (int i = 0; i <= MAX_ROW; i++)
    {
        for (int j = 0; j <= MAX_COL; j++)
        {
            c = getc(file);
            if (j == MAX_COL)
            {
                if (c != '\n' && c != EOF)
                {
                    fprintf(stderr, "Wrong file contents. Initializing default map.\n");
                    defaultMapInit();
                    return;
                }
                else
                    continue;
            }
            else
            {
                if (c != '1' && c != '0' && c != '\n' && c != EOF)
                {
                    fprintf(stderr, "Wrong file contents. Initializing default map.\n");
                    defaultMapInit();
                    return;
                }
            }
            map[i][j] = c - '0';
        }
    }
}

void paddle_bounce(void)
{
    float relativeIntersectX = (paddle.x + (paddle.w / 2)) - (ball.x + (BALL_DIAMETER / 2));
    float normalizedRelativeIntersectX = relativeIntersectX / (paddle.w / 2);
    float bounceAngle = normalizedRelativeIntersectX * (5 * 3.1415 / 20); // max angle
    if (fabs(bounceAngle) < 0.1)
    {
        if (bounceAngle > 0)
            bounceAngle += 0.1;
        else if (bounceAngle < 0)
            bounceAngle -= 0.1;
        else
        {
            if (rand() % 2 == 0)
                bounceAngle += 0.1;
            else
                bounceAngle -= 0.1;
        }
    }
    mvX = BALL_SPEED * -sin(bounceAngle);
    mvY = -BALL_SPEED * cos(bounceAngle);
    return;
    // https://gamedev.stackexchange.com/questions/4253/in-pong-how-do-you-calculate-the-balls-direction-when-it-bounces-off-the-paddl
}

void prepare(void)
{
    if (SDL_HasIntersection(&ball, &paddle))
        paddle_bounce();
    if (paddle.x <= 0)
        paddle.x = 0;
    if (paddle.x + paddle.w >= SCREEN_WIDTH)
        paddle.x = SCREEN_WIDTH - PADDLE_WIDTH;
    if (ball.x <= 0 || ball.x + BALL_DIAMETER > SCREEN_WIDTH)
        mvX = -mvX;
    if (ball.y <= 0)
    {
        ball.y = 0;
        mvY = -mvY;
    }
    if (ball.y + BALL_DIAMETER / 2 >= PADDLE_Y + PADDLE_HEIGHT / 2)
        resetGame();

    ball.x += mvX;
    ball.y += mvY;

    for (int i = 0; i < MAX_ROW; i++)
    {
        for (int j = 0; j < MAX_COL; j++)
        {
            if (bricks[i][j])
                putBrick(i, j);
            if (SDL_HasIntersection(&ball, &brick) && bricks[i][j] == 1)
            {
                blocks_on--;
                bricks[i][j] = 0;
                mvY = -mvY;
                if (ball.y <= brick.y)
                {
                    ball.y = ball.y - 20;
                }
                if (ball.y >= brick.y)
                {
                    ball.y = ball.y + 20;
                }
            }
        }
    }
    return;
}

void input(void)
{
    const unsigned char *input = SDL_GetKeyboardState(NULL);
    if (input[SDL_SCANCODE_RIGHT] || input[SDL_SCANCODE_D])
        paddle.x += PADDLE_SPEED;
    if (input[SDL_SCANCODE_LEFT] || input[SDL_SCANCODE_A])
        paddle.x -= PADDLE_SPEED;
    if (input[SDL_SCANCODE_R])
        resetGame();
    if (input[SDL_SCANCODE_Q])
        is_running = 0;
    return;
}

void optimizeFrames(void)
{
    timerFPS = SDL_GetTicks64() - lastFrame;
    if (timerFPS < (MAX_DELAY / MAX_FPS))
        SDL_Delay((MAX_DELAY / MAX_FPS) - timerFPS);
    return;
}

void draw(void)
{
    SDL_SetRenderDrawColor(renderer, 40, 2, 30, 255);
    SDL_RenderClear(renderer);
    optimizeFrames();
    SDL_SetRenderDrawColor(renderer, 238, 129, 179, 255);
    SDL_RenderFillRect(renderer, &paddle);
    SDL_SetRenderDrawColor(renderer, 255, 249, 215, 255);
    SDL_RenderFillRect(renderer, &ball);
    SDL_SetRenderDrawColor(renderer, 129, 9, 85, 255);
    for (int i = 0; i < MAX_ROW; i++)
    {
        for (int j = 0; j < MAX_COL; j++)
        {
            if (bricks[i][j])
            {
                putBrick(i, j);
                SDL_RenderFillRect(renderer, &brick);
            }
        }
    }
    SDL_RenderPresent(renderer);
    return;
}

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
        fprintf(stderr, "Failed Initialization\n");
    if (SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC, &window, &renderer) < 0)
        fprintf(stderr, "Failed at Creating Window\n");
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_Event event;
    srand(time(NULL));
    is_running = 1;
    int lastTime = 0;
    if (argc == 2)
    {
        char file_path[14 + 50] = "./custom_maps/";
        strcat(file_path, argv[1]);
        FILE *file = fopen(file_path, "r");
        if (file == NULL)
        {
            defaultMapInit();
            fprintf(stderr, "Failed at opening file. Initializing default map.\n");
        }
        else
        {
            customMapInit(file);
            fclose(file);
        }
    }
    else if (argc > 2)
    {
        fprintf(stderr, "Too many arguments. Initializing default map.\n");
        defaultMapInit();
    }
    else
        defaultMapInit();
    resetGame();
    while (is_running != 0)
    {
        while (SDL_PollEvent(&event))
            if (event.type == SDL_QUIT)
                is_running = 0;
        lastFrame = SDL_GetTicks64();
        if (lastFrame >= (lastTime + MAX_DELAY))
        {
            lastTime = lastFrame;
        }
        prepare();
        input();
        draw();
        if (blocks_on == 0)
        {
            SDL_Delay(1000);
            resetGame();
        }
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}