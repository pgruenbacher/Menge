#include "./UserEvent.h"
#include "MengeCore/Core.h"
#include "MengeCore/Agents/SimulatorInterface.h"
#include "Plugins/napoleon/DamageTask.h"
#include "Plugins/napoleon/UserCommandTask.h"

void UserEvents::ProjectileCollision::perform() const {
  Napoleon::DamageTask* dt = Napoleon::DamageTask::getSingleton();
  dt->adjustHealth(agentId, damage);
}

void UserEvents::ToFormation::perform() const {
  Napoleon::UserCommandTask* tsk = Napoleon::UserCommandTask::getSingleton();
  Napoleon::UserGroupCommand cmd;
  cmd.move_to_formation = true;
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
