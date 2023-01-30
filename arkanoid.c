#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 875
#define PADDLE_WIDTH 250
#define PADDLE_HEIGHT 20
#define PADDLE_SPEED 15
#define PADDLE_Y 800
#define BALL_DIAMETER 20
#define BALL_SPEED 15
#define MAX_IN_COL 9
#define MAX_IN_ROW 14
#define BRICK_HEIGHT 40
#define FPS 30

SDL_Color fontColor = {205, 102, 77};
SDL_Event event;
SDL_Renderer *renderer;
SDL_Rect paddle, ball, brick, field;
SDL_Surface *message;
SDL_Texture *messageTexture;
SDL_Window *window;
TTF_Font *Font;

int is_running, bricks_on, afterReset, nightMode, firstLaunch = 1;
int bricks[MAX_IN_COL][MAX_IN_ROW];
int map[MAX_IN_COL][MAX_IN_ROW];

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
    brick.w = (SCREEN_WIDTH - 50 - (10 * MAX_IN_ROW)) / MAX_IN_ROW;
    brick.h = BRICK_HEIGHT;
    mvX = 0;
    mvY = BALL_SPEED;
    for (int i = 0; i < MAX_IN_COL; i++)
    {
        for (int j = 0; j < MAX_IN_ROW; j++)
        {
            bricks[i][j] = map[i][j];
            if (bricks[i][j])
                if (bricks[i][j] != 4)
                    bricks_on += bricks[i][j];
        }
    }
    afterReset = 1;
    return;
}

void endScreen(void)
{
    SDL_SetRenderDrawColor(renderer, 15, 13, 12, 255);
    SDL_RenderClear(renderer);
    TTF_SetFontSize(Font, 100);
    if (bricks_on == 0)
        message = TTF_RenderText_Solid(Font, "[!YOU WIN!]", fontColor);
    else
        message = TTF_RenderText_Solid(Font, "[YOU LOSE]", fontColor);
    if (message == NULL)
    {
        fprintf(stderr, "Failed at creating message.\n");
        is_running = 0;
        return;
    }
    messageTexture = SDL_CreateTextureFromSurface(renderer, message);
    if (messageTexture == NULL)
    {
        fprintf(stderr, "Failed at creating texture.\n");
        is_running = 0;
        return;
    }
    int texW2 = 0;
    int texH2 = 0;
    if (SDL_QueryTexture(messageTexture, NULL, NULL, &texW2, &texH2) < 0)
        fprintf(stderr, "Failed at setting attributes to texture.\n");
    SDL_Rect dstrect2 = {SCREEN_WIDTH / 2 - message->w / 2, SCREEN_HEIGHT / 2 - message->h / 2, texW2, texH2};
    if (SDL_RenderCopy(renderer, messageTexture, NULL, &dstrect2) < 0)
        fprintf(stderr, "Failed at setting texture to current rendering.\n");
    SDL_RenderPresent(renderer);
    SDL_Delay(2000);
    SDL_FreeSurface(message);
    SDL_DestroyTexture(messageTexture);
    resetGame();
    return;
}

void setBrickPos(int i, int j)
{
    brick.x = (SCREEN_WIDTH - MAX_IN_ROW * brick.w - (MAX_IN_ROW - 1) * 10) / 2 + j * brick.w + j * 10;
    brick.y = 20 + i + i * brick.h + i * 10;
    return;
}

void defaultMapInit(void)
{
    for (int i = 0; i < MAX_IN_COL; i++)
    {
        for (int j = 0; j < MAX_IN_ROW; j++)
        {
            map[i][j] = 1;
        }
    }
    return;
}

void customMapInit(FILE *file)
{
    int c, cnt = 0;
    for (int i = 0; i < MAX_IN_COL; i++)
    {
        for (int j = 0; j < MAX_IN_ROW; j++)
        {
            bricks[i][j] = 0;
            map[i][j] = 0;
        }
    }
    for (int i = 0; i <= MAX_IN_COL; i++)
    {
        if (i == MAX_IN_COL && c != EOF)
        {
            fprintf(stderr, "Wrong file contents. Initializing default map.\n");
            defaultMapInit();
            return;
        }
        for (int j = 0; j <= MAX_IN_ROW; j++)
        {
            c = getc(file);
            if (j == MAX_IN_ROW)
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
                if (c != '1' && c != '0' && c != '2' && c != '3' && c != '4')
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
            if (map[i][j] < 4 && map[i][j] > 0)
                cnt++;
        }
    }
    if (cnt == 0)
    {
        fprintf(stderr, "Wrong file contents. Initializing default map.\n");
        defaultMapInit();
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
    {
        ball.x = brick.x + brick.w;
        mvX = -mvX;
    }
    else if (mvX > 0 && ball.x + ball.w < brick.x + slip && ball.y < brick.y + brick.h && ball.y + ball.h > brick.y)
    {
        ball.x = brick.x - ball.w;
        mvX = -mvX;
    }
    else if (mvY < 0 && ball.x + ball.w > brick.x && ball.x < brick.x + brick.w && ball.y > brick.y + brick.h - ball.h)
    {
        ball.y = brick.y + brick.h;
        mvY = -mvY;
    }
    else if (mvY > 0 && ball.x + ball.w > brick.x && ball.x < brick.x + brick.w && ball.y + ball.h < brick.y + ball.h)
    {
        ball.y = brick.y - ball.h;
        mvY = -mvY;
    }
    else if ((mvY < 0 && ball.y + ball.h <= brick.y + brick.h / 2) || (mvY > 0 && ball.y >= brick.y + brick.h / 2))
    {
        mvX = -mvX;
    }
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
    if (ball.x <= 0 || ball.x + BALL_DIAMETER >= SCREEN_WIDTH)
        mvX = -mvX;
    if (ball.y <= 0)
    {
        ball.y = 0;
        mvY = -mvY;
    }
    if (ball.y + BALL_DIAMETER / 2 >= PADDLE_Y)
        endScreen();

    ball.x += mvX;
    ball.y += mvY;

    for (int i = 0; i < MAX_IN_COL; i++)
    {
        for (int j = 0; j < MAX_IN_ROW; j++)
        {
            if (bricks[i][j] > 0)
                setBrickPos(i, j);
            if (SDL_HasIntersection(&ball, &brick) && bricks[i][j])
            {
                if (bricks[i][j] != 4)
                {
                    bricks_on--;
                    bricks[i][j] -= 1;
                }
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
    if (input[SDL_SCANCODE_E])
    {
        nightMode = -nightMode;
        SDL_Delay(200);
    }
    if (input[SDL_SCANCODE_Q])
        is_running = 0;
    return;
}

void draw(void)
{
    if (nightMode < 1)
    {
        SDL_SetRenderDrawColor(renderer, 199, 193, 172, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 153, 152, 131, 255);
        SDL_RenderFillRect(renderer, &paddle);
        SDL_RenderDrawLineF(renderer, 0, paddle.y + (paddle.h / 2), SCREEN_WIDTH, paddle.y + (paddle.h / 2));
        SDL_SetRenderDrawColor(renderer, 205, 102, 77, 255);
        SDL_RenderFillRect(renderer, &ball);
    }
    else
    {
        SDL_SetRenderDrawColor(renderer, 18, 20, 19, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 82, 67, 58, 255);
        SDL_RenderFillRect(renderer, &paddle);
        SDL_RenderDrawLineF(renderer, 0, paddle.y + (paddle.h / 2), SCREEN_WIDTH, paddle.y + (paddle.h / 2));
        SDL_SetRenderDrawColor(renderer, 205, 102, 77, 255);
        SDL_RenderFillRect(renderer, &ball);
    }
    for (int i = 0; i < MAX_IN_COL; i++)
    {
        for (int j = 0; j < MAX_IN_ROW; j++)
        {
            if (bricks[i][j])
            {
                setBrickPos(i, j);
                if (nightMode < 1)
                {
                    if (bricks[i][j] == 4)
                    {
                        SDL_SetRenderDrawColor(renderer, 136, 124, 101, 255);
                    }
                    else if (bricks[i][j] == 3)
                        SDL_SetRenderDrawColor(renderer, 187, 155, 129, 255);
                    else if (bricks[i][j] == 2)
                        SDL_SetRenderDrawColor(renderer, 169, 161, 130, 255);
                    else
                        SDL_SetRenderDrawColor(renderer, 227, 217, 170, 255);
                }
                else
                {
                    if (bricks[i][j] == 4)
                    {
                        SDL_SetRenderDrawColor(renderer, 35, 36, 31, 255);
                    }
                    else if (bricks[i][j] == 3)
                        SDL_SetRenderDrawColor(renderer, 62, 60, 50, 255);
                    else if (bricks[i][j] == 2)
                        SDL_SetRenderDrawColor(renderer, 88, 75, 62, 255);
                    else
                        SDL_SetRenderDrawColor(renderer, 118, 98, 87, 255);
                }
                SDL_RenderFillRect(renderer, &brick);
                if (bricks[i][j] == 4)
                {
                    if (nightMode < 1)
                        SDL_SetRenderDrawColor(renderer, 199, 193, 172, 255);
                    else
                        SDL_SetRenderDrawColor(renderer, 81, 74, 63, 255);
                    SDL_RenderDrawLineF(renderer, brick.x, brick.y, brick.x + brick.w, brick.y + brick.h);
                    SDL_RenderDrawLineF(renderer, brick.x, brick.y + brick.h, brick.x + brick.w, brick.y);
                }
            }
        }
    }
    SDL_RenderPresent(renderer);
    return;
}

void afterResetAwait(void)
{
    TTF_SetFontSize(Font, 30);
    message = TTF_RenderText_Solid(Font, "[Press any key to start]", fontColor);
    if (message == NULL)
    {
        fprintf(stderr, "Failed at creating message.\n");
        is_running = 0;
        return;
    }
    messageTexture = SDL_CreateTextureFromSurface(renderer, message);
    if (messageTexture == NULL)
    {
        fprintf(stderr, "Failed at creating texture.\n");
        is_running = 0;
        return;
    }
    int texW = 0;
    int texH = 0;
    if (SDL_QueryTexture(messageTexture, NULL, NULL, &texW, &texH) < 0)
        fprintf(stderr, "Failed at setting attributes to texture.\n");
    SDL_Rect dstrect = {SCREEN_WIDTH / 2 - message->w / 2, ball.y - 100, texW, texH};
    if (SDL_RenderCopy(renderer, messageTexture, NULL, &dstrect) < 0)
        fprintf(stderr, "Failed at setting texture to current rendering.\n");
    SDL_RenderPresent(renderer);
    while (1)
    {
        if (SDL_WaitEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                is_running = 0;
                return;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                afterReset = 0;
                break;
            }
        }
    }
    SDL_Delay(200);
    SDL_DestroyTexture(messageTexture);
    SDL_FreeSurface(message);
    return;
}

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        fprintf(stderr, "Failed Initialization\n");
        return 0;
    }
    if (SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC, &window, &renderer) < 0)
    {
        fprintf(stderr, "Failed at Creating Window\n");
        return 0;
    }
    if (TTF_Init() < 0)
    {
        fprintf(stderr, "Failed at initializing font.\n");
        return 0;
    }
    Font = TTF_OpenFont("./include/Retro_Gaming.ttf", 30);
    if (Font == NULL)
    {
        fprintf(stderr, "Failed at opening font.\n");
        return 0;
    }
    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear"))
        fprintf(stderr, "SetHint did not work properly.\n");
    srand(time(NULL));
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
    is_running = 1;
    nightMode = -1;
    unsigned int ticks;
    while (is_running != 0)
    {
        ticks = SDL_GetTicks();
        while (SDL_PollEvent(&event))
            if (event.type == SDL_QUIT)
                is_running = 0;
        prepare();
        input();
        draw();
        if (afterReset)
            afterResetAwait();
        if (bricks_on == 0)
        {
            SDL_Delay(500);
            endScreen();
        }
        if (1000 / FPS > (SDL_GetTicks() - ticks))
            SDL_Delay(1000 / FPS - (SDL_GetTicks() - ticks));
    }
    TTF_CloseFont(Font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}