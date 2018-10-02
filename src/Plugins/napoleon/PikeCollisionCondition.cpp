#include "PikeCollisionCondition.h"
#include "MengeCore/Core.h"
#include "MengeCore/Agents/BaseAgent.h"
#include "PikeTask.h"

namespace Napoleon {


    ///////////////////////////////////////////////////////////////////////////
    //                   Implementation of PikeCollisionCondition
    ///////////////////////////////////////////////////////////////////////////

    PikeCollisionCondition::PikeCollisionCondition() : _distSquared(0.f) {
    }


    ///////////////////////////////////////////////////////////////////////////

    bool PikeCollisionCondition::conditionMet( BaseAgent * agent, const Goal * goal ) {
      PikeTask* task = PikeTask::getSingleton();
      return task->areAgentsCollidingWithPike(agent);
    }

    ///////////////////////////////////////////////////////////////////////////

    PikeCollisionCondition * PikeCollisionCondition::copy() {
      return new PikeCollisionCondition( *this );
    }

    ///////////////////////////////////////////////////////////////////////////
    //                   Implementation of PikeCollisionCondFactory
    /////////////////////////////////////////////////////////////////////

    PikeCollisionCondFactory::PikeCollisionCondFactory() : ConditionFactory() {

    }

    ///////////////////////////////////////////////////////////////////////////

    bool PikeCollisionCondFactory::setFromXML( Condition * condition, TiXmlElement * node,
                       const std::string & behaveFldr ) const {
      PikeCollisionCondition * tCond = dynamic_cast< PikeCollisionCondition * >( condition );
      assert( tCond != 0x0 &&
          "Trying to set the properties of a enemy near condition on an incompatible "
          "object" );

      if ( !ConditionFactory::setFromXML( condition, node, behaveFldr ) ) return false;

      return true;
    }

} // namespace Napoleon
