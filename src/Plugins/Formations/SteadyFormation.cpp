#include "SteadyFormation.h"

namespace Formations {
  using Menge::logger;
  using Menge::Logger;
  using Menge::Resource;
  using Menge::ResourceException;
  using Menge::ResourceManager;
  using Menge::Agents::BaseAgent;
  using Menge::Agents::PrefVelocity;
  using Menge::BFSM::FSM;
  using Menge::BFSM::VelModFatalException;
  using Menge::Math::Vector2;
  using Menge::Math::sqr;

  const std::string SteadyFormation::LABEL("steady-formation");

  void SteadyFormation::mapAgentsToFormation(const FSM * fsm) {
    // only updates if either a) it hasn't been set the first time or b)
    // explictly caleld to "reset" the mapping of agents to points. e.g. when
    // formation flips more than 180 deg or game timer or num agents changes
    // drastically.
    if (_shouldUpdate || getFormationPointAgent().size() == 0) {
      _shouldUpdate = false;
      FreeFormation::mapAgentsToFormation(fsm);
    } else {
      return;
    }
  }
}