#pragma once

#include "Engine.h"
#include "Max7219.h"

class Display : public StateObserver {

  public:

    Display(Engine& /*engine*/);
    ~Display();

    void stateChanged() override;

  private:

    Engine& m_engine;
    Max7219 max7219;

};