#include "StaggerTask.h"
#include "MengeCore/BFSM/FSM.h"
#include "MengeCore/Agents/SimulatorInterface.h"
#include "MengeCore/Agents/BaseAgent.h"
// #include "MengeCore/Core.h"
// #include "MengeCore/Agents/BaseAgent.h"

namespace Napoleon {
  StaggerTask* StaggerTask::STAGGER_TASK = 0x0;

  using Menge::BFSM::FSM;
  using Menge::BFSM::Task;
  using Menge::BFSM::TaskException;

  // meant to synchronously apply damage to units.

  /////////////////////////////////////////////////////////////////////
  //                   Implementation of NearestEnemTask
  /////////////////////////////////////////////////////////////////////

  StaggerTask::StaggerTask() : Menge::BFSM::Task() {
  }
  StaggerTask::~StaggerTask() {
    STAGGER_TASK = 0x0;
    // make sure to clear the singleton if deleting.
  }

  /////////////////////////////////////////////////////////////////////
  StaggerTask* StaggerTask::getSingleton() {
    if (STAGGER_TASK == 0x0) {
      STAGGER_TASK = new StaggerTask();
    }
    return STAGGER_TASK;
  }

  void StaggerTask::doWork( const FSM * fsm )  {
    // std::cout << "CLEAR " << std::endl;
    // copy to the read-only map for the concurrency.
    _stagger_map = _pending_stagger;
  }
  /////////////////////////////////////////////////////////////////////

  void StaggerTask::setStaggerComplete(size_t agentId) {
    // we write to pending stagger and then copy to a read-only buffer
    // after the doWork phase.
    _lock.lockWrite();
    _pending_stagger.erase(agentId);
    _lock.releaseWrite  ();
  }

  void StaggerTask::setCanStagger(size_t agentId, Vector2 force) {
    // we write to pending stagger and then copy to a read-only buffer
    // after the doWork phase.
    _lock.lockWrite();
    _pending_stagger[agentId] = StaggerData{force};
    _lock.releaseWrite();
  }

  Vector2 StaggerTask::getStaggerForce(size_t agentId) const {
    auto const find_iter = _stagger_map.find(agentId);
    if (find_iter != _stagger_map.end()) {
      return find_iter->second.force;
    }
    return Vector2(0.f, 0.f);
  }

  bool StaggerTask::canStagger(size_t agentId) const {
    auto const find_iter = _stagger_map.find(agentId);
    if (find_iter != _stagger_map.end()) {
      return true;
    }
    return false;
  }

  std::string StaggerTask::toString() const {
    return "Stagger Task";
  }

  /////////////////////////////////////////////////////////////////////

  bool StaggerTask::isEquivalent( const Task * task ) const {
    const StaggerTask * other = dynamic_cast< const StaggerTask * >( task );
    if ( other == 0x0 ) {
      return false;
    }
    // if (task == this) {
    //   std
    // }
    return true;
  }

  /////////////////////////////////////////////////////////////////////

} // namespace Napoleon
