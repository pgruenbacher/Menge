#include "FacingTargetEnem.h"
#include "MengeCore/Core.h"
#include "MengeCore/Agents/BaseAgent.h"
#include "NearestEnemTask.h"

namespace Napoleon {


    ///////////////////////////////////////////////////////////////////////////
    //                   Implementation of EnemyNearCondition
    ///////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////

    bool FacingTargetEnem::conditionMet( BaseAgent * agent, const Goal * goal ) {
      NearestEnemTask* t = NearestEnemTask::getSingleton();
      Menge::Agents::NearAgent d(100, 0x0);
      if (!t->getCurrentTarget(agent, d)) return false;
      if (d.agent == 0x0) return false;
      Vector2 dir = d.agent->_pos - agent->_pos;
      dir.normalize();
      if ((dir * agent->_orient) > _threshold) {
        return true;
      }
      return false;
    }

    ///////////////////////////////////////////////////////////////////////////

    FacingTargetEnem * FacingTargetEnem::copy() {
      return new FacingTargetEnem( *this );
    }

    ///////////////////////////////////////////////////////////////////////////
    //                   Implementation of EnemyNearCondFactory
    /////////////////////////////////////////////////////////////////////

    FacingTargetEnemCondFactory::FacingTargetEnemCondFactory() : ConditionFactory() {
      _thresholdID = _attrSet.addFloatAttribute("threshold", false, 0.5);
    }

    ///////////////////////////////////////////////////////////////////////////

    bool FacingTargetEnemCondFactory::setFromXML( Condition * condition, TiXmlElement * node,
                       const std::string & behaveFldr ) const {
      FacingTargetEnem * tCond = dynamic_cast< FacingTargetEnem * >( condition );
      assert( tCond != 0x0 &&
          "Trying to set the properties of a enemy near condition on an incompatible "
          "object" );

      if ( !ConditionFactory::setFromXML( condition, node, behaveFldr ) ) return false;

      float threshold = _attrSet.getFloat(_thresholdID);
      tCond->_threshold = (threshold);


      return true;
    }

} // namespace Napoleon
