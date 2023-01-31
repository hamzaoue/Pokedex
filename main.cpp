#include "Pokedex.h"

int main(int argc, char* argv[])
{
    //Initialize SDL , Initialize PNG loading
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) < 0)
        throw std::runtime_error("SDL_Init error");
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
        throw std::runtime_error("IMG_Init error");
    if (TTF_Init() == -1)
        throw std::runtime_error("TTF_Init error");
    if (SDL_INIT_AUDIO == -1)
        throw std::runtime_error("Audio_Init error");
    int audio_rate = 22050; Uint16 audio_format = AUDIO_S16SYS; int audio_channels = 2; int audio_buffers = 4096;

    if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) < 0)
        throw std::runtime_error("Mixer_Init error");

    //Debut du jeu
    Application app = Application();
    app.loop();
    //End
    Mix_CloseAudio();
    TTF_Quit();
    SDL_Quit();
    return 0;
}