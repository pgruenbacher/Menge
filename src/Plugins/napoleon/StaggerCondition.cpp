#include "StaggerCondition.h"
#include "MengeCore/Core.h"
#include "MengeCore/Agents/BaseAgent.h"
#include "StaggerTask.h"

namespace Napoleon {


    ///////////////////////////////////////////////////////////////////////////
    //                   Implementation of StaggerCondition
    ///////////////////////////////////////////////////////////////////////////

    StaggerCondition::StaggerCondition() {
    }


    ///////////////////////////////////////////////////////////////////////////

    bool StaggerCondition::conditionMet( BaseAgent * agent, const Goal * goal ) {
      StaggerTask* stgr = StaggerTask::getSingleton();
      return stgr->canStagger(agent->_id);
    }

    ///////////////////////////////////////////////////////////////////////////

    StaggerCondition * StaggerCondition::copy() {
      return new StaggerCondition( *this );
    }

    ///////////////////////////////////////////////////////////////////////////
    //                   Implementation of StaggerCondFactory
    /////////////////////////////////////////////////////////////////////

    StaggerCondFactory::StaggerCondFactory() : ConditionFactory() {

    }

    ///////////////////////////////////////////////////////////////////////////

    bool StaggerCondFactory::setFromXML( Condition * condition, TiXmlElement * node,
                       const std::string & behaveFldr ) const {
      StaggerCondition * tCond = dynamic_cast< StaggerCondition * >( condition );
      assert( tCond != 0x0 &&
          "Trying to set the properties of a enemy near condition on an incompatible "
          "object" );

      if ( !ConditionFactory::setFromXML( condition, node, behaveFldr ) ) return false;

      return true;
    }

} // namespace Napoleon
