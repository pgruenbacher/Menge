#ifndef _FACING_TARGET_ENEM_H_
#define _FACING_TARGET_ENEM_H_

#include "MengeCore/BFSM/Transitions/Condition.h"
#include "MengeCore/CoreConfig.h"
#include "MengeCore/BFSM/fsmCommon.h"
#include "MengeCore/BFSM/Transitions/ConditionFactory.h"

using Menge::Agents::BaseAgent;
using Menge::BFSM::Goal;
using Menge::BFSM::Condition;
using Menge::BFSM::ConditionFactory;

namespace Napoleon {

  class MENGE_API FacingTargetEnem : public Condition {

    public:
      // FacingTargetEnem();
      virtual bool conditionMet( BaseAgent * agent, const Goal * goal );
      FacingTargetEnem * copy();
      float _threshold;
  };

  /*!
   *  @brief    The factory for creating the TimerCondition
   */
  class MENGE_API FacingTargetEnemCondFactory : public ConditionFactory {
  public:
    /*!
     *  @brief    Constructor.
     */
    FacingTargetEnemCondFactory();

    virtual const char * name() const { return "facing_enemy"; }

    virtual const char * description() const {
      return "Facing target enem condition.";
    }

  protected:
    size_t _thresholdID;
    virtual Condition * instance() const { return new FacingTargetEnem(); }
    virtual bool setFromXML( Condition * condition, TiXmlElement * node,
                 const std::string & behaveFldr ) const;
  };
} // namespace Napoleon
#endif // _FACING_TARGET_ENEM_H_
