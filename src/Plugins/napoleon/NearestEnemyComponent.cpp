/*

License

Menge
Copyright © and trademark ™ 2012-14 University of North Carolina at Chapel Hill.
All rights reserved.

Permission to use, copy, modify, and distribute this software and its documentation
for educational, research, and non-profit purposes, without fee, and without a
written agreement is hereby granted, provided that the above copyright notice,
this paragraph, and the following four paragraphs appear in all copies.

This software program and documentation are copyrighted by the University of North
Carolina at Chapel Hill. The software program and documentation are supplied "as is,"
without any accompanying services from the University of North Carolina at Chapel
Hill or the authors. The University of North Carolina at Chapel Hill and the
authors do not warrant that the operation of the program will be uninterrupted
or error-free. The end-user understands that the program was developed for research
purposes and is advised not to rely exclusively on the program for any reason.

IN NO EVENT SHALL THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL OR THE AUTHORS
BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS
DOCUMENTATION, EVEN IF THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL OR THE
AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL AND THE AUTHORS SPECIFICALLY
DISCLAIM ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE AND ANY STATUTORY WARRANTY
OF NON-INFRINGEMENT. THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND
THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL AND THE AUTHORS HAVE NO OBLIGATIONS
TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

Any questions or comments should be sent to the authors {menge,geom}@cs.unc.edu

*/

// #include "MengeCore/BFSM/VelocityComponents/VelCompGoal.h"
#include "NearestEnemyComponent.h"
#include "MengeCore/resources/Resource.h"
#include "MengeCore/Core.h"
#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/Agents/PrefVelocity.h"
#include "MengeCore/BFSM/Goals/Goal.h"
#include "MengeCore/Runtime/os.h"
#include "NearestEnemTask.h"

const float sumNearAgentWeights(const std::vector<Menge::Agents::NearAgent>& nearAgents, float checkDist = 3.0) {
  // return nearAgents.size();
  // WTF C++, if I don't set to 0.0, then it defaults to whatever previous value was!??
  // Apparently uninitalized variables don't default to zero...
  // return nearAgents.size();
  // const float meleeDist = 3.0;
  float result = 0.0;
  // std::cout << "RESULT?" << result << std::endl;

  for (Menge::Agents::NearAgent agent : nearAgents) {
    // result += agent.distanceSquared;
    if (checkDist < agent.distanceSquared) {
      continue;
    }
    // std::cout << "V " << (agent.agent->_neighborDist * agent.agent->_neighborDist) << " " << agent.distanceSquared << std::endl;
    result += 1.0f;
  }
  // std::cout << "RESULT" << result << " " << nearAgents.size() << std::endl;
  return result;
}

namespace Napoleon {
  using Menge::logger;

  /////////////////////////////////////////////////////////////////////
  //                   Implementation of GoalVelComponent
  /////////////////////////////////////////////////////////////////////

  const std::string NearestEnemComponent::NAME = "enemy_near_comp";

  /////////////////////////////////////////////////////////////////////

  void NearestEnemComponent::onEnter(BaseAgent* agent) {
    // _lock.lockWrite();
    //   // _formation->addAgent(agent);
    // _agents[agent->_id] = agent;
    // _lock.releaseWrite();
    NearestEnemTask* task = NearestEnemTask::getSingleton();
    task->addAgent(agent->_id);
  }

  void NearestEnemComponent::onExit(BaseAgent* agent) {
    // _lock.lockWrite();
    //   // _formation->addAgent(agent);
    // _agents[agent->_id] = agent;
    // _lock.releaseWrite();
    NearestEnemTask* task = NearestEnemTask::getSingleton();
    task->removeAgent(agent->_id);
  }

  /////////////////////////////////////////////////////////////////////

  NearestEnemComponent::NearestEnemComponent() : VelComponent() {
  }

  void NearestEnemComponent::setIdleVelocity(const BaseAgent* enem, const BaseAgent * agent, const Goal * goal,
                      PrefVelocity & pVel ) const {
      using Menge::SIM_TIME_STEP;
      Vector2 target = enem->_pos;
      goal->setDirections( agent->_pos, agent->_radius, pVel );

      // speed
      Vector2 goalPoint = pVel.getTarget();
      Vector2 disp = goalPoint - agent->_pos;
      const float distSq = absSq( disp );
      float speed = agent->_prefSpeed;

      if ( distSq <= 0.0001f ) {
        // I've basically arrived -- speed should be zero.
        speed = 0.f;
      } else {
        const float speedSq = speed * speed;
        const float TS_SQD = SIM_TIME_STEP * SIM_TIME_STEP;
        if ( distSq / speedSq < TS_SQD ) {
          // The distance is less than I would travel in a single time step.
          speed = sqrtf( distSq ) / SIM_TIME_STEP;
        }
      }
      pVel.setSpeed( speed );
      // also make sure target is set for orientation.
      pVel.setTarget( target );
  }

  void NearestEnemComponent::setWithdrawingVelocity(const BaseAgent* enem,
                                                  const BaseAgent* agent,
                                                  PrefVelocity& pVel,
                                                  float distSq) const {
    Vector2 target = enem->_pos;
    Vector2 disp = target - agent->_pos;
    Vector2 dir(0.0, 0.0);
    if ( distSq > 1e-8 ) {
      // Distant enough that I can normalize the direction.
      dir.set( disp / sqrtf( distSq ) );
    } else {
      dir.set( 0.f, 0.f );
    }
    pVel.setSingle(-dir);
    pVel.setSpeed(0.1);
    pVel.setTarget(target);
    return;

  }

  void NearestEnemComponent::setAdvancingVelocity(const BaseAgent* enem,
                                                  const BaseAgent* agent,
                                                  PrefVelocity& pVel,
                                                  float distSq) const {
    Vector2 target = enem->_pos;
    Vector2 disp = target - agent->_pos;
    Vector2 dir(0.0, 0.0);
    if ( distSq > 1e-8 ) {
      // Distant enough that I can normalize the direction.
      dir.set( disp / sqrtf( distSq ) );
    } else {
      dir.set( 0.f, 0.f );
    }
    pVel.setTarget(target);
    float speed = agent->_prefSpeed;
    if (distSq <= (agent->getMeleeRange() - 0.2) && _slowToArrive) {
      // I've basically arrived -- speed should be zero.
      speed = 0.f;
    } else {
      const float speedSq = speed * speed;
      const float TS_SQD = Menge::SIM_TIME_STEP * Menge::SIM_TIME_STEP;
      if (distSq / speedSq < TS_SQD && _slowToArrive) {
        // The distance is less than I would travel in a single time step.
        speed = sqrtf(distSq) / Menge::SIM_TIME_STEP;
      } else {
        // std:: cout << "?? " << distSq / speedSq << " " << TS_SQD << " " <<
        // SIM_TIME_STEP << std::endl; speed = agent->_prefSpeed;
      }
    }
    // std::cout << "SPEED " << speed << std::endl;
    pVel.setSingle(dir);
    pVel.setSpeed(speed);
  }

  const NearAgent NearestEnemComponent::getTargetEnem(const BaseAgent* agent) const {
    // float distSq = 1000.f * 1000.f;
    // NearAgent targetEnem(distSq, 0x0);
    // for (Menge::Agents::NearAgent enem : agent->_nearEnems) {
    //   if (enem.distanceSquared < distSq) {
    //     distSq = enem.distanceSquared;
    //     targetEnem = enem;
    //   }
    // }

    const NearestEnemTask* task = NearestEnemTask::getSingleton();
    NearAgent d(100, 0x0);
    task->getCurrentTarget(agent, d);

    return d;
  }

  /////////////////////////////////////////////////////////////////////

  void NearestEnemComponent::setPrefVelocity( const BaseAgent * agent, const Goal * goal,
                      PrefVelocity & pVel ) const {
    bool modify = agent->_nearEnems.size() > 0;
    Vector2 target(0.f, 0.f);
    if ( !modify ) {
      // pVel.setSpeed(agent->_prefSpeed);
      // Vector2 returnToOrig = (agent->_pos) * -1;
      // returnToOrig.normalize();
      // pVel.setSingle(returnToOrig);
      // std::cout << "no modify! fix! " << agent->_id << "GOAL" << goal << " " << std::endl;
      // return setIdleVelocity(agent, goal, pVel);
      goal->setDirections(agent->_pos, agent->_radius, pVel);
      // goal.
      return;
    }
    const NearestEnemData targetEnem = getTargetEnem(agent);
    if (targetEnem.agent == 0x0) return;
    // target = targetEnem.agent->_pos;
    float distSq = targetEnem.distanceSquared;

    if (_actionType == WITHDRAWING) {
      return setWithdrawingVelocity(targetEnem.agent, agent, pVel, distSq);
    } else if (_actionType == IDLE) {
      return setIdleVelocity(targetEnem.agent, agent, goal, pVel);
    } else {
      return setAdvancingVelocity(targetEnem.agent, agent, pVel, distSq);
    }
  }

  Task * NearestEnemComponent::getTask(){
    return NearestEnemTask::getSingleton();
  };


  /////////////////////////////////////////////////////////////////////
  NearestEnemComponentFactory::NearestEnemComponentFactory() : VelCompFactory() {
    _methodID = _attrSet.addStringAttribute("method", true /* required */);
    _slowToArriveID = _attrSet.addBoolAttribute("slow_to_arrive", false, false);
  }

  /////////////////////////////////////////////////////////////////////

  bool NearestEnemComponentFactory::setFromXML( VelComponent * component, TiXmlElement * node,
                     const std::string & behaveFldr ) const {
      NearestEnemComponent * nearestEnemComp = dynamic_cast<NearestEnemComponent *>(component);
    if ( ! VelCompFactory::setFromXML( component, node, behaveFldr ) ) return false;

      assert( nearestEnemComp != 0x0 &&
      "Trying to set property component properties on an incompatible object" );

    if (_attrSet.getString(_methodID).empty()) {
      logger << Logger::ERR_MSG << "Require method for velocity component";
      logger.close();
      throw Menge::BFSM::VelCompFatalException( "Require method for velocity component." );
    }

    std::string _typeString = _attrSet.getString(_methodID);
    if (_typeString == "advancing") {
      nearestEnemComp->_actionType = NearestEnemComponent::ADVANCING;
    } else if (_typeString == "withdrawing") {
      nearestEnemComp->_actionType = NearestEnemComponent::WITHDRAWING;
    } else if (_typeString == "idle") {
      nearestEnemComp->_actionType = NearestEnemComponent::IDLE;
    } else {
      logger << Logger::ERR_MSG << "Should be advancing or withdrawing got: '" << _typeString << "' instead...";
      return false;
    }
    nearestEnemComp->_slowToArrive = _attrSet.getBool(_slowToArriveID);
    return true;
  }
} // namespace Formations
