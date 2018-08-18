#include "MengeCore/BFSM/State.h"
#include "MengeCore/BFSM/FSMDescrip.h"
#include "MengeCore/Agents/SimulatorInterface.h"


#ifndef _FSM_BUILDER_H_
#define _FSM_BUILDER_H_

namespace Menge {

namespace BFSM {
  class FSMBuilder {
  public:
    FSMBuilder();
    ~FSMBuilder();
    FSM* build(FSMDescrip& fsmDescrip, Agents::SimulatorInterface* sim,
               bool VERBOSE);
    bool add_dynamic_states(FSM* fsm);
    void add_state(State * s);
  protected:

      /*!
       *  @brief    The states in the BFSM.
       */
      std::vector< State * >  _states;
  };
}
}

#endif  // _FSM_BUILDER_H_
