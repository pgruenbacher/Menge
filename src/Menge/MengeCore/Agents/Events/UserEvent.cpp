#include "./UserEvent.h"
#include "MengeCore/Core.h"
#include "MengeCore/BFSM/FSM.h"
#include "MengeCore/BFSM/State.h"
#include "MengeCore/Agents/SimulatorInterface.h"
#include "Plugins/napoleon/DamageTask.h"
#include "Plugins/napoleon/UserCommandTask.h"
#include "Plugins/napoleon/waypoints/WayPointComponent.h"
#include "Plugins/napoleon/waypoints/WayPointTask.h"
#include "MengeCore/BFSM/GoalSelectors/GoalSelectorIdentity.h"
#include "Plugins/Formations/SteadyFormation.h"
#include "Plugins/Formations/FreeFormation.h"
#include "Plugins/Formations/FormationsComponent.h"

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


Menge::BFSM::State* makeWaypointState(size_t classId);
Menge::BFSM::State* makeFormationState(size_t classId);


void UserEvents::AddWaypoints::perform() const {
  const char* waypointStateName = "WayPoints";
  std::vector< Menge::BFSM::State * >& states = Menge::ACTIVE_FSM->getStates();
  Napoleon::WayPointComponent* wc = 0x0;
  for (const Menge::BFSM::State* state : states) {
    if (state->getType() == waypointStateName &&
      state->getClassId() == groupId) {
        wc = dynamic_cast<Napoleon::WayPointComponent*>(state->getVelComponent());
        if (!wc) continue;
        break;
    }
  }
  if (!wc) {
    Menge::BFSM::State* state = makeWaypointState(groupId);
    wc = dynamic_cast<Napoleon::WayPointComponent*>(state->getVelComponent());
    // otherwise we need to make the state and add it in.
    Menge::ACTIVE_FSM->addNode(state);
    Menge::ACTIVE_FSM->addTask(wc->getTask());
    Menge::ACTIVE_FSM->updateTransitions();
  }

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

void UserEvents::AddFormation::perform() const {
  const char* waypointStateName = "FormState";
  std::vector< Menge::BFSM::State * >& states = Menge::ACTIVE_FSM->getStates();
  Formations::FormationComponent* fc = 0x0;
  for (const Menge::BFSM::State* state : states) {
    if (state->getType() == waypointStateName &&
      state->getClassId() == groupId) {
        fc = dynamic_cast<Formations::FormationComponent*>(state->getVelComponent());
        if (!fc) continue;
        break;
    }
  }
  if (!fc) {
    Menge::BFSM::State* state = makeFormationState(groupId);
    fc = dynamic_cast<Formations::FormationComponent*>(state->getVelComponent());
    // otherwise we need to make the state and add it in.
    Menge::ACTIVE_FSM->addNode(state);
    Menge::ACTIVE_FSM->addTask(fc->getTask());
    Menge::ACTIVE_FSM->updateTransitions();
  }

  std::vector<size_t> weights;
  weights.resize(points.size());
  std::fill(weights.begin(), weights.end(), 1);
  unsigned int borderCount = 0;
  Formations::FormationPtr form = fc->getFormation();
  form->setPoints(points, weights, borderCount);
}

void UserEvents::ToWaypoints::perform() const {
  Napoleon::UserCommandTask* tsk = Napoleon::UserCommandTask::getSingleton();
  Napoleon::UserGroupCommand cmd = tsk->getGroupCommand(groupId);
  cmd.toWaypoints = true;
  tsk->setGroupCommand(groupId, cmd);
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


Menge::BFSM::State* makeWaypointState(size_t classId) {
  using Napoleon::WayPointComponent;
  using Napoleon::Curve2D;
  // godot specific.
  // uses SteadyFormation instead of FreeFormation,
  // which means it won't update mapAgentsToPoitns continuously.
  Menge::BFSM::State* st = new Menge::BFSM::State("Waypoints" + std::to_string(classId), "Waypoints", classId);
  st->setGoalSelector(new Menge::BFSM::IdentityGoalSelector());
  st->setFinal(false);
  std::string resourceName = "curve2D" + std::to_string(classId);
  Resource * rsrc = ResourceManager::getResource( resourceName, &Curve2D::make, Curve2D::LABEL );
  if ( rsrc == 0x0 ) {
    logger << Logger::ERR_MSG << "No resource available.";
    delete st;
    throw ResourceException();
  }
  // std::cout << "FORM  " << st->getName() << std::endl;
  Curve2D * curve = dynamic_cast< Curve2D * >( rsrc );
  WayPointComponent* wComp = new WayPointComponent();
  wComp->setCurve(Napoleon::CurvePtr( curve ));
  st->setVelComponent(wComp);
  return st;
}

Menge::BFSM::State* makeFormationState(size_t classId) {
  // godot specific.
  // uses SteadyFormation instead of FreeFormation,
  // which means it won't update mapAgentsToPoitns continuously.
  using Formations::SteadyFormation;
  using Formations::FormationComponent;
  using Formations::FormationPtr;

  size_t uniqueIndex = classId;
  Menge::BFSM::State* st = new Menge::BFSM::State("Formation" + std::to_string(uniqueIndex), "FormState", classId);
  st->setGoalSelector(new Menge::BFSM::IdentityGoalSelector());
  st->setFinal(false);
  std::string resourceName = "freeFormation" + std::to_string(uniqueIndex);
  Resource * rsrc = ResourceManager::getResource( resourceName, &SteadyFormation::make, SteadyFormation::LABEL );
  if ( rsrc == 0x0 ) {
    logger << Logger::ERR_MSG << "No resource available.";
    delete st;
    throw ResourceException();
  }
  // std::cout << "FORM  " << st->getName() << std::endl;
  SteadyFormation * form = dynamic_cast< SteadyFormation * >( rsrc );
  FormationComponent* fComp = new FormationComponent();
  fComp->setFormation(FormationPtr( form ));
  st->setVelComponent(fComp);
  return st;
}