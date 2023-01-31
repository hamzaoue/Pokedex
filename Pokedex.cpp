#include "Pokedex.h"
//*****************************************************************************
//*************************** POKEMON_SPECIES *********************************
//*****************************************************************************
Pokemon_Species::Pokemon_Species( SDL_Renderer* renderer)
{
	this->renderer_ = renderer;
	this->values_["Seen"] = rand() % 2000;
	this->values_["Owned"] = rand() % 3;
}
///////////////////////////////////////////////////
Pokemon_Species::~Pokemon_Species()
{
	for (auto iter = sprites_.begin(); iter != sprites_.end(); ++iter)
		SDL_DestroyTexture(iter->second);
	sprites_.clear();
	values_.clear();
	datas_.clear();
}
///////////////////////////////////////////////////
int Pokemon_Species::getValue(std::string value) { return  this->values_[value]; }
std::string Pokemon_Species::getData(std::string data){return  this->datas_[data];}
SDL_Texture* Pokemon_Species::getSprite(std::string sprite) { return this->sprites_[sprite];}
///////////////////////////////////////////////////
void Pokemon_Species::setDatas(std::string txtPath)
{
	std::ifstream file(txtPath);
	for (std::string line; std::getline(file, line);)
	{
		int colonPos = line.find(":");
		if (colonPos == std::string::npos)
			printf("Ligne dans le ficher %s invalide : %s\n", txtPath.c_str(), line.c_str());
		else
		{
			std::string entete = line.substr(0, colonPos);
			std::string value = line.substr(colonPos + 1);
			if (value.empty() || entete.empty())
				continue;
			try { this->values_[entete] = stoi(value); }
			catch (const std::invalid_argument& e) { this->datas_[entete] = value; }
		}
	}
	file.close();
}
///////////////////////////////////////////////////
void Pokemon_Species::setSprites(std::string imgPath)
{
	SDL_Surface* surface = IMG_Load(imgPath.c_str());
	if (surface == NULL) { return; }
	this->sprites_["Front"] = this->createTexture(surface, { 0, 0, 96, 96 });
	this->sprites_["FrontShiny"] = this->createTexture(surface, { 0, 97, 96, 96 });
	this->sprites_["Back"] = this->createTexture(surface,{ 97, 0, 80, 80 });
	this->sprites_["BackShiny"] = this->createTexture(surface, { 97, 81, 80, 80 });
	this->sprites_["Icon"] = this->createTexture(surface, { 97, 162, 32, 32 });
	this->sprites_["Icon2"] = this->createTexture(surface, { 130, 162, 32, 32 });
	SDL_FreeSurface(surface);
}
///////////////////////////////////////////////////
SDL_Texture* Pokemon_Species::createTexture(SDL_Surface* surface, SDL_Rect rect)
{
	SDL_Surface* subSurface = SDL_CreateRGBSurface(0, rect.w, rect.h, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	SDL_BlitSurface(surface, &rect, subSurface, nullptr);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(this->renderer_, subSurface);
	SDL_FreeSurface(subSurface);
	return texture;
}
//*****************************************************************************
//******************************* POKEDEX *************************************
//*****************************************************************************
Pokedex::Pokedex(SDL_Renderer* renderer, std::string region,std::string folder)
{
	
	this->renderer_ = renderer;
	this->region_ = region;
	this->lastScroll_ = SDL_GetTicks();
	this->consecutiveScroll_ = this->currentIndex_ = this->selectedBar_ = 0;
	this->plink_ = Mix_LoadWAV("Sounds/Plink.wav");
	this->font_ = TTF_OpenFont("Fonts/Alkhemikal.ttf", 14);
	this->textures_["Background"] = IMG_LoadTexture(this->renderer_, "Images/Background.png");
	this->textures_["CacheBack"] = IMG_LoadTexture(this->renderer_, "Images/CacheBack.png");
	this->textures_["Cache"] = IMG_LoadTexture(this->renderer_, "Images/Cache.png");
	this->textures_["Bar"] = IMG_LoadTexture(this->renderer_, "Images/Bar.png");
	this->textures_["Scroll"] = IMG_LoadTexture(this->renderer_, "Images/Scroll.png");
	this->textures_["Owned"] = IMG_LoadTexture(this->renderer_, "Images/Owned.png");
	this->textures_["Owned2"] = IMG_LoadTexture(this->renderer_, "Images/Owned2.png");
	this->init(folder);
}
/////////////////////////////////////////////
Pokedex::~Pokedex()
{
	for (Pokemon_Species* species: this->species_)
		delete species;
	for (auto& texture : textures_)
		SDL_DestroyTexture(texture.second);
	this->species_.clear();
	this->textures_.clear();
	TTF_CloseFont(this->font_);
	Mix_FreeChunk(this->plink_);
}
///////////////////////////////////////////////////
void Pokedex::draw()
{
	this->drawTexture(this->textures_["Background"], { 0,0,256,192 });
	if (this->species_.size() <= this->currentIndex_ || this->currentIndex_ < 0)return;
	this->drawTexture(this->textures_["CacheBack"], { 109,27 + this->selectedBar_ * 24,25,16 });
	this->drawTexture(this->species_[this->currentIndex_]->getSprite("Icon"), { 105,18 + this->selectedBar_ * 24,32,32 });
	this->drawTexture(this->textures_["Cache"], { 105,18,33,153 });
	this->drawTexture(this->species_[this->currentIndex_]->getSprite(this->shiny_ ? "FrontShiny" : "Front"), { 5,48,96,96 });
	this->drawTexture(this->textures_["Bar"], { 105,24 + 24 * this->selectedBar_,145,21 });
	this->drawTexture(this->textures_["Scroll"], { 251,30 + ((this->currentIndex_ + 1) * 121) / (int)this->species_.size(),5,7 });
	SDL_Color white = { 255,255,255 }, brown = { 194, 159, 143 };
	this->drawText(this->region_ + " Pokedex", { 16,4 }, white, 0);
	this->drawText(this->species_[this->currentIndex_]->getData("Name_Fr"), { 55,35 }, white, 1);
	this->drawText(std::to_string(this->species_[this->currentIndex_]->getValue(this->shiny_ ? "SeenShiny" : "Seen")), { 32,156 }, white, 1);
	this->drawText(std::to_string(this->species_[this->currentIndex_]->getValue(this->shiny_ ? "OwnedShiny" : "Owned")), { 84,156 }, white, 1);
	this->drawText("Seek", { 35,181 }, white, 1);
	this->drawText("Info", { 35 + 32,181 }, white, 1);
	this->drawText("Area", { 35 + 64,181 }, white, 1);
	this->drawText("Cry", { 35 + 96,181 }, white, 1);
	for (int i = 0; i < 6; i++)
	{
		int pokemonNbr = this->currentIndex_ + 1 + i - this->selectedBar_;
		std::string pokemonName = this->species_[pokemonNbr - 1]->getData("Name_Fr");
		char nbr[4]; snprintf(nbr, sizeof(nbr), "%03d", pokemonNbr);
		this->drawText(nbr, { 157,28 + i * 24 }, i == this->selectedBar_ ? brown : white, 0);
		this->drawText(pokemonName, { 180,28 + i * 24 }, i == this->selectedBar_ ? brown : white, 0);
		if (this->species_[pokemonNbr - 1]->getValue("Owned") > 0)
			this->drawTexture(i == this->selectedBar_ ? this->textures_["Owned2"] : this->textures_["Owned"], { 141,29 + i * 24,11,11 });
	}
}
/////////////////////////////////////////////
void Pokedex::drawTexture(SDL_Texture* texture, SDL_Rect bounds)
{
	if (texture == NULL)return;
	SDL_RenderCopy(this->renderer_, texture, NULL, &bounds);
}
/////////////////////////////////////////////
void Pokedex::drawText(std::string text, Point pos, SDL_Color color, bool center)
{
	if (text.empty())return;
	int x = pos.x; int y = pos.y;
	int textWidth, textHeight;
	TTF_SizeText(this->font_, text.c_str(), &textWidth, &textHeight);
	if (center){x = x - (textWidth / 2);y = y - (textHeight / 2);}
	SDL_Surface* textSurface = TTF_RenderText_Blended(this->font_, text.c_str(), color);
	if (textSurface == NULL)return;
	SDL_Rect textLocation = { x, y, textWidth, textHeight };
	SDL_Texture* texture = SDL_CreateTextureFromSurface(this->renderer_, textSurface);
	SDL_RenderCopy(this->renderer_, texture, NULL, &textLocation);
	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(texture);

}
/////////////////////////////////////////////
void Pokedex::keyDown(SDL_Keycode key)
{
	if (SDL_GetTicks() - this->lastScroll_ > 400)
		this->consecutiveScroll_ = 0;
	switch (key)
	{
	case SDLK_SPACE:this->shiny_ = this->shiny_ ^ 1; break;
	case SDLK_UP:
		if (this->currentIndex_ > 0 && SDL_GetTicks() - this->lastScroll_ > std::max(300 - this->consecutiveScroll_ * 50, 0))
		{
			this->currentIndex_--;
			Mix_HaltChannel(0); Mix_PlayChannel(0, this->plink_, 0);
			this->lastScroll_ = SDL_GetTicks();
			this->consecutiveScroll_++;
			if (this->selectedBar_ > 0)
				this->selectedBar_--;
		}break;
	case SDLK_DOWN:
		if (this->currentIndex_ < this->species_.size() - 1 &&
			SDL_GetTicks() - this->lastScroll_ > std::max(300 - (this->consecutiveScroll_ * 50), 0))
		{
			this->currentIndex_++;
			Mix_HaltChannel(0); Mix_PlayChannel(0, this->plink_, 0);
			this->lastScroll_ = SDL_GetTicks();
			this->consecutiveScroll_++;
			if (this->selectedBar_ < 5)
				this->selectedBar_++;
		}break;
	}
}
///////////////////////////////////////////////////
void Pokedex::init(std::string folder)
{
	if (!std::filesystem::exists(folder))
	{printf("Dossier introuvable : %s", folder.c_str()); return;}
	//Parcours des fichiers
	for (auto& entry : std::filesystem::directory_iterator(folder))
	{
		std::string path = entry.path().string();
		std::string fileName = path.substr(path.find_last_of("\\/") + 1);
		std::string extension = path.substr(path.find_last_of(".") + 1);
		// Vérifie si le nom de fichier est valide
		int dashPos = fileName.find("-");
		if (dashPos == std::string::npos) {
			printf("Nom de fichier invalide: %s\n", fileName.c_str());
			continue;
		}
		try//Essaie de récupérer le numéro de pokemon
		{
			int pokemonNbr = std::stoi(fileName.substr(0, dashPos));
			if (pokemonNbr <= 0 || pokemonNbr >= 1000)
				printf("Numero trop grand ou trop faible pour le fichier %s\n", fileName.c_str());
			else
			{
				// Agrandit le pokedex si besoin
				while (this->species_.size() < pokemonNbr)
					this->species_.push_back(new Pokemon_Species(this->renderer_));
				// Ajoute les données associées
				if (extension == "png")
					this->species_[pokemonNbr - 1]->setSprites(path);
				else if (extension == "txt")
					this->species_[pokemonNbr - 1]->setDatas(path);
				else
					printf("Extension de fichier non prise en charge: %s\n", extension.c_str());
			}
		}
		catch (const std::invalid_argument& e) {printf("Nom de fichier invalide: %s\n", fileName.c_str());}
		catch (const std::out_of_range& e) {printf("Numero trop grand pour le fichier %s\n", fileName.c_str());}
	}
}
//*****************************************************************************
//******************************** APPLICATION ********************************
//*****************************************************************************
Application::Application()
{
	this->window_ptr_ = SDL_CreateWindow("Windows", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, frame_width, frame_height, SDL_WINDOW_RESIZABLE);
	this->renderer_ = SDL_CreateRenderer(this->window_ptr_, -1, 2 | 4);
	if (this->window_ptr_ == NULL|| this->renderer_ == NULL){ exit(1); }
	//this->pokedex_ = new Pokedex(this->renderer_, "Johto", "2-Johto");
	this->pokedex_ = new Pokedex(this->renderer_, "Kanto", "1-Kanto");
}
/////////////////////////////////////////////
Application::~Application()
{
	delete pokedex_;
	SDL_DestroyWindow(this->window_ptr_);
	SDL_DestroyRenderer(this->renderer_);
}
/////////////////////////////////////////////
void Application::loop()
{
	while (1)
	{
		//start mesure update duration
		int startTimeUpdate = SDL_GetTicks();
		//Events
		this->processEvents();
		//Repaint
		SDL_RenderClear(this->renderer_);
		this->pokedex_->draw();
		SDL_RenderPresent(this->renderer_);
		//stop mesure update duration
		int updateDuration = SDL_GetTicks() - startTimeUpdate;
		//wait
		SDL_Delay(std::max(0, 1000 / FPS - updateDuration));
		printf("%s", SDL_GetError());
	}
}
/////////////////////////////////////////////
void Application::processEvents()
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
		case SDL_QUIT:exit(0); break;
		case SDL_KEYDOWN:this->pokedex_->keyDown(e.key.keysym.sym); break;
		}
	}
}