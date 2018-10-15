#include "UserCommandCondition.h"
#include "MengeCore/Core.h"
#include "MengeCore/Agents/BaseAgent.h"
#include "UserCommandTask.h"

namespace Napoleon {

    bool UserCommandCondition::conditionMet( BaseAgent * agent, const Goal * goal ) {
    }

    ///////////////////////////////////////////////////////////////////////////

    UserCommandCondition * UserCommandCondition::copy() {
      return new UserCommandCondition( *this );
    }

    UserCommandConditionFactory::UserCommandConditionFactory() : ConditionFactory() {
    }

    ///////////////////////////////////////////////////////////////////////////

    bool UserCommandConditionFactory::setFromXML( Condition * condition, TiXmlElement * node,
                       const std::string & behaveFldr ) const {
      UserCommandCondition * tCond = dynamic_cast< UserCommandCondition * >( condition );
      assert( tCond != 0x0 &&
          "Trying to set the properties of a user command condition on an incompatible "
          "object" );

      if ( !ConditionFactory::setFromXML( condition, node, behaveFldr ) ) return false;



      return true;
    }

} // namespace Napoleon
