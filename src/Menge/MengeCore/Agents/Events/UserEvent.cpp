#include "./UserEvent.h"
#include "MengeCore/Core.h"
#include "MengeCore/BFSM/FSM.h"
#include "MengeCore/BFSM/State.h"
#include "MengeCore/Agents/SimulatorInterface.h"
#include "Plugins/napoleon/DamageTask.h"
#include "Plugins/napoleon/UserCommandTask.h"
#include "Plugins/napoleon/waypoints/WayPointComponent.h"
#include "Plugins/napoleon/waypoints/WayPointTask.h"

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
        Napoleon::WayPointComponent* wc = dynamic_cast<Napoleon::WayPointComponent*>(state->getVelComponent());
        if (!wc) continue;
        Napoleon::WayPointsTask* tsk = wc->get_task();
        Napoleon::CurvePtr curve = wc->getCurve();
        tsk->reset();
        curve->clear();
        for (const Menge::Math::Vector2& pt : points) {
          curve->addPoint(pt);
        }
        if (autoStart) {
          tsk->setCanPerform(true);
        }
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
