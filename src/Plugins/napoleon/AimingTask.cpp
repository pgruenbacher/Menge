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

#include "AimingTask.h"
#include "MengeCore/BFSM/FSM.h"
#include "MengeCore/Core.h"
#include <algorithm>
#include "./NearestEnemTask.h"
#include "MengeCore/Agents/SimulatorInterface.h"

namespace Napoleon {

  const int MAX_TARGETED = 8;

  using Menge::BFSM::FSM;
  using Menge::BFSM::Task;
  using Menge::BFSM::TaskException;
  using Menge::BFSM::TaskFactory;
  using Menge::Agents::BaseAgent;
  using Menge::Agents::NearAgent;

  /////////////////////////////////////////////////////////////////////
  //                   Implementation of AimingTask
  /////////////////////////////////////////////////////////////////////

  // AimingTask::AimingTask() : Task(),{
  // }

  /////////////////////////////////////////////////////////////////////

  AimingTask* AimingTask::TASK_PTR = 0x0;

  AimingEnemData::AimingEnemData(const Menge::Agents::NearAgent obj )
    : NearAgent(obj) {}

  AimingTask* AimingTask::getSingleton() {
    if (TASK_PTR == 0x0) {
      TASK_PTR = new AimingTask();
    }
    return TASK_PTR;
  }

  void AimingTask::doWork( const FSM * fsm ) throw( TaskException ) {
    _numTargetedBy.clear();
    AimingEnemDataMap::iterator it;
    for (it = _nearEnems.begin(); it != _nearEnems.end(); it++) {
      if (_getTarget(it->first, it->second)) {
        if (it->second.agent == 0x0) {
          std::cout << " !!!!!??!?!?!" << std::endl;
        }
        _numTargetedBy[it->second.agent->_id] += 1;
      }
    }
  }
  /////////////////////////////////////////////////////////////////////

  std::string AimingTask::toString() const {
    return "Aiming Task";
  }

  void AimingTask::addAgent(size_t id) {
    _lock.lockWrite();
    _nearEnems.insert(NearestEnemDataMap::value_type(id, AimingEnemData(NearAgent(1000, 0x0))));
    _lock.releaseWrite();
  }

  void AimingTask::removeAgent(size_t id) {
    _lock.lockWrite();
    _nearEnems.erase(id);
    _lock.releaseWrite();
  }

  bool AimingTask::getCurrentTarget(const BaseAgent* agt, NearAgent& result) const {
    // const float delay = 1.f;
    // Not sure if we really need the lock safety
    // _lock.lockWrite();
    // AimingEnemData d(Menge::Agents::NearAgent(100, 0x0));
    AimingEnemDataMap::const_iterator it = _nearEnems.find(agt->_id);
    if (it == _nearEnems.end()) {
      // std::cout << "WARNING!!!!" << std::endl;
      return false;
    } else {
      result.agent = it->second.agent;
      result.distanceSquared = it->second.distanceSquared;
    }
    return true;
  }

  bool AimingTask::_getTarget(size_t agent_id, AimingEnemData& result, float max_angle) const {
    const float delay = 1.f;
    BaseAgent* agent = Menge::SIMULATOR->getAgent(agent_id);

    if (result.agent == 0x0) {
      _updateTarget(agent, result, max_angle);
      result.timeout = Menge::SIM_TIME + delay;
    } else if (result.timeout < Menge::SIM_TIME || result.agent->isDead()) {
      _updateTarget(agent, result, max_angle);
      result.timeout = Menge::SIM_TIME + delay;
    } else {
      // no need to update.
    }

    if (result.agent == 0x0) {
      // std::cout << "WARNING!!!!" << std::endl;
      return false;
    }
    return true;

  }

  bool AimingTask::_updateTarget(const BaseAgent* agent, NearAgent& result, float max_angle) const {

    // we can try nearest enemies first...
    NearestEnemTask* task = NearestEnemTask::getSingleton();
    Vector2 dir;
    Vector2 pref_dir = agent->_velPref.getPreferred();
    float pref_angle = atan2(pref_dir.x(), pref_dir.y());
    // float max_angle = _angles[agent->_id];
    // std::cout << "GET TASK "<< std::endl;
    if (task->getCurrentTarget(agent, result)) {
      dir = result.agent->_pos - agent->_pos;
      float angle = atan2(dir.y(), dir.x());
      if (std::abs(angle - pref_angle) < max_angle) {
        return true;
      }
    }

    // no luck? ok we'll need to iterate through all agents then I guess to find
    // the closest ones that are with the angle of aiming.
    // since this only needs to be called once per agent when entering aiming
    // state, we don't need to optimize this with kdtree or anything, just iterate.
    // i don't think parallel is necessary either.

    int num_targeted = 0;

    const BaseAgent* agt;
    for (size_t i = 0; i < Menge::SIMULATOR->getNumAgents(); ++i) {
      agt = Menge::SIMULATOR->getAgent(i);
      // check that it's enemy
      if (agt->_class == agent->_class) continue;
      dir = agt->_pos - agent->_pos;
      float angle = atan2(dir.y(), dir.x());
      if (std::abs(angle - pref_angle) < max_angle) {
        if (_numTargetedBy.count(agt->_id) > 0) {
          num_targeted = _numTargetedBy.find(agt->_id)->second;
        }
        if (absSq(dir) < result.distanceSquared && num_targeted < MAX_TARGETED) {
          result.agent = agt;
          result.distanceSquared = absSq(dir);
        }
      }
    }

    if (result.agent == 0x0) return false;

    return true;
  }
  /////////////////////////////////////////////////////////////////////

  bool AimingTask::isEquivalent( const Task * task ) const {
    const AimingTask * other = dynamic_cast< const AimingTask * >( task );
    if ( other == 0x0 ) {
      return false;
    }
    return true;
  }

  /////////////////////////////////////////////////////////////////////

} // namespace Formations
