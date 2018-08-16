/*
 Menge Crowd Simulation Framework

 Copyright and trademark 2012-17 University of North Carolina at Chapel Hill

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0
 or
    LICENSE.txt in the root of the Menge repository.

 Any questions or comments should be sent to the authors menge@cs.unc.edu

 <http://gamma.cs.unc.edu/Menge/>
*/

/*!
 *  @file   Condition.h
 *  @brief    The basis for determining the conditions under which
 *        transitions become "active" (and are taken).
 */
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
    virtual Condition * instance() const { return new FacingTargetEnem(); }
    virtual bool setFromXML( Condition * condition, TiXmlElement * node,
                 const std::string & behaveFldr ) const;
  };
} // namespace Napoleon
#endif // _FACING_TARGET_ENEM_H_
