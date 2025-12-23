#pragma once
#include "plant/PeaShooter.h"

class ReaPeater : public PeaShooter {
public:
    CREATE_FUNC(ReaPeater);
    virtual bool init() override;

protected:
    virtual void attack() override;
};