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

#ifndef __NEAREST_ENEM_TASK_H__
#define __NEAREST_ENEM_TASK_H__

#include <map>
#include <string>
#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/BFSM/Tasks/Task.h"
#include "MengeCore/BFSM/Tasks/TaskFactory.h"
#include "MengeCore/BFSM/fsmCommon.h"
#include "MengeCore/Runtime/ReadersWriterLock.h"
#include "MengeCore/Agents/SimulatorInterface.h"

#include "thirdParty/tinyxml.h"

namespace Napoleon {
using namespace Menge;
/*!
* @brief  Task responsible for updating agent data for maintaining a formation.
*/
enum NearestEnemMethod {
  // has to be within min dist
  MELEE,
  // has to be within angle
  AIMING,
  // has to be within angle and min / max.
  PIKE
};

struct NearestEnemData : public Menge::Agents::NearAgent {
  // const BaseAgent* enem;
  // float distSquared;
  float timeout;
  bool doWork;
  NearestEnemMethod method;

  NearestEnemData(const Menge::Agents::NearAgent obj);
  bool isStillRecent();
};

typedef std::map<size_t, NearestEnemData> NearestEnemDataMap;

class NearestEnemTask : public Menge::BFSM::Task {
  Menge::ReadersWriterLock _lock;

  NearestEnemDataMap _nearEnems;
  // internally track the number of enems targeting each agent
  // which may be useful.
  std::map<size_t, int> _numTargetedBy;
  static NearestEnemTask* TASK_PTR;
  void _getNearestTarget(
      const Menge::Agents::BaseAgent* agt, Menge::Agents::NearAgent& result) const;
  bool _doWorkData(size_t agent_id,
                 NearestEnemData& result) const;
  void _updateAimingTarget(const Menge::Agents::BaseAgent* agent,
                           Menge::Agents::NearAgent& result,
                           float max_angle = 3.14 * 2) const;
  void _updatePikeTarget(const Menge::Agents::BaseAgent* agent,
                           Menge::Agents::NearAgent& result,
                           float max_angle = 3.14 * 2) const;

 public:
  static NearestEnemTask* getSingleton();
  ~NearestEnemTask();
  NearestEnemTask();
  /*!
   *  @brief    Constructor
   *
   */

  /*!
   *  @brief    The work performed by the task.
   *
   *  @param    fsm   The finite state machine for the task to operate on.
   *  @throws   A TaskException if there was some non-fatal error
   *        in execution.  It should be logged.
   *  @throws   A TaskFatalException if there is a fatal error that
   *        should arrest execution of the simulation.
   */
  virtual void doWork(const Menge::BFSM::FSM* fsm) throw(
      Menge::BFSM::TaskException);

  bool getCurrentTarget(size_t id,
                        Menge::Agents::NearAgent& result) const;
  bool getCurrentTarget(const Menge::Agents::BaseAgent* agt,
                        Menge::Agents::NearAgent& result) const;

  /*!
   *  @brief    String representation of the task
   *
   *  @returns  A string containing task information.
   */
  virtual std::string toString() const;
  void addAgent(size_t agent_id, NearestEnemMethod method = MELEE);
  void removeAgent(size_t agent_id);

  /*!
   *  @brief    Reports if this task is "equivalent" to the given task.
   *        This makes it possible for a task to be redundantly added
   *        to the fsm without fear of duplication as the equivalent
   *        duplicates will be culled.
   *
   *  @param    task    The task to test against this one.
   *  @returns  A boolean reporting if the two tasks are equivalent (true)
   *        or unique (false).
   */
  virtual bool isEquivalent(const Menge::BFSM::Task* task) const;
};
}  // namespace Napoleon
#endif  // __NEAREST_ENEM_TASK_H__
