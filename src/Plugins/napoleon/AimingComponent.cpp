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

  const std::string AimingComponent::NAME = "aiming";

  // void setDirections(const Vector2 & q,  Agents::PrefVelocity & directions) {
  //   Vector2 disp = _position - q;
  //   const float distSq = absSq( disp );
  //   Vector2 dir;
  //   if ( distSq > 1e-8 ) {
  //     // Distant enough that I can normalize the direction.
  //     dir.set( disp / sqrtf( distSq ) );
  //   } else {
  //     dir.set( 0.f, 0.f );
  //   }
  //   directions.setSingle(dir);
  //   directions.setTarget(_position);
  // }
  /////////////////////////////////////////////////////////////////////

  void AimingComponent::onEnter(BaseAgent* agent) {
    // dynamically resize angles based on number of agents
    // without worrying about the sim->getNumAgents.
    // _lock.lockWrite();
    // if (agent->_id > _angles.size()) {
    //   _angles.resize(agent->_id, defaultAngle);
    //   // _hold_positions[agent->_id] = agent->_pos;
    // }
    // //   // _formation->addAgent(agent);
    // // _agents[agent->_id] = agent;
    // _lock.releaseWrite();
    AimingTask* task = AimingTask::getSingleton();
    // NearAgent d(100, 0x0);
    return task->addAgent(agent->_id);

  }

  void AimingComponent::onExit(BaseAgent* agent) {
    // _lock.lockWrite();
    //   // _formation->addAgent(agent);
    // _agents[agent->_id] = agent;
    // _lock.releaseWrite();
    AimingTask* task = AimingTask::getSingleton();
    // NearAgent d(100, 0x0);
    return task->removeAgent(agent->_id);
  }

  /////////////////////////////////////////////////////////////////////

  AimingComponent::AimingComponent() : VelComponent() {
  }


  bool AimingComponent::getTargetEnem(const BaseAgent* agent, NearAgent& result) const {
    AimingTask* task = AimingTask::getSingleton();
    // NearAgent d(100, 0x0);
    return task->getCurrentTarget(agent, result);
    // return d;
  }

  /////////////////////////////////////////////////////////////////////

  void AimingComponent::setPrefVelocity( const BaseAgent * agent, const Goal * goal,
                      PrefVelocity & pVel ) const {
    NearAgent aimEnem(10000, 0x0);
    // setDirections(pVel);
    // assume the goal is a identity goal
    goal->setDirections(agent->_pos, agent->_radius, pVel);
    if (getTargetEnem(agent, aimEnem)) {
      // pVel.setSpeed(0);
      if (aimEnem.agent == 0x0) {
        return;
      }
      pVel.setTarget(aimEnem.agent->_pos);
    }
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
