/*

License

Menge
Copyright © and trademark ™ 2012-14 University of North Carolina at Chapel Hill.
All rights reserved.

Permission to use, copy, modify, and distribute this software and its documentation
for educational, research, and non-profit purposes, without fee, and without a
written agreement is hereby granted, provided that the above copyright notice,
this paragraph, and the following four paragraphs appear in all copies.

This software program and documentation are copyrighted by the University of North
Carolina at Chapel Hill. The software program and documentation are supplied "as is,"
without any accompanying services from the University of North Carolina at Chapel
Hill or the authors. The University of North Carolina at Chapel Hill and the
authors do not warrant that the operation of the program will be uninterrupted
or error-free. The end-user understands that the program was developed for research
purposes and is advised not to rely exclusively on the program for any reason.

IN NO EVENT SHALL THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL OR THE AUTHORS
BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS
DOCUMENTATION, EVEN IF THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL OR THE
AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL AND THE AUTHORS SPECIFICALLY
DISCLAIM ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE AND ANY STATUTORY WARRANTY
OF NON-INFRINGEMENT. THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND
THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL AND THE AUTHORS HAVE NO OBLIGATIONS
TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

Any questions or comments should be sent to the authors {menge,geom}@cs.unc.edu

*/

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
  using Menge::BFSM::TaskFactory;

  // meant to synchronously apply damage to units.

  /////////////////////////////////////////////////////////////////////
  //                   Implementation of NearestEnemTask
  /////////////////////////////////////////////////////////////////////

  StaggerTask::StaggerTask() : Menge::BFSM::Task() {
  }

  /////////////////////////////////////////////////////////////////////
  StaggerTask* StaggerTask::getSingleton() {
    if (STAGGER_TASK == 0x0) {
      STAGGER_TASK = new StaggerTask();
    }
    return STAGGER_TASK;
  }

  void StaggerTask::doWork( const FSM * fsm ) throw( TaskException ) {
    // std::cout << "CLEAR " << std::endl;
    // copy to the read-only map for the concurrency.
    _stagger_map = _pending_stagger;
    // _stagger_map.clear();
    // _stagger_map.insert(_pending_stagger.begin(), _pending_stagger.end());
    // std::cout << "CLEAR2 " << std::endl;
    // std::cout << " GET AGENT ATATC " << getAgentAttackValue(15) << std::endl;
  }
  /////////////////////////////////////////////////////////////////////

  void StaggerTask::setStaggerComplete(size_t agentId) {
    _lock.lockWrite();
    _pending_stagger.erase(agentId);
    _lock.releaseWrite  ();
  }

  void StaggerTask::setCanStagger(size_t agentId, Vector2 force) {
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

} // namespace Formations
