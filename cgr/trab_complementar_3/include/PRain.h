#ifndef PRAIN_HPP
#define PRAIN_HPP

// Alexandre Maros
// pridexs.com
// CGR - Computacao Grafica
//
// PRain
// Class to hold the Rain Particles and Logic.mpo
//

#include <vector>
#include <SDL2/SDL.h>
#include <GL/glu.h>

class PRain
{
    public:
        PRain(int spawnPointX, int spawnPointY, 
            float width, float height, float maxHeight,
             float velocity_y, unsigned int nParticles);

        void handleEvent( SDL_Keysym *keysym );

        void render();
        void update(float dt);
        void resetParticles();

    private:
        void initParticles();
        void updateParticles();

        float mWidth, mHeight, mMaxHeight;
        float mVelocity_y, mVelocity_x;
        float mRainAngle;

        int mSpawnPointX, mSpawnPointY;
        int mNumParticles;

        struct Particle {
            float x1, y1, veloc_y;
            float x2, y2, veloc_x;
        };

        std::vector<struct Particle> mParticles;
};

#endif