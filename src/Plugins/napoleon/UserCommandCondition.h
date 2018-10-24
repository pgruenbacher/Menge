#ifndef _USER_COMMAND_CONDITION_H_
#define _USER_COMMAND_CONDITION_H_

#include "MengeCore/BFSM/Transitions/Condition.h"
#include "MengeCore/CoreConfig.h"
#include "MengeCore/BFSM/fsmCommon.h"
#include "MengeCore/BFSM/Transitions/ConditionFactory.h"

using Menge::Agents::BaseAgent;
using Menge::BFSM::Goal;
using Menge::BFSM::Condition;
using Menge::BFSM::ConditionFactory;

namespace Napoleon {

  class MENGE_API UserCommandCondition : public Condition {

    public:
      // UserCommandCondition();
      virtual bool conditionMet( BaseAgent * agent, const Goal * goal );
      UserCommandCondition * copy();
      bool _canFire;
      bool _toFormation;
      bool _toWaypoints;
  };

  /*!
   *  @brief    The factory for creating the TimerCondition
   */
  class MENGE_API UserCommandConditionFactory : public ConditionFactory {
  public:
    /*!
     *  @brief    Constructor.
     */
    UserCommandConditionFactory();

    virtual const char * name() const { return "user_command_condition"; }

    virtual const char * description() const {
      return "Facing target enem condition.";
    }

  protected:

    size_t _canFireID;
    size_t _toFormationID;
    size_t _toWaypointsID;

    virtual Condition * instance() const { return new UserCommandCondition(); }
    virtual bool setFromXML( Condition * condition, TiXmlElement * node,
                 const std::string & behaveFldr ) const;
  };
} // namespace Napoleon
#endif // _USER_COMMAND_CONDITION_H_
