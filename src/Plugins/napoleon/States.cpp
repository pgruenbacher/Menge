#include "./States.h"
#include "MengeCore/Core.h"
#include "MengeCore/BFSM/FSM.h"
#include "MengeCore/BFSM/State.h"
#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/Agents/SimulatorInterface.h"

MENGE_STATE get_state_from_string(const std::string& name) {
  // std::cout << "NAME " << name << std::endl;
  if (name == "Attack") {
    return ATTACK;
  } else if (name == "Idle") {
    return IDLE;
  } else if (name == "Dying") {
    return DYING;
  } else if (name == "Dead") {
    return DEAD;
  } else if (name == "TooManyEnemyNear") {
    return WITHDRAWING;
  } else if (name == "AdvancingEnemy") {
    return ADVANCING;
  } else if (name == "Firing") {
    return FIRING;
  } else if (name == "Aiming") {
    return AIMING;
  } else if (name == "Bracing") {
    return BRACING;
  } else if (name == "CavAttackCharge") {
    return ATTACK;
  }
  return NONE;
}

MENGE_STATE Napoleon::getState(size_t agent_id) {
    if (!Menge::ACTIVE_FSM) return NONE;
    Menge::BFSM::State* st = Menge::ACTIVE_FSM->getCurrentState(agent_id);
    // std::cout << " ST " << st->getName() << std::endl;
    return get_state_from_string(st->getName());
}

const char* Napoleon::getStateString(size_t agent_id) {
  // return "None";
  if (!Menge::ACTIVE_FSM) return "None";
  Menge::BFSM::State* st = Menge::ACTIVE_FSM->getCurrentState(agent_id);
  // std::cout << " ST " << st->getName() << std::endl;
  return (st->getName().c_str());
}
// void Napoleon::debugState(size_t agent_id) {
//     if (!Menge::ACTIVE_FSM) return;
//     Menge::BFSM::State* st = Menge::ACTIVE_FSM->getCurrentState(agent_id);
//     Menge::Agents::BaseAgent* ba = Menge::SIMULATOR->getAgent(agent_id);
//     std::cout << " State " << st->getName() << "ORIENT " << ba->_orient << " " << ba->_velPref.getPreferred()  << " " << ba->_velPref.getTarget() << std::endl;
//     // return get_state_from_string(st->getName());
// }
