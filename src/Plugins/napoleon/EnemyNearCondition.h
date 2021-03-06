#ifndef _ENEMY_NEAR_COND_H_
#define _ENEMY_NEAR_COND_H_

#include "MengeCore/BFSM/Transitions/Condition.h"
#include "MengeCore/CoreConfig.h"
#include "MengeCore/BFSM/fsmCommon.h"
#include "MengeCore/BFSM/Transitions/ConditionFactory.h"

using Menge::Agents::BaseAgent;
using Menge::BFSM::Goal;
using Menge::BFSM::Condition;
using Menge::BFSM::ConditionFactory;

namespace Napoleon {

  class MENGE_API EnemyNearCondition : public Condition {
    float _distSquared;
    public:
      bool _isClose;
      EnemyNearCondition();
      virtual bool conditionMet( BaseAgent * agent, const Goal * goal );
      EnemyNearCondition * copy();
      void setDist(float dist);
  };

  /*!
   *  @brief    The factory for creating the TimerCondition
   */
  class MENGE_API EnemyNearCondFactory : public ConditionFactory {
  public:
    /*!
     *  @brief    Constructor.
     */
    EnemyNearCondFactory();

    virtual const char * name() const { return "enemy_near"; }

    virtual const char * description() const {
      return "Enemy near condition.";
    }

  protected:
    virtual Condition * instance() const { return new EnemyNearCondition(); }
    virtual bool setFromXML( Condition * condition, TiXmlElement * node,
                 const std::string & behaveFldr ) const;
    size_t _distID;
    size_t _isCloseID;
  };
} // namespace Napoleon
#endif // _ENEMY_NEAR_COND_H_
