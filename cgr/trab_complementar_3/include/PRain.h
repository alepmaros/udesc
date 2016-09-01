#ifndef PRAIN_HPP
#define PRAIN_HPP

// Alexandre Maros
// pridexs.com
// CGR - Computacao Grafica

//
// mWidth - Determinar o tamanho da area em que a chuva podera
// aparecer
//
// mHeight - A altura que a chuva ira cair (ao percorrer x unidades
// ela some, em que x é o height)
//
// mNParticles - Numero de particulas máximas que podem existir ao
// mesmo tempo
//

#include <vector>
#include <list>
#include <SDL2/SDL.h>
#include <GL/glu.h>

class PRain
{
    public:
        PRain(int spawnPointX, int spawnPointY, 
            float width, float height, float velocity_y, unsigned int nParticles);

        void handleEvent( SDL_Event& e );

        void render();
        void update(float dt);

    private:
        float mWidth, mHeight;
        float mVelocity_y;

        int mSpawnPointX, mSpawnPointY;
        int mNParticles;

        struct Particle {
            float x, y, veloc_y;
            float distanceTraveled;
        };

        std::list<struct Particle> mParticles;
};

#endif