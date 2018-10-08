#include "./UserEvent.h"
#include "MengeCore/Core.h"
#include "MengeCore/Agents/SimulatorInterface.h"
#include "Plugins/napoleon/DamageTask.h"

void UserEvents::ProjectileCollision::perform() const {
  Napoleon::DamageTask* dt = Napoleon::DamageTask::getSingleton();
  dt->adjustHealth(agentId, damage);
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
