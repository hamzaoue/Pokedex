#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <string>
#include <fstream>
#include <filesystem>
#include <iostream> 
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <vector>
#include < functional >
constexpr int frame_width = 256;
constexpr int frame_height = 192;
constexpr int FPS = 60;
struct Point{ int x, y;};
struct Dimension{int width, height;};
//*****************************************************************************
//*************************** POKEMON_SPECIES *********************************
//*****************************************************************************
class Pokemon_Species
{
    private:
        SDL_Renderer* renderer_; 
        std::map<std::string, int>  values_;
        std::map<std::string, std::string> datas_;
        std::map<std::string, SDL_Texture*> sprites_;

    public:
        Pokemon_Species(SDL_Renderer* renderer);
        ~Pokemon_Species();
        void setDatas(std::string txtPath);
        void setSprites(std::string imgPath);
        SDL_Texture* createTexture(SDL_Surface* surface, SDL_Rect rect);
        int getValue(std::string);
        std::string getData(std::string);
        SDL_Texture* getSprite(std::string sprite);
};
//*****************************************************************************
//******************************* POKEDEX *************************************
//*****************************************************************************
class Pokedex
{
    private:
        std::map<std::string,SDL_Texture*> textures_;
        std::vector<Pokemon_Species*> species_;
        int currentIndex_, selectedBar_, consecutiveScroll_, lastScroll_;
        SDL_Renderer* renderer_;
        std::string region_;
        TTF_Font* font_;
        Mix_Chunk* plink_;
        bool shiny_;

    public:
        Pokedex(SDL_Renderer* renderer,std::string region, std::string folder);
        ~Pokedex();
        void draw();
        void drawText(std::string text, Point pos, SDL_Color color, bool center);
        void drawTexture(SDL_Texture* texture, SDL_Rect bounds);
        void init(std::string folder);
        void keyDown(SDL_Keycode key);
};
//*****************************************************************************
//******************************** APPLICATION ********************************
//*****************************************************************************
class Application
{
    private:
        SDL_Window* window_ptr_;
        SDL_Renderer* renderer_;
        Pokedex* pokedex_;

    public:
        Application();
        ~Application();
        void loop();
        void processEvents();
};
