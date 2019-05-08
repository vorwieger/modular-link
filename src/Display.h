#pragma once

#include "State.h"
#include "Max7219.h"

class Display : public StateObserver {

  public:

    Display(State& /*state*/);
    ~Display();

    void stateChanged() override;

  private:

    State& m_state;
    Max7219 max7219;

};