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

#include "TurnModifier.h"
#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/Core.h"

namespace Napoleon {

  using Menge::logger;
  using Menge::Logger;
  using Menge::Agents::BaseAgent;
  using Menge::Agents::PrefVelocity;
  using Menge::BFSM::Task;
  using Menge::BFSM::VelModifier;
  using Menge::BFSM::VelModFactory;
  using Menge::Math::Vector2;

  /////////////////////////////////////////////////////////////////////
  //                   Implementation of TurnModifier
  /////////////////////////////////////////////////////////////////////

  TurnModifier::TurnModifier() {
  }

  /////////////////////////////////////////////////////////////////////

  TurnModifier::~TurnModifier(){
  };

  /////////////////////////////////////////////////////////////////////

  VelModifier* TurnModifier::copy() const{
    return new TurnModifier( );
  };

  /////////////////////////////////////////////////////////////////////

  void TurnModifier::adaptPrefVelocity( const BaseAgent * agent, PrefVelocity & pVel ) {
    float radius = agent->getTurnRadius();

    Vector2 pref = pVel.getPreferred();
    pref.normalize();
    Vector2 cur = agent->_vel;
    cur.normalize();


    // const float MAX_ANGLE_CHANGE = timeStep * _maxAngVel;
    // float maxCt = cos( MAX_ANGLE_CHANGE );
    // float ct = newOrient * newDir;
    float MAX_ANGLE_CHANGE = agent->_maxAngVel * Menge::SIM_TIME_STEP;

    float maxCt = cos(MAX_ANGLE_CHANGE);
    float ct = cur * pref;
    Vector2 newPref = agent->_vel;
    if (absSq(newPref) == 0) {
      newPref = agent->_orient;
    }
    newPref.normalize();

    if (agent->targetInTurnCircle(pVel.getTarget() + (newPref * 2.f))) {
      // go current vel
      if (ct < 0.5) {
        pVel.setSingle(newPref);
        pVel.setSpeed(abs(agent->_vel));
        return;
      } else {
        // this means that the target is relatively forward of us, so we should slow down to turn to approach..
        // unless we're charging...
        std::cout << "GO FOR" << std::endl;
        pVel.setSpeed(std::max(abs(agent->_prefSpeed) - 0.5, double(0)));
      }
    }

    if (ct > maxCt) {
      // within turn limit.
      std::cout << "WITHIN TURN LIMIT" << std::endl;
      return;
    } else {
      // changing direction at a rate greater than _maxAngVel
      float maxSt = sin( MAX_ANGLE_CHANGE );
      if ( det( newPref, pref ) > 0.f ) {
        // rotate newDir left
        newPref.set( maxCt * newPref._x - maxSt * newPref._y,
               maxSt * newPref._x + maxCt * newPref._y );
      } else {
        // rotate newDir right
        newPref.set( maxCt * newPref._x + maxSt * newPref._y,
               -maxSt * newPref._x + maxCt * newPref._y );
      }

      // std::cout << "NEW PREF " << agent->_maxAngVel << " " << maxCt << " " << ct << " " << pVel.getSpeed() << std::endl;
    }
    pVel.setSingle(newPref);
    // pVel.setSpeed
    // std::cout << "CTERR " << maxCT <<  " " << ct << std::endl;

    // Vector2 target = pVel.getTarget();
    // std::cout << "TRA " << target << std::endl;
    // float cross

  }

  /////////////////////////////////////////////////////////////////////

  // void TurnModifier::registerAgent(const BaseAgent * agent) {
  // };

  /////////////////////////////////////////////////////////////////////

  // void TurnModifier::unregisterAgent(const BaseAgent * agent){

  // };

  /////////////////////////////////////////////////////////////////////
  //                   Implementation of FormationModFactory
  /////////////////////////////////////////////////////////////////////

  TurnModifierFactory::TurnModifierFactory() : VelModFactory() {
    //no properties yet
  }

  /////////////////////////////////////////////////////////////////////

  bool TurnModifierFactory::setFromXML( VelModifier * modifier, TiXmlElement * node,
                         const std::string & behaveFldr ) const {
    TurnModifier * turnMod = dynamic_cast<TurnModifier *>(modifier);
        assert( turnMod != 0x0 &&
        "Trying to set property modifier properties on an incompatible object" );

    if ( ! Menge::BFSM::VelModFactory::setFromXML( modifier, node, behaveFldr ) ) return false;

    return true;
  }

  /////////////////////////////////////////////////////////////////////
} // namespace Napoleons
