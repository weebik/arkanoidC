#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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
#define SENSITIVITY 4
#define MAX_ROW 9
#define MAX_COL 14
#define BRICK_HEIGHT 20

SDL_Renderer *renderer;
SDL_Window *window;
SDL_Rect paddle, ball, brick;

int frameCount, timerFPS, lastFrame, is_running, custom_map;
int bricks[MAX_ROW][MAX_COL];

float mvX, mvY;

void resetGame(void)
{
    paddle.w = PADDLE_WIDTH; 
    paddle.h = PADDLE_HEIGHT;
    paddle.x = (SCREEN_WIDTH/2) - (PADDLE_WIDTH/2);
    paddle.y = PADDLE_Y;
    ball.w = ball.h = BALL_DIAMETER;
    ball.x = (SCREEN_WIDTH/2) - (BALL_DIAMETER/2);
    ball.y = PADDLE_Y - 200;
    brick.w = (SCREEN_WIDTH-50-(5*MAX_COL))/MAX_COL;
    brick.h = BRICK_HEIGHT;
    mvX = 0;
    mvY = BALL_SPEED;
    //ukladaj cegly BUDOWA
    return;
}

void customMapRead()
{
    return;
}

void normalMapInit(void)
{
    for(int i=0; i<MAX_COL; i++)
    {
        memset(bricks[i], 1, MAX_ROW);
    }
    return;
}

void paddle_bounce(void)
{
    float relativeIntersectX = (paddle.x+(paddle.w/2))-(ball.x+(BALL_DIAMETER/2));
    float normalizedRelativeIntersectX = relativeIntersectX/(paddle.w/2);
    float bounceAngle = normalizedRelativeIntersectX * (5* 3.1415/12); //75degreee angle
    mvX=BALL_SPEED*-sin(bounceAngle);
    mvY=-BALL_SPEED*cos(bounceAngle);
    return;
    //https://gamedev.stackexchange.com/questions/4253/in-pong-how-do-you-calculate-the-balls-direction-when-it-bounces-off-the-paddl
}

void prepare(void)
{
    if(SDL_HasIntersection(&ball, &paddle)) paddle_bounce();
    if(paddle.x <= 0) paddle.x =0;
    if(paddle.x + paddle.w >= SCREEN_WIDTH) paddle.x = SCREEN_WIDTH - PADDLE_WIDTH;
    if(ball.x <= 0 || ball.x + BALL_DIAMETER > SCREEN_WIDTH) mvX = -mvX;
    if(ball.y <= 0) mvY = -mvY;
    if(ball.y + BALL_DIAMETER >= PADDLE_Y + PADDLE_HEIGHT + BALL_DIAMETER) resetGame();
    
    ball.x += mvX;
    ball.y += mvY;

    for(int i=0; i<MAX_ROW; i++)
    {
        for(int j=0; j<MAX_COL; j++)
        {
            //putbrick();
        }
    }
    //jesli wszystkie zniszczone resetGame();
    return;
}

void input(void)
{
    SDL_Event event;
    const unsigned char *input = SDL_GetKeyboardState(NULL);
    while(SDL_PollEvent(&event)) if(event.type == SDL_QUIT) is_running = 0;
    if(input[SDL_SCANCODE_RIGHT] || input[SDL_SCANCODE_D]) paddle.x += PADDLE_SPEED;
    if(input[SDL_SCANCODE_LEFT] || input[SDL_SCANCODE_A]) paddle.x -= PADDLE_SPEED;
    if(input[SDL_SCANCODE_R]) resetGame();
    if(input[SDL_SCANCODE_Q]) is_running = 0;
    return;
}

void optimizeFrames(void)
{
    timerFPS = SDL_GetTicks64()-lastFrame;
    if(timerFPS<(MAX_DELAY/MAX_FPS))
    {
        SDL_Delay((MAX_DELAY/MAX_FPS)-timerFPS);
    }
    return;
}

void draw(void)
{ 
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    optimizeFrames();
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &paddle);
    SDL_SetRenderDrawColor(renderer, 178, 102, 255, 255);
    SDL_RenderFillRect(renderer, &ball);
    SDL_RenderPresent(renderer);
    return;
}

int main(int argc, char* argv[])
{
    if(SDL_Init(SDL_INIT_EVERYTHING)<0) printf("Failed Initialization\n");
    if(SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_RENDERER_ACCELERATED  | SDL_RENDERER_PRESENTVSYNC, &window, &renderer)<0) printf("Failed at Creating Window\n");
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    is_running=1;
    int lastTime=0;
    resetGame();
    while(is_running!=0)
    {
        lastFrame=SDL_GetTicks64();
        if(lastFrame>=(lastTime+MAX_DELAY))
        {
            lastTime=lastFrame;
        }
        prepare();
        input();
        draw();   
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}