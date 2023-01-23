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
#define BALL_DIAMETER 20
#define BALL_SPEED 15
#define MAX_ROW 9
#define MAX_COL 14
#define BRICK_HEIGHT 40
#define FPS 30

SDL_Renderer *renderer;
SDL_Window *window;
SDL_Rect paddle, ball, brick;

int frameCount, timerFPS, lastFrame, is_running, bricks_on;
int bricks[MAX_ROW][MAX_COL];
int map[MAX_ROW][MAX_COL];

float mvX, mvY;

void resetGame(void)
{
    bricks_on = 0;
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
                bricks_on++;
        }
    }
    return;
}

void setBrickPos(int i, int j)
{
    brick.x = (SCREEN_WIDTH - MAX_COL * brick.w - (MAX_COL - 1) * 10) / 2 + j * brick.w + j * 10;
    brick.y = 20 + i + i * brick.h + i * 10;
    return;
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
    int c, cnt = 0;
    for (int i = 0; i < MAX_ROW; i++)
    {
        for (int j = 0; j < MAX_COL; j++)
        {
            bricks[i][j] = 0;
            map[i][j] = 0;
        }
    }
    for (int i = 0; i <= MAX_ROW; i++)
    {
        if (i == MAX_ROW && c != EOF)
        {
            fprintf(stderr, "Wrong file contents. Initializing default map.\n");
            defaultMapInit();
            return;
        }
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
                if (c != '1' && c != '0')
                {
                    if (j == 0 && c == EOF)
                        break;
                    fprintf(stderr, "Wrong file contents. Initializing default map.\n");
                    defaultMapInit();
                    return;
                }
            }
            if (c == EOF)
                break;
            map[i][j] = c - '0';
            if (map[i][j] == 1)
                cnt++;
        }
    }
    if (cnt == 0)
    {
        fprintf(stderr, "Wrong file contents. Initializing default map.\n");
        defaultMapInit();
        return;
    }
    return;
}

void paddle_bounce(void)
{
    float delta = 11;
    ball.y = paddle.y - BALL_DIAMETER;
    float relativeIntersectX = (paddle.x + (paddle.w / 2)) - (ball.x + (BALL_DIAMETER / 2));
    float normalizedRelativeIntersectX = relativeIntersectX / (paddle.w / 2);
    float bounceAngle = normalizedRelativeIntersectX * (3.1415 / 4); // MAX ANGLE: 45 DEGREE
    if (ball.x + (ball.w / 2) >= paddle.x + (paddle.w / 2) - delta && ball.x + (ball.w / 2) <= paddle.x + (PADDLE_WIDTH / 2) + delta)
    {
        if (mvX > 0)
        {
            mvX = BALL_SPEED * -sin(-0.1);
            mvY = -BALL_SPEED * cos(-0.1);
            return;
        }
        else if (mvX < 0)
        {
            mvX = BALL_SPEED * -sin(0.1);
            mvY = -BALL_SPEED * cos(0.1);
            return;
        }
        else
        {
            if (rand() % 2 == 0)
            {
                mvX = BALL_SPEED * -sin(0.1);
                mvY = -BALL_SPEED * cos(0.1);
                return;
            }
            else
            {
                mvX = BALL_SPEED * -sin(0.1);
                mvY = -BALL_SPEED * cos(0.1);
                return;
            }
        }
    }
    mvX = BALL_SPEED * -sin(bounceAngle);
    mvY = -BALL_SPEED * cos(bounceAngle);
    return;
    // https://gamedev.stackexchange.com/questions/4253/in-pong-how-do-you-calculate-the-balls-direction-when-it-bounces-off-the-paddl
}

void brickBounce(void)
{
    float slip = (BALL_DIAMETER / 2) / 2;
    if (mvX < 0 && ball.x > brick.x + brick.w - slip && ball.y < brick.y + brick.h && ball.y + ball.h > brick.y)
        mvX = -mvX;
    else if (mvX > 0 && ball.x + ball.w < brick.x + slip && ball.y < brick.y + brick.h && ball.y + ball.h > brick.y)
        mvX = -mvX;
    else if (mvY < 0 && ball.x + ball.w > brick.x && ball.x < brick.x + brick.w && ball.y > brick.y + brick.h - ball.h)
        mvY = -mvY;
    else if (mvY > 0 && ball.x + ball.w > brick.x && ball.x < brick.x + brick.w && ball.y + ball.h < brick.y + ball.h)
        mvY = -mvY;
    else
    {
        mvX = -mvX;
        mvY = -mvY;
    }
    return;
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
    if (ball.y + BALL_DIAMETER / 2 >= PADDLE_Y)
        resetGame();
    ball.x += mvX;
    ball.y += mvY;

    for (int i = 0; i < MAX_ROW; i++)
    {
        for (int j = 0; j < MAX_COL; j++)
        {
            if (bricks[i][j])
                setBrickPos(i, j);
            if (SDL_HasIntersection(&ball, &brick) && bricks[i][j] == 1)
            {
                bricks_on--;
                bricks[i][j] = 0;
                brickBounce();
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

void draw(void)
{
    SDL_SetRenderDrawColor(renderer, 40, 2, 30, 255);
    SDL_RenderClear(renderer);
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
                setBrickPos(i, j);
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
    unsigned int ticks;
    if (argc == 2)
    {
        FILE *file = fopen(argv[1], "r");
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
        ticks = SDL_GetTicks();
        while (SDL_PollEvent(&event))
            if (event.type == SDL_QUIT)
                is_running = 0;
        prepare();
        input();
        draw();
        if (bricks_on == 0)
        {
            SDL_Delay(1000);
            resetGame();
        }
        if (1000 / FPS > (SDL_GetTicks() - ticks))
            SDL_Delay(1000 / FPS - (SDL_GetTicks() - ticks));
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}