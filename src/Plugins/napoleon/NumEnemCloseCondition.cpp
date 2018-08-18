#include "NumEnemCloseCondition.h"

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

#include "MengeCore/Core.h"
#include "MengeCore/Agents/BaseAgent.h"

namespace Napoleon {

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
    ///////////////////////////////////////////////////////////////////////////
    //                   Implementation of EnemyNearCondition
    ///////////////////////////////////////////////////////////////////////////

    NumEnemyCloseCondition::NumEnemyCloseCondition() {
      _friendDistSquared = 4.0 * 4.0;
      _enemDistSquared = 4.0 * 4.0;
      // _isClose = true;
      _inverse = false;
    }

    ///////////////////////////////////////////////////////////////////////////

    // EnemyNearCondition::EnemyNearCondition( const EnemyNearCondition & cond ):Condition(cond) {
    // }

    ///////////////////////////////////////////////////////////////////////////

    // EnemyNearCondition::~EnemyNearCondition() {
    // }

    ///////////////////////////////////////////////////////////////////////////

    // void EnemyNearCondition::onEnter( BaseAgent * agent ) {
    // }

    // ///////////////////////////////////////////////////////////////////////////

    // void EnemyNearCondition::onLeave( BaseAgent * agent ) {
    // }

    void NumEnemyCloseCondition::setFriendDist(float dist) {
      _friendDistSquared = dist * dist;
    }
    void NumEnemyCloseCondition::setEnemDist(float dist) {
      _enemDistSquared = dist * dist;
    }

    ///////////////////////////////////////////////////////////////////////////

    bool NumEnemyCloseCondition::conditionMet( BaseAgent * agent, const Goal * goal ) {
      float sumEnem = sumNearAgentWeights(agent->_nearEnems, _enemDistSquared);
      float sumFriend = sumNearAgentWeights(agent->_nearFriends, _friendDistSquared);
      if (_inverse) {
        return !(sumEnem > (sumFriend));
      }
      return (sumEnem > (sumFriend));
    }

    ///////////////////////////////////////////////////////////////////////////

    NumEnemyCloseCondition * NumEnemyCloseCondition::copy() {
      return new NumEnemyCloseCondition( *this );
    }

    ///////////////////////////////////////////////////////////////////////////
    //                   Implementation of EnemyNearCondFactory
    /////////////////////////////////////////////////////////////////////

    NumEnemyCloseCondFactory::NumEnemyCloseCondFactory() : ConditionFactory() {
      _friendDistID = _attrSet.addFloatAttribute( "enem_dist", false, 4.0f);
      _enemDistID = _attrSet.addFloatAttribute( "friend_dist", false, 4.0f);
      // _isCloseID = _attrSet.addBoolAttribute( "is_close", false, true);
      _inverseID = _attrSet.addBoolAttribute("inverse", false, false);
    }

    ///////////////////////////////////////////////////////////////////////////

    bool NumEnemyCloseCondFactory::setFromXML( Condition * condition, TiXmlElement * node,
                       const std::string & behaveFldr ) const {
      NumEnemyCloseCondition * tCond = dynamic_cast< NumEnemyCloseCondition * >( condition );
      assert( tCond != 0x0 &&
          "Trying to set the properties of a enemy near condition on an incompatible "
          "object" );

      if ( !ConditionFactory::setFromXML( condition, node, behaveFldr ) ) return false;

      float friendDist = _attrSet.getFloat(_friendDistID);
      float enemDist = _attrSet.getFloat(_enemDistID);
      // bool isClose = _attrSet.getBool(_isCloseID);
      bool inverse = _attrSet.getBool(_inverseID);
      tCond->setFriendDist(friendDist);
      tCond->setEnemDist(enemDist);
      tCond->_inverse = inverse;
      // tCond->_isClose = isClose;
      return true;
    }

} // namespace Napoleon
