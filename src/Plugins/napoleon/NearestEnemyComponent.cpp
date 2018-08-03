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


namespace Napoleon {

  /////////////////////////////////////////////////////////////////////
  //                   Implementation of GoalVelComponent
  /////////////////////////////////////////////////////////////////////

  const std::string NearestEnemComponent::NAME = "enemy_near_comp";

  /////////////////////////////////////////////////////////////////////

  void NearestEnemComponent::onEnter(BaseAgent* agent) {
    _lock.lockWrite();
      // _formation->addAgent(agent);
    _agents[agent->_id] = agent;
    _lock.releaseWrite();
  }

  void NearestEnemComponent::onExit(BaseAgent* agent) {
    _lock.lockWrite();
      // _formation->addAgent(agent);
    _agents[agent->_id] = agent;
    _lock.releaseWrite();
  }

  /////////////////////////////////////////////////////////////////////

  NearestEnemComponent::NearestEnemComponent() : VelComponent() {
  }


  /////////////////////////////////////////////////////////////////////

  void NearestEnemComponent::setPrefVelocity( const BaseAgent * agent, const Goal * goal,
                      PrefVelocity & pVel ) const {
    bool modify = agent->_nearEnems.size() > 0;
    if ( modify ) {
      Vector2 target;
      float distSq = 1000.f * 1000.f;
      float localDist;
      for (Menge::Agents::NearAgent enem : agent->_nearEnems) {
        if (enem.distanceSquared < distSq) {
          distSq = enem.distanceSquared;
          target = enem.agent->_pos;
        }
      }

      // target + displacement - agent position
      Vector2 disp = target - agent->_pos;
      Vector2 dir;
      if ( distSq > 1e-8 ) {
        // Distant enough that I can normalize the direction.
        dir.set( disp / sqrtf( distSq ) );
      } else {
        dir.set( 0.f, 0.f );
      }
      pVel.setSingle(dir);
      if (agent->_nearEnems.size() > 5) {
        std::cout << "ENEM SIZE " << agent->_nearEnems.size() << std::endl;
        pVel.setSingle(-dir);
        pVel.setSpeed(0.1);
        pVel.setTarget(target);
        return;
      }
      pVel.setTarget(target);

      float speed = agent->_prefSpeed;

      if ( distSq <= agent->getMeleeRange() ) {
        // I've basically arrived -- speed should be zero.
        speed = 0.f;
      } else {
        const float speedSq = speed * speed;
        const float TS_SQD = Menge::SIM_TIME_STEP * Menge::SIM_TIME_STEP;
        if ( distSq / speedSq < TS_SQD ) {
          // The distance is less than I would travel in a single time step.
          speed = sqrtf( distSq ) / Menge::SIM_TIME_STEP;
          // std::cout << "DO REDUCE" << std::endl;
        } else {
          // std:: cout << "?? " << distSq / speedSq << " " << TS_SQD << " " << SIM_TIME_STEP << std::endl;
        }
      }
      pVel.setSpeed( speed );

    } else {

      // std::cout << "no modify " << target._x << " " << target._y << std::endl;
    }
  }

  // Task * NearestEnemComponent::getTask(){
  //   // return new FormationsTask( _formation );
  // };


  /////////////////////////////////////////////////////////////////////
  NearestEnemComponentFactory::NearestEnemComponentFactory() : VelCompFactory() {
  }

  /////////////////////////////////////////////////////////////////////

  bool NearestEnemComponentFactory::setFromXML( VelComponent * component, TiXmlElement * node,
                     const std::string & behaveFldr ) const {
      NearestEnemComponent * nearestEnemComp = dynamic_cast<NearestEnemComponent *>(component);
      assert( nearestEnemComp != 0x0 &&
      "Trying to set property component properties on an incompatible object" );
    if ( ! VelCompFactory::setFromXML( nearestEnemComp, node, behaveFldr ) ) return false;
    return true;
  }
} // namespace Formations
