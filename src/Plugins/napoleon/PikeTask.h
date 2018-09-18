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
 *  @file   FormationsTask.h
 *  @brief    Task used to maintain a single formation.
 */

#ifndef _PIKE_TASK_H_
#define _PIKE_TASK_H_

#include "MengeCore/BFSM/fsmCommon.h"
#include "MengeCore/BFSM/Tasks/Task.h"
#include "MengeCore/BFSM/Tasks/TaskFactory.h"

#include <string>
#include <map>

#include "thirdParty/tinyxml.h"
#include "MengeCore/Runtime/ReadersWriterLock.h"
#include "MengeCore/Math/RandGenerator.h"
#include "MengeCore/Agents/BaseAgent.h"

using namespace Menge;
namespace Napoleon {

  class PikeProximityQuery : public Menge::Agents::ProximityQuery {

      /*!
       *  @brief   the max number of agent results to store
       */
      std::vector<Menge::Agents::NearAgent> _agentResults;
      size_t _maxAgentResults;

      /*!
       *  @brief   the max distance to an agent result. Useful for informing the spatial
       *           query to stop searching further.
       */
      float _maxAgentResultDistance;

      /*!
       *  @brief   the start point for the query
       */
      Math::Vector2 queryPoint;
      Math::Vector2 queryDirection;
      float queryDotProduct;
    public:
      PikeProximityQuery(Vector2 pos, Vector2 dir);
      // PikeProximityQuery(Vector2 point) : _queryPoint(point), _maxAgentResultDistance(2*2),
      float getMaxAgentRange() override { return _maxAgentResultDistance; }
      void filterAgent(const Menge::Agents::BaseAgent * agent, float distanceSquared) override;
      void startQuery() override {}; // no need to clear or reset anything.
      Vector2 getQueryPoint() override { return queryPoint; };
      bool containsAgent(const Menge::Agents::BaseAgent* agent) const;
      // empties
      float getMaxObstacleRange() override {};
      void filterObstacle(const Menge::Agents::Obstacle* obstacle, float distSq) override {};
  };

  struct Pike {
    Vector2 pos;
    Vector2 direction;
    PikeProximityQuery query;
    Pike(Vector2 pos, Vector2 dir);
  };
    /*!
   *  @brief  Task responsible for updating agent data for maintaining a formation.
   */
class PikeTask : public Menge::BFSM::Task {
private:
  ReadersWriterLock _lock;
  typedef std::map<size_t, Pike> PikeMap;
  PikeMap _pikes;

    static PikeTask* PIKE_TASK;
 public:
  /*!
   *  @brief    Constructor
   *
   */
  PikeTask();

  virtual void doWork(const Menge::BFSM::FSM* fsm) throw(
      Menge::BFSM::TaskException);

  virtual std::string toString() const;
  bool isAgentFacingPike(const Menge::Agents::BaseAgent* agt) const;
  virtual bool isEquivalent(const Menge::BFSM::Task* task) const;
  void adjustPike(const Menge::Agents::BaseAgent& agent, bool is_lowered, Vector2 pos, Vector2 dir);
  static PikeTask* getSingleton();
  };


} // namespace Napoleon
#endif // _PIKE_TASK_H_
