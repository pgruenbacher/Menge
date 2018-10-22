#include "./UserEvent.h"
#include "MengeCore/Core.h"
#include "MengeCore/BFSM/FSM.h"
#include "MengeCore/BFSM/State.h"
#include "MengeCore/Agents/SimulatorInterface.h"
#include "Plugins/napoleon/DamageTask.h"
#include "Plugins/napoleon/UserCommandTask.h"

void UserEvents::ProjectileCollision::perform() const {
  Napoleon::DamageTask* dt = Napoleon::DamageTask::getSingleton();
  dt->adjustHealth(agentId, damage);
}

void UserEvents::CanFire::perform() const {
  Napoleon::UserCommandTask* tsk = Napoleon::UserCommandTask::getSingleton();
  Napoleon::UserGroupCommand cmd = tsk->getGroupCommand(groupId);
  cmd.canFire = true;
  tsk->setGroupCommand(groupId, cmd);
}

void UserEvents::AddWaypoints::perform() const {
  const char* waypointStateName = "WayPoints";
  std::vector< Menge::BFSM::State * >& states = Menge::ACTIVE_FSM->getStates();
  for (const Menge::BFSM::State* state : states) {
    if (state->getType() == waypointStateName &&
      state->getClassId() == groupId) {
    }
  }
}

void UserEvents::ToFormation::perform() const {
  Napoleon::UserCommandTask* tsk = Napoleon::UserCommandTask::getSingleton();
  Napoleon::UserGroupCommand cmd;
  cmd.moveToFormation = true;
  tsk->setGroupCommand(groupId, cmd);
  // const size_t NUM_AGENT = Menge::SIMULATOR->getNumAgents();
  // for ( size_t i = 0; i < NUM_AGENT; ++i ) {
  //   const BaseAgent * agt = Menge::SIMULATOR->getAgent( i );
  //   //if this agent is in my box
  //   if (agt->_class == groupId) {
  //     agt->
  //   }
  // }
}

void UserEvents::addUserEvent(const UserEvent* evt) {
  _userEvents.push_back(evt);
}


void UserEvents::perform() {
  for (const UserEvent* evt : _userEvents) {
    evt->perform();
    delete evt;
  }
  _userEvents.clear();
}
