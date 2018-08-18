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
#ifndef _NUM_ENEMY_CLOSE_H_
#define _NUM_ENEMY_CLOSE_H_

#include "MengeCore/BFSM/Transitions/Condition.h"
#include "MengeCore/CoreConfig.h"
#include "MengeCore/BFSM/fsmCommon.h"
#include "MengeCore/BFSM/Transitions/ConditionFactory.h"

using Menge::Agents::BaseAgent;
using Menge::BFSM::Goal;
using Menge::BFSM::Condition;
using Menge::BFSM::ConditionFactory;

namespace Napoleon {
  // meant to advance / withdraw unit based on num enemies vs num friends nearby.
  // i.e. overwhelming.

  class MENGE_API NumEnemyCloseCondition : public Condition {
    float _friendDistSquared;
    float _enemDistSquared;
    public:
      // bool _isClose;
      NumEnemyCloseCondition();
      virtual bool conditionMet( BaseAgent * agent, const Goal * goal );
      NumEnemyCloseCondition * copy();
      void setEnemDist(float dist);
      void setFriendDist(float dist);
      bool _inverse;
  };

  /*!
   *  @brief    The factory for creating the TimerCondition
   */
  class MENGE_API NumEnemyCloseCondFactory : public ConditionFactory {
  public:
    /*!
     *  @brief    Constructor.
     */
    NumEnemyCloseCondFactory();

    virtual const char * name() const { return "enemy_proximity"; }

    virtual const char * description() const {
      return "Num. Enemy > friends proximity condition.";
    }

  protected:
    virtual Condition * instance() const { return new NumEnemyCloseCondition(); }
    virtual bool setFromXML( Condition * condition, TiXmlElement * node,
                 const std::string & behaveFldr ) const;
    size_t _friendDistID;
    size_t _enemDistID;
    size_t _inverseID;
  };
} // namespace Napoleon
#endif // _NUM_ENEMY_CLOSE_H_
