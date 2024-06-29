#include "RayCaster.hpp"

int main(int nArgs, char **args)
{
    HighResClock hrc;
    float dt = 0.f;
    RayCaster rc(nArgs, args);

    while (rc.isRunning())
    {
        hrc.restart();
        rc.update(dt);
        rc.draw();
        dt = hrc.getElapsedTimeAsSeconds();
    }
    return rc.close();
}