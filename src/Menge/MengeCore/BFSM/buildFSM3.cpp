/*

License

Menge
Copyright © and trademark ™ 2012-14 University of North Carolina at Chapel Hill.
All rights reserved.

Permission to use, copy, modify, and distribute this software and its
documentation for educational, research, and non-profit purposes, without fee,
and without a written agreement is hereby granted, provided that the above
copyright notice, this paragraph, and the following four paragraphs appear in
all copies.

This software program and documentation are copyrighted by the University of
North Carolina at Chapel Hill. The software program and documentation are
supplied "as is," without any accompanying services from the University of North
Carolina at Chapel Hill or the authors. The University of North Carolina at
Chapel Hill and the authors do not warrant that the operation of the program
will be uninterrupted or error-free. The end-user understands that the program
was developed for research purposes and is advised not to rely exclusively on
the program for any reason.

IN NO EVENT SHALL THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL OR THE AUTHORS
BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR
CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS
SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF NORTH CAROLINA AT
CHAPEL HILL OR THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL AND THE AUTHORS SPECIFICALLY
DISCLAIM ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE AND ANY STATUTORY
WARRANTY OF NON-INFRINGEMENT. THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
BASIS, AND THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL AND THE AUTHORS HAVE
NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
MODIFICATIONS.

Any questions or comments should be sent to the authors {menge,geom}@cs.unc.edu

*/

/*!
 *  @file buildFSM.cpp
 *  @brief  Functionality to realize Behavior FSM from configuration.
 */

#include "MengeCore/BFSM/FSMDescrip.h"

#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/Agents/Elevations/Elevation.h"
#include "MengeCore/Agents/Events/Event.h"
#include "MengeCore/Agents/Events/EventException.h"
#include "MengeCore/Agents/Events/EventSystem.h"
#include "MengeCore/Agents/SimulatorInterface.h"
#include "MengeCore/Agents/SimulatorState.h"
#include "MengeCore/Agents/SpatialQueries/SpatialQuery.h"
#include "MengeCore/BFSM/FSM.h"
#include "MengeCore/BFSM/GoalSelectors/GoalSelector.h"
#include "MengeCore/BFSM/GoalSelectors/GoalSelectorIdentity.h"
#include "MengeCore/BFSM/GoalSelectors/GoalSelectorShared.h"
#include "MengeCore/BFSM/State.h"
#include "MengeCore/BFSM/Transitions/Transition.h"
#include "MengeCore/BFSM/VelocityComponents/VelCompConst.h"
#include "MengeCore/BFSM/fsmCommon.h"
#include "MengeCore/Core.h"
#include "MengeCore/resources/NavMesh.h"
#include "buildFSM3.h"

namespace Menge {

namespace BFSM {
/////////////////////////////////////////////////////////////////////
//                   Implementation of buildFSM
/////////////////////////////////////////////////////////////////////

void initGlobalVariables(Agents::SimulatorInterface* sim) {
  // Acquire the spatial query instance
  SPATIAL_QUERY = sim->getSpatialQuery();

  // Global Simulator interface
  SIMULATOR = sim;
}

bool initializeAgents(FSM* fsm, FSMDescrip& fsmDescrip,
                      Agents::SimulatorInterface* sim, bool VERBOSE) {
  const size_t AGT_COUNT = sim->getNumAgents();
  // we'll need this iterator later
  std::vector<VelModifier*>::iterator vItr;
  //  5. Initialize all agents
  if (VERBOSE) logger << Logger::INFO_MSG << "Initializing agents:\n";
  Agents::SimulatorState* initState = sim->getInitialState();

  for (size_t a = 0; a < AGT_COUNT; ++a) {
    Agents::BaseAgent* agt = sim->getAgent(a);
    // update current state to class-appropriate value
    const std::string stateName = initState->getAgentState(agt->_id);

    // std::map< std::string, size_t >::iterator stateIDItr =
    //   stateNameMap.find( stateName );
    if (!fsm->getNode(stateName)) {
      logger << Logger::ERR_MSG << "Agent " << agt->_id;
      logger << " requested to start in an unknown state: " << stateName << ".";
      delete fsm;
      return false;
    }
    // size_t stateID = stateIDItr->second;

    // initialize velocity to preferred velocity
    State* cState = fsm->getNode(stateName);
    if (VERBOSE) {
      logger << Logger::INFO_MSG << "Agent " << agt->_id << " starts in ";
      logger << cState->getName() << ".";
    }
    fsm->setCurrentState(agt, stateName);
    cState->enter(agt);
    // TODO: Restore support for defining inital velocity state: zero or
    // preferred
    agt->_vel.set(Vector2(0.f, 0.f));

    // register the agent for all vel modifiers
    vItr = fsm->getVelModifiers().begin();
    // TODO: replace global vel mod initalizer
    for (; vItr != fsm->getVelModifiers().end(); ++vItr) {
      (*vItr)->registerAgent(agt);
    }
  }
  return true;
}

bool initializeTasks(FSM* fsm, FSMDescrip& fsmDescrip,
                      Agents::SimulatorInterface* sim, bool VERBOSE) {

  fsm->collectTasks();
  for (std::list<Task*>::iterator itr = fsmDescrip.getTasks().begin();
       itr != fsmDescrip.getTasks().end(); ++itr) {
    fsm->addTask((*itr));
  }
  fsmDescrip.getTasks().clear();

  // spatial query and elevation tasks
  fsm->addTask(SPATIAL_QUERY->getTask());
  if (sim->getElevationInstance()) {
    // NOTE: The elevation instance is null because none were specified.
    //  Eventually, the default elevation will be set.
    // HOWEVER, if the default ever changes such that it requires a task,
    //  this won't catch it!!!  So, make sure the default never requires
    //  a task.
    fsm->addTask(sim->getElevationInstance()->getTask());
  }

  logger << Logger::INFO_MSG << "There are " << fsm->getTaskCount();
  logger << " registered tasks.\n";
  fsm->doTasks();
  return true;
}

bool initializeStates(FSM* fsm, FSMDescrip& fsmDescrip,
                      Agents::SimulatorInterface* sim, bool VERBOSE) {

  //  1. Create states
  //    a. Add velocity components and actions
  //    b. add to fsm
  // we'll need this iterator later
  std::vector<VelModifier*>::iterator vItr;

  std::list<StateDescrip*>::const_iterator sItr =
      fsmDescrip.getStates().begin();
  for (; sItr != fsmDescrip.getStates().end(); ++sItr) {
    StateDescrip* sData = *sItr;
    State* s = fsmDescrip.addState(sData);

    if (s == 0x0) {
      logger << Logger::ERR_MSG << "Error creating state!";
      delete fsm;
      return false;
    }
    if (VERBOSE) {
      logger << Logger::INFO_MSG << "\tAdding state: " << s->getName();
      logger << "(" << s->getID() << ")\n";
    }

    // State's goal selector
    GoalSelector* gs = sData->_goalSelector;
    if (gs == 0x0) {
      logger << Logger::WARN_MSG << "The state " << sData->_name;
      logger << " doesn't specify a goal selector.  "
                "The identity goal selector will be used.";
      gs = new IdentityGoalSelector();
    }

    try {
      gs->setGoalSet(
          fsm->getGoalSets());  // possibly throws GoalSelectorException
      s->setGoalSelector(gs);   // possibly throws GoalSelectorException
    } catch (GoalSelectorException) {
      logger << Logger::ERR_MSG
             << "Problem initializing the goal selector for "
                "the state "
             << s->getName() << ".";
      delete fsm;
      return false;
    }
    sData->_goalSelector = 0x0;

    // construct each velocity component
    if (sData->_velComponent == 0x0) {
      logger << Logger::WARN_MSG << "The state " << sData->_name
             << " doesn't "
                "specify a velocity component.  The zero velocity component "
                "will be used.";
      s->setVelComponent(new ZeroVelComponent());
    } else {
      s->setVelComponent(sData->_velComponent);
      sData->_velComponent = 0x0;
    }

    // transfer each action
    std::list<Action*>::iterator aItr = sData->_actions.begin();
    for (; aItr != sData->_actions.end(); ++aItr) {
      s->addAction(*aItr);
    }
    sData->_actions.clear();

    // transfer velocity modifiers from the state description
    vItr = sData->_velModifiers.begin();
    for (; vItr != sData->_velModifiers.end(); ++vItr) {
      s->addVelModifier(*vItr);
    }
    sData->_velModifiers.clear();

    // Set start node
    fsm->addNode(s);
    // stateNameMap[ sData->_name ] = stateID;
  }
  return true;
}

bool initializeTransitions(FSM* fsm, FSMDescrip& fsmDescrip,
                      Agents::SimulatorInterface* sim, bool VERBOSE) {
  if (VERBOSE) {
    logger << Logger::INFO_MSG << "There are "
           << fsmDescrip.getTransitions().size();
    logger << " transitions\n";
  }
  std::map<std::string, std::list<Transition*> >::iterator stItr =
      fsmDescrip.getTransitions().begin();
  for (; stItr != fsmDescrip.getTransitions().end(); ++stItr) {
    const std::string fromName = stItr->first;
    std::list<Transition*>& tList = stItr->second;

    // Determine if the origin state is valid
    if (!fsm->getNode(fromName)) {
      logger << Logger::ERR_MSG << "Transition with invalid from node name: ";
      logger << fromName << ".";
      delete fsm;
      return false;
    }

    // Try to connect the transitions to the destination(s)
    std::list<Transition*>::iterator tItr = tList.begin();
    for (; tItr != tList.end(); ++tItr) {
      Transition* t = *tItr;
      if (!t->connectStates(fsm->getStateNameMap())) {
        delete fsm;
        return false;
      }
      // fsm->addTransition( stateNameMap[ fromName ], t );
      fsm->getNode(fromName)->addTransition(t);
    }
    tList.clear();
  }
  fsmDescrip.getTransitions().clear();
  // fsmDescrip.getStateNameMap().clear();
  return true;
}

bool connectSharedGoalSelectors(FSM* fsm, FSMDescrip& fsmDescrip,
                      Agents::SimulatorInterface* sim, bool VERBOSE) {
  // Connect all shared goal selectors
  // std::map< std::string, size_t >::iterator stateItr = stateNameMap.begin();
  // for ( ; stateItr != stateNameMap.end(); ++stateItr ) {
  for (State* state : fsm->getNodes()) {
    std::string stateName = state->getName();
    // std::string stateName = stateItr->first;
    // size_t stateID = stateItr->second;
    // State * state = fsm->getNode( stateID );
    SharedGoalSelector* gs =
        dynamic_cast<SharedGoalSelector*>(state->getGoalSelector());
    if (gs) {
      if (!fsm->getNode(gs->getStateName()) == 0) {
        logger << Logger::ERR_MSG
               << "Found shared goal selector defined on line ";
        logger << gs->getLineNo()
               << ", but unable to locate state with the provided "
                  "name: \""
               << gs->getStateName() << "\".";
        delete fsm;
        return false;
      }
      State* src = fsm->getNode(gs->getStateName());
      if (dynamic_cast<SharedGoalSelector*>(src->getGoalSelector())) {
        logger << Logger::ERR_MSG << "Shared goal selector defined on line ";
        logger << gs->getLineNo()
               << " references a state with a shared goal.  The "
                  "source state must have a full goal selector definition.";
        delete fsm;
        return false;
      }
      state->clearGoalSelector();
      GoalSelector* srcGS = src->getGoalSelector();
      srcGS->setPersistence(true);
      state->setGoalSelector(srcGS);
    }
  }
  return true;
}

FSMBuilder::FSMBuilder() {
}

void FSMBuilder::add_state(State* state) {
  // Menge::BFSM::State* st = new Menge::BFSM::State("SquareFormation-formation-state", "Class", 1);
  // st->setFinal(false);
  // st->setGoalSelector(new Menge::BFSM::IdentityGoalSelector());
  // st->setVelComponent(new Menge::BFSM::ZeroVelComponent());
  _states.push_back(state);
}

FSMBuilder::~FSMBuilder() {
  // std::cout << "DELETE FSM BUILDER " << std::endl;
}

void copyToFSM(FSMDescrip& fsmDescrip, FSM* fsm) {

  // we'll need this iterator later
  std::vector<VelModifier*>::iterator vItr;
  // Copy the GoalSets
  fsm->getGoalSets().clear();
  fsm->getGoalSets().insert(fsmDescrip.getGoalSets().begin(),
                            fsmDescrip.getGoalSets().end());

  fsmDescrip.getGoalSets().clear();
  // copy over the velocity modifiers
  vItr = fsmDescrip.getVelModifiers().begin();
  // TODO: replace global vel mod initalizer
  for (; vItr != fsmDescrip.getVelModifiers().end(); ++vItr) {
    fsm->addVelModifier(*vItr);
  }
  fsmDescrip.getVelModifiers().clear();
}

bool FSMBuilder::add_dynamic_states(FSM* fsm) {
  for (State* state : _states) {
    fsm->addNode(state);
  }
  return true;
}

FSM* FSMBuilder::build(FSMDescrip& fsmDescrip, Agents::SimulatorInterface* sim,
               bool VERBOSE) {
  initGlobalVariables(sim);
  FSM* fsm = new FSM(sim);
  copyToFSM(fsmDescrip, fsm);

  if (!initializeStates(fsm, fsmDescrip, sim, VERBOSE)) {
    return 0x0;
  }

  if (!add_dynamic_states(fsm)) {
    // dynamic states should have their transitions already connected.
    // if they have goal selectors those should be set as well.
    return 0x0;
  }

  if (!connectSharedGoalSelectors(fsm, fsmDescrip, sim, VERBOSE)) {
    return 0x0;
  }

  if (!initializeTransitions(fsm, fsmDescrip, sim, VERBOSE)) {
    return 0x0;
  }

  // add all extra dynamic states beforehand since
  // fsm.h is iterating over all states to collect transitions!!
  if (!initializeTasks(fsm, fsmDescrip, sim, VERBOSE)) {
    return 0x0;
  }

  // add all extra dynamic states beforehand!!
  if (!initializeAgents(fsm, fsmDescrip, sim, VERBOSE)) {
    return 0x0;
  }

  ACTIVE_FSM = fsm;

  return fsm;
}

}  // namespace BFSM
}  // namespace Menge
