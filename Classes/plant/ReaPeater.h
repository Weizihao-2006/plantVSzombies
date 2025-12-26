#pragma once
#include "plant/PeaShooter.h"

class RePeater : public PeaShooter {
public:
    CREATE_FUNC(RePeater);
    virtual bool init() override;

protected:
    virtual void attack() override;
};