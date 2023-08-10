/*
Title: Pebble Math
Author: Kevin Barnard
Description: A board game where two players
throw stones and can land on lines,
crosses or a colour.

todo:
I have one pebble to play with.
Eventually i'll need three pebbles for
each player.

i could create more pebbles (a surface)
i could switch the rect for the pebble
i could handle the collision (which i need
the pebble instead of the rect)
i could use the magnitude for the rolling

launchpebble makes one pebble bounce
if i want three pebbles not to collide
id need to find .back() in landed
*/

#include "header.hpp"

// Function for loading an image to SDL_Texture
static SDL_Texture *loadImage(SDL_Renderer *renderer, const char *path)
{
    SDL_Surface *img = IMG_Load(path);
    if (img == NULL)
    {
        return NULL;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, img);
    SDL_FreeSurface(img);
    if (texture == NULL)
    {
        return NULL;
    }
    return texture;
}

int main(int argc, char *argv[])
{
    CGame game{};

    if (game.Init() != 0)
    {
        return 1;
    }
    else
    {
        game.Run();
    }
    return 0;
}

// could return a pebble?
void CPebble::CreatePebble(
    SDL_Renderer *rend)
{
    SDL_Surface *pebbleSurface =
        SDL_CreateRGBSurfaceWithFormat(
            0,
            60, 60,
            32,
            SDL_PIXELFORMAT_RGBA32);

    if (pebbleSurface == NULL)
    {
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_ERROR,
            "Title",
            SDL_GetError(),
            NULL);
    }

    Uint32 colBlack =
        SDL_MapRGB(pebbleSurface->format,
                   0, 0, 0);
    Uint32 colWhite =
        SDL_MapRGB(pebbleSurface->format,
                   255, 255, 255);

    // vector of pebbles

    //black
    SDL_Rect first{0, 0, 60, 60};
    SDL_FillRect(pebbleSurface, &first, colBlack);
    //white
    SDL_Rect top{20, 0, 20, 20};
    SDL_FillRect(pebbleSurface, &top, colWhite);
    //mid
    SDL_Rect mid{0, 20, 60, 20};
    SDL_FillRect(pebbleSurface, &mid, colWhite);
    //bottom
    SDL_Rect bot{20, 40, 20, 20};
    SDL_FillRect(pebbleSurface, &bot, colWhite);

    pebbleTexture =
        SDL_CreateTextureFromSurface(
            rend, pebbleSurface);

    SDL_FreeSurface(pebbleSurface);
}

// create the texture
// and colliders
CPebble CGame::CreatePebbles(
    int tx, int ty, int tw, int th,
    int mx, int my, int mw, int mh,
    int bx, int by, int bw, int bh)
{
    CPebble pebble{};
    pebble.colliders.resize(3);

    SDL_Surface *pebbleSurface =
        SDL_CreateRGBSurfaceWithFormat(
            0,
            60, 60,
            32,
            SDL_PIXELFORMAT_RGBA32);

    if (pebbleSurface == NULL)
    {
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_ERROR,
            "title",
            SDL_GetError(),
            NULL);
    }

    /*
    Uint32 colBlack =
        SDL_MapRGB(pebbleSurface->format,
        0,0,0);
    */
    Uint32 colWhite =
        SDL_MapRGB(pebbleSurface->format,
                   255, 255, 255);

    // draw
    SDL_Rect top{tx, ty, tw, th};
    SDL_FillRect(pebbleSurface, &top, colWhite);
    SDL_Rect mid{mx, my, mw, mh};
    SDL_FillRect(pebbleSurface, &mid, colWhite);
    SDL_Rect bot{bx, by, bw, bh};
    SDL_FillRect(pebbleSurface, &bot, colWhite);
    // collision
    pebble.colliders[0] = top;
    pebble.colliders[1] = mid;
    pebble.colliders[2] = bot;
    // texture
    pebble.pebbleTexture =
        SDL_CreateTextureFromSurface(
            renderer, pebbleSurface);
    SDL_FreeSurface(pebbleSurface);

    return pebble;
}

// do i want to return a SDL_Rect?
void CPebble::LaunchPebble(
    Vec2 start,
    int bounces)
{
    // say the energy goes in here...
    // 50 is 1 unit
    int mag = 50 + rand() % 100;
    int x = -2 + rand() % 4;
    int y = -2 + -(rand() % 4);

    Vec2 s{start};
    int ready{0};
    Vec2 land{0};

    while (ready < bounces)
    {
        // try to throw
        land.x = s.x + x * mag;
        land.y = s.y + y * mag;
        mag = 50 + rand() % 100;
        x = -2 + rand() % 4;
        y = -2 + rand() % 4;
        if (InBounds(land) &&
            NotColliding(landed, land))
        {
            s = land;
            ++ready;
            landed.push_back(s);
        }
    }

    /*
    for (int i = 0; i < bounces; ++i)
    {
        s.x += x * mag;
        s.y += y * mag;
        mag = 50 + rand() % 100;
        x = -2 + rand() % 4;
        y = -2 + rand() % 4;

        landed.push_back(s);
    }
*/
}

bool CPebble::InBounds(Vec2 xy)
{
    if (xy.x >= 0 && xy.x <= 720 && xy.y >= 0 && xy.y <= 720)
        return true;
    return false;
}

bool CPebble::NotColliding(
    std::vector<Vec2> v, Vec2 vec)
{
    SDL_Rect f{vec.x, vec.y, 50, 50};

    for (int i = 0; i < v.size(); ++i)
    {
        SDL_Rect s{v[i].x, v[i].y, 50, 50};

        if (SDL_HasIntersection(&f, &s))
        {
            return false;
        }
    }
    return true;
}

Vec2 operator+(Vec2 &s, Vec2 &f)
{
    s.x += f.x;
    s.y += f.y;
}

int CGame::Init()
{
    if (SDL_Init(
            SDL_INIT_VIDEO | SDL_INIT_AUDIO) == 0)
    {
        ++created;
    }

    window = SDL_CreateWindow(
        "Pebble Title",
        0, 0,
        0, 0,
        SDL_WINDOW_MAXIMIZED);
    if (window != NULL)
    {
        ++created;
    }

    renderer = SDL_CreateRenderer(
        window, -1, 0);
    if (renderer != NULL)
    {
        ++created;
    }

    // audio test
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 1, 4096) == -1)
        return 1;

    chunk = Mix_LoadWAV("magnet_on.wav");

    if (chunk != NULL)
    {
        ++created;
    }

    Mix_Volume(0, 30);

    if (created < 4)
        return 1;
    else
    {
        srand(time(0));
        SDL_GetRendererOutputSize(
            renderer,
            &sw, &sh);
        return 0;
    }
}

void CGame::Quit()
{
    Mix_FreeChunk(chunk);
    SDL_DestroyTexture(white.curveTexture);
    SDL_DestroyTexture(black.curveTexture);
    for (int i = 0; i < pebbles.size(); ++i)
    {
        SDL_DestroyTexture(pebbles[i].pebbleTexture);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    SDL_Quit();
}

void CGame::Load()
{
    // board
    white.curveTexture =
        loadImage(renderer, "imgs/white.png");
    black.curveTexture =
        loadImage(renderer, "imgs/black.png");

    // controls
    upTexture = 
        loadImage(renderer, "imgs/upArrow.png");
    downTexture =
        loadImage(renderer, "imgs/downArrow.png");
    leftTexture =
        loadImage(renderer, "imgs/leftArrow.png");
    rightTexture =
        loadImage(renderer, "imgs/rightArrow.png");
    dropTexture =
        loadImage(renderer, "imgs/drop.png");
        
    MakeColliders();

    // pebbles
    pebbles.push_back(
        CreatePebbles(
            0, 0, 60, 20,
            20, 20, 40, 20,
            40, 40, 20, 20));

    pebbles.push_back(
        CreatePebbles(
            20, 0, 20, 20,
            0, 20, 60, 20,
            0, 40, 60, 20));
}

void CGame::Run()
{
    Load();

    while (running)
    {
        Input();
    }
}

void CGame::MakeColliders()
{
    int Xoffset{160};
    int Yoffset{105};
    int Xwidth{120};
    int Yheight{120};

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 5; ++j)
        {
            int y = Yoffset + Yheight * j;
            SDL_Rect temp{Xoffset, y, 30, 30};
            board.crossColls.push_back(temp);
        }
        Xoffset += Xwidth;
    }

    int wXoffA{60};
    int wYoff{40};
    int wXoffB{175};
    int wW{20};
    int wH{40};

    SDL_Rect b = board.aaRect;
    SDL_Rect r{wXoffA + b.x, wYoff + b.y, wW,wH};
    board.whiteColls.push_back(r);
    b = board.abRect;
    r = {wXoffA + b.x, wYoff + b.y, wW, wH};
    board.whiteColls.push_back(r);
    b = board.baRect;
    r = {wXoffB + b.x, wYoff + b.y, wW, wH};
    board.whiteColls.push_back(r);
    b = board.bbRect;
    r = {wXoffB + b.x, wYoff + b.y, wW, wH};
    board.whiteColls.push_back(r);

    b = board.caRect;
    r = {wXoffA + b.x, wYoff + b.y, wW, wH};
    board.whiteColls.push_back(r);
    b = board.cbRect;
    r = {wXoffA + b.x, wYoff + b.y, wW, wH};
    board.whiteColls.push_back(r);
    b = board.daRect;
    r = {wXoffB + b.x, wYoff + b.y, wW, wH};
    board.whiteColls.push_back(r);
    b = board.dbRect;
    r = {wXoffB + b.x, wYoff + b.y, wW, wH};
    board.whiteColls.push_back(r);

    // controls
    upRect = {60, 800, 120, 120};
    downRect = {180, 800, 120, 120};
    leftRect = {300, 800, 120, 120};
    rightRect = {420, 800, 120, 120};
    dropRect = {540, 800, 120, 120};
    // quit
    quitButton.x = sw - 200;
    quitButton.y = sh - 100;
    quitButton.w = 200;
    quitButton.h = 100;
}

void CGame::Input()
{
    SDL_Event e{};
    SDL_TouchFingerEvent f{};

    while (SDL_PollEvent(&e))
    {
        f = e.tfinger;
        touch.x = f.x * sw;
        touch.y = f.y * sh;

        switch (e.type)
        {
        case SDL_FINGERDOWN:
        {
            Mix_PlayChannel(0, chunk, 0);
            //throw
            Vec2 v{board.throwPos.x,
                   board.throwPos.y};

            pebble.LaunchPebble(v, 3);

            if (SDL_HasIntersection(&touch, &quitButton))
            {
                running = false;
            }

            UpdateScreen();

            pebble.landed.clear();
            break;
        }
        default:
            break;
        }
    }
}

void CGame::UpdateScreen()
{
    // back screen
    SDL_SetRenderDrawColor(
        renderer,
        scrColr, scrColg, scrColb, 255);
    SDL_RenderClear(renderer);

    // border
    SDL_SetRenderDrawColor(
        renderer,
        200, 200, 200, 255);
    SDL_Rect border{60, 60, 600, 600};
    SDL_RenderFillRect(
        renderer,
        &border);

    // curves
    SDL_RenderCopy(
        renderer,
        white.curveTexture,
        NULL,
        &board.aaRect);
    SDL_RenderCopy(
        renderer,
        white.curveTexture,
        NULL,
        &board.abRect);
    SDL_RenderCopy(
        renderer,
        black.curveTexture,
        NULL,
        &board.baRect);
    SDL_RenderCopy(
        renderer,
        black.curveTexture,
        NULL,
        &board.bbRect);
    SDL_RenderCopy(
        renderer,
        white.curveTexture,
        NULL,
        &board.caRect);
    SDL_RenderCopy(
        renderer,
        white.curveTexture,
        NULL,
        &board.cbRect);
    SDL_RenderCopy(
        renderer,
        black.curveTexture,
        NULL,
        &board.daRect);
    SDL_RenderCopy(
        renderer,
        black.curveTexture,
        NULL,
        &board.dbRect);

    // temp pebbles
    for (int i = 0; i < pebbles.size(); ++i)
    {
        SDL_Rect r{0 + i * 60, 0, 60, 60};

        SDL_RenderCopy(
            renderer,
            pebbles[i].pebbleTexture,
            NULL,
            &r);
    }

    // throw placeholder
    SDL_SetRenderDrawColor(
        renderer,
        255, 0, 0, 255);
    SDL_RenderFillRect(
        renderer,
        &board.throwPos);

    // test rects
    for (int i = 0; i < pebble.landed.size(); ++i)
    {
        SDL_Rect fr{pebble.landed[i].x,
                    pebble.landed[i].y,
                    50, 50};

        int j{0}, k{0}, l{0};
        if (i == 0)
        {
            j = 1;
        }
        if (i == 1)
        {
            k = 1;
        }
        if (i == 2)
        {
            l = 1;
        }
        SDL_SetRenderDrawColor(
            renderer,
            j * 255, k * 255, l * 255, 255);

        SDL_RenderFillRect(
            renderer,
            &fr);
    }

    // test collision
    SDL_SetRenderDrawColor(
        renderer,
        75, 75, 200, 255);

    for (int i = 0; i < board.crossColls.size(); ++i)
    {
        SDL_RenderDrawRect(
            renderer,
            &board.crossColls[i]);
    }
    for (int i = 0; i < board.whiteColls.size(); ++i)
    {
        SDL_RenderDrawRect(
            renderer,
            &board.whiteColls[i]);
    }
    
    // controls
    SDL_RenderCopy(
        renderer,
        upTexture,
        NULL,
        &upRect);
    SDL_RenderCopy(
        renderer,
        downTexture,
        NULL,
        &downRect);
    SDL_RenderCopy(
        renderer,
        leftTexture,
        NULL,
        &leftRect);
    SDL_RenderCopy(
        renderer,
        rightTexture,
        NULL,
        &rightRect);
    SDL_RenderCopy(
        renderer,
        dropTexture,
        NULL,
        &dropRect);

    SDL_RenderDrawRect(renderer, &touch);
    SDL_RenderFillRect(renderer, &quitButton);

    SDL_RenderPresent(renderer);
}