#include "EnemyNearCondition.h"
#include "MengeCore/Core.h"
#include "MengeCore/Agents/BaseAgent.h"

namespace Napoleon {


    ///////////////////////////////////////////////////////////////////////////
    //                   Implementation of EnemyNearCondition
    ///////////////////////////////////////////////////////////////////////////

    EnemyNearCondition::EnemyNearCondition() {
      _distSquared = 0.5 * 0.5;
      _isClose = true;
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

    void EnemyNearCondition::setDist(float dist) {
      _distSquared = dist * dist;
    }

    ///////////////////////////////////////////////////////////////////////////

    bool EnemyNearCondition::conditionMet( BaseAgent * agent, const Goal * goal ) {
      bool enemClose = false;;
      for (Menge::Agents::NearAgent agt : agent->_nearEnems) {
        if (agt.distanceSquared < _distSquared) {
          enemClose = true;
          break;
        }
      }
      if (_isClose) {
        return enemClose;
      } else {
        return !enemClose;
      }
    }

    ///////////////////////////////////////////////////////////////////////////

    EnemyNearCondition * EnemyNearCondition::copy() {
      return new EnemyNearCondition( *this );
    }

    ///////////////////////////////////////////////////////////////////////////
    //                   Implementation of EnemyNearCondFactory
    /////////////////////////////////////////////////////////////////////

    EnemyNearCondFactory::EnemyNearCondFactory() : ConditionFactory() {
      _distID = _attrSet.addFloatAttribute( "dist", true, 1.0f);
      _isCloseID = _attrSet.addBoolAttribute( "is_close", false, true);
    }

    ///////////////////////////////////////////////////////////////////////////

    bool EnemyNearCondFactory::setFromXML( Condition * condition, TiXmlElement * node,
                       const std::string & behaveFldr ) const {
      EnemyNearCondition * tCond = dynamic_cast< EnemyNearCondition * >( condition );
      assert( tCond != 0x0 &&
          "Trying to set the properties of a enemy near condition on an incompatible "
          "object" );

      if ( !ConditionFactory::setFromXML( condition, node, behaveFldr ) ) return false;

      float dist = _attrSet.getFloat(_distID);
      bool isClose = _attrSet.getBool(_isCloseID);
      tCond->setDist(dist);
      tCond->_isClose = isClose;
      return true;
    }

} // namespace Napoleon
