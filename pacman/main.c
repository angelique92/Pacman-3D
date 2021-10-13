#include <SDL/SDL.h>
#include <iostream>


int main(int argc, char **argv)
{
   // Notre fenêtre et le clavier

   SDL_Surface *fenetre(0);
   SDL_Event evenements = {0};
   bool terminer(false);


   // Initialisation de la SDL

   if(SDL_Init(SDL_INIT_VIDEO) < 0)
   {
       std::cout << "Erreur lors de l'initialisation de la SDL : " << SDL_GetError() << std::endl;
       SDL_Quit();

       return -1;
   }


   // Création de la fenêtre

   fenetre = SDL_SetVideoMode(800, 600, 32, SDL_HWSURFACE);


   // Boucle principale

   while(!terminer)
   {
        SDL_WaitEvent(&evenements);

        if(evenements.type == SDL_QUIT)
            terminer = true;
   }


   // On quitte la SDL

   SDL_Quit();

   return 0;
}
