#ifndef _STAGGER_TASK_H_
#define _STAGGER_TASK_H_

#include "MengeCore/BFSM/Tasks/Task.h"
#include "MengeCore/BFSM/Tasks/TaskFactory.h"
#include "MengeCore/BFSM/fsmCommon.h"

#include <map>
#include <string>

#include "MengeCore/Math/RandGenerator.h"
#include "MengeCore/Runtime/ReadersWriterLock.h"
#include "thirdParty/tinyxml.h"

using namespace Menge;
namespace Napoleon {

struct StaggerData {
  Vector2 force;
};

/*!
* @brief  Task responsible for updating agent data for maintaining a formation.
*/
class StaggerTask : public Menge::BFSM::Task {
 private:
  ReadersWriterLock _lock;
  typedef std::map<size_t, StaggerData> StaggerMap;
  StaggerMap _stagger_map;
  StaggerMap _pending_stagger;

  static StaggerTask* STAGGER_TASK;

 public:
  /*!
   *  @brief    Constructor
   *
   */
  StaggerTask();
  ~StaggerTask();

  // float getAgentAttackValue(size_t agentId) const {
  //   // it's ok for concurrent reading...
  //   return _randGenerator.getValue(agentId);
  // }

  bool canStagger(size_t agentId) const;
  void setStaggerComplete(size_t agentId);
  void setCanStagger(size_t agentId, Vector2 force);
  Vector2 getStaggerForce(size_t agentId) const;
  /*!
   *  @brief    The work performed by the task.
   *
   *  @param    fsm   The finite state machine for the task to operate on.
   *  @throws   A TaskException if there was some non-fatal error
   *        in execution.  It should be logged.
   *  @throws   A TaskFatalException if there is a fatal error that
   *        should arrest execution of the simulation.
   */
  virtual void doWork(const Menge::BFSM::FSM* fsm);

  /*!
   *  @brief    String representation of the task
   *
   *  @returns  A string containing task information.
   */
  virtual std::string toString() const;

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

  static StaggerTask* getSingleton();
};

}  // namespace Napoleon
#endif  // _STAGGER_TASK_H_
