//
// Created by derek on 27/10/20.
//

#ifndef SLINGER_SCENE_H
#define SLINGER_SCENE_H

class Scene
{
public:
    virtual ~Scene() { }

    virtual void step() = 0;
};


#endif //SLINGER_SCENE_H
