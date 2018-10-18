#include "UserCommandCondition.h"
#include "MengeCore/Core.h"
#include "MengeCore/Agents/BaseAgent.h"
#include "UserCommandTask.h"

namespace Napoleon {

    bool UserCommandCondition::conditionMet( BaseAgent * agent, const Goal * goal ) {
      UserCommandTask* tsk = UserCommandTask::getSingleton();
      const UserGroupCommand& groupCmd = tsk->getGroupCommand(agent->_class);

      bool isValid = true;
      if (_canFire && !groupCmd.canFire) {
        isValid = false;
      }
      if (_toFormation && !groupCmd.moveToFormation) {
        isValid = false;
      }
      return isValid;
    }

    ///////////////////////////////////////////////////////////////////////////

    UserCommandCondition * UserCommandCondition::copy() {
      return new UserCommandCondition( *this );
    }

    UserCommandConditionFactory::UserCommandConditionFactory() : ConditionFactory() {
      _canFireID = _attrSet.addBoolAttribute("can_fire", false, false);
      _toFormationID = _attrSet.addBoolAttribute("to_formation", false, false);
    }

    ///////////////////////////////////////////////////////////////////////////

    bool UserCommandConditionFactory::setFromXML( Condition * condition, TiXmlElement * node,
                       const std::string & behaveFldr ) const {
      UserCommandCondition * tCond = dynamic_cast< UserCommandCondition * >( condition );
      assert( tCond != 0x0 &&
          "Trying to set the properties of a user command condition on an incompatible "
          "object" );

      if ( !ConditionFactory::setFromXML( condition, node, behaveFldr ) ) return false;

      tCond->_canFire = _attrSet.getBool(_canFireID);
      tCond->_toFormation = _attrSet.getBool(_toFormationID);

      if (!tCond->_canFire && !tCond->_toFormation) {
        return false;
      }

      return true;
    }

} // namespace Napoleon
