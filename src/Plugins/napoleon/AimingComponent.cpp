// #include "MengeCore/BFSM/VelocityComponents/VelCompGoal.h"
#include "AimingComponent.h"
#include "MengeCore/resources/Resource.h"
#include "MengeCore/Core.h"
#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/Agents/PrefVelocity.h"
#include "MengeCore/BFSM/Goals/Goal.h"
#include "MengeCore/Runtime/os.h"
#include "NearestEnemTask.h"
#include "MengeCore/Agents/SimulatorInterface.h"
#include "AimingTask.h"


namespace Napoleon {
  using Menge::logger;

  /////////////////////////////////////////////////////////////////////
  //                   Implementation of GoalVelComponent
  /////////////////////////////////////////////////////////////////////

  const std::string AimingComponent::NAME = "aiming_component";


  /////////////////////////////////////////////////////////////////////

  void AimingComponent::onEnter(BaseAgent* agent) {
    // dynamically resize angles based on number of agents
    // without worrying about the sim->getNumAgents.
    if (agent->_id > _angles.size()) {
      _angles.resize(agent->_id, defaultAngle);
    }
    // _lock.lockWrite();
    //   // _formation->addAgent(agent);
    // _agents[agent->_id] = agent;
    // _lock.releaseWrite();
  }

  void AimingComponent::onExit(BaseAgent* agent) {
    // _lock.lockWrite();
    //   // _formation->addAgent(agent);
    // _agents[agent->_id] = agent;
    // _lock.releaseWrite();
  }

  /////////////////////////////////////////////////////////////////////

  AimingComponent::AimingComponent() : VelComponent() {
  }


  const NearAgent AimingComponent::getTargetEnem(const BaseAgent* agent) const {
    AimingTask* task = AimingTask::getSingleton();
    NearAgent d(100, 0x0);
    task->getTarget(agent, d);
    return d;
  }

  /////////////////////////////////////////////////////////////////////

  void AimingComponent::setPrefVelocity( const BaseAgent * agent, const Goal * goal,
                      PrefVelocity & pVel ) const {
    // bool modify = agent->_nearEnems.size() > 0;
    // Vector2 target(0.f, 0.f);
    // if ( !modify ) {
    //   // pVel.setSpeed(agent->_prefSpeed);
    //   // Vector2 returnToOrig = (agent->_pos) * -1;
    //   // returnToOrig.normalize();
    //   // pVel.setSingle(returnToOrig);
    //   // std::cout << "no modify! fix! " << agent->_id << "GOAL" << goal << " " << std::endl;
    //   // return setIdleVelocity(agent, goal, pVel);
    //   goal->setDirections(agent->_pos, agent->_radius, pVel);
    //   // goal.
    //   return;
    // }
    // const NearestEnemData targetEnem = getTargetEnem(agent);
    // if (targetEnem.agent == 0x0) return;
    // // target = targetEnem.agent->_pos;
    // float distSq = targetEnem.distanceSquared;
  }

  Task * AimingComponent::getTask(){
    return AimingTask::getSingleton();
  };


  /////////////////////////////////////////////////////////////////////
  AimingComponentFactory::AimingComponentFactory() : VelCompFactory() {
    // _methodID = _attrSet.addStringAttribute("method", true /* required */);
    // _slowToArriveID = _attrSet.addBoolAttribute("slow_to_arrive", false, false);
  }

  /////////////////////////////////////////////////////////////////////

  bool AimingComponentFactory::setFromXML( VelComponent * component, TiXmlElement * node,
                     const std::string & behaveFldr ) const {
      AimingComponent * nearestEnemComp = dynamic_cast<AimingComponent *>(component);
    if ( ! VelCompFactory::setFromXML( component, node, behaveFldr ) ) return false;

      assert( nearestEnemComp != 0x0 &&
      "Trying to set property component properties on an incompatible object" );



    return true;
  }
} // namespace Formations
