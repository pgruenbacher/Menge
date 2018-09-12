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
      _numTargetedBy[it->second.agent->_id] += 1;
    }
  }
  /////////////////////////////////////////////////////////////////////

  std::string AimingTask::toString() const {
    return "Aiming Task";
  }

  bool AimingTask::getCurrentTarget(const BaseAgent* agt, NearAgent& result) {
    const float delay = 1.f;

    // Not sure if we really need the lock safety
    // _lock.lockWrite();
    AimingEnemData d(Menge::Agents::NearAgent(100, 0x0));
    AimingEnemDataMap::iterator it = _nearEnems.find(agt->_id);
    if (it == _nearEnems.end()) {
      // std::cout << "WARNING!!!!" << std::endl;
      return false;
    } else {
      d = it->second;
    }
    result.agent = d.agent;
    result.distanceSquared = d.distanceSquared;
    return true;
  }

  bool AimingTask::getTarget(const BaseAgent* agent, NearAgent& result, float max_angle) {
    const float delay = 1.f;

    // Not sure if we really need the lock safety
    // _lock.lockWrite();
    AimingEnemData d(Menge::Agents::NearAgent(100, 0x0));
    AimingEnemDataMap::iterator it = _nearEnems.find(agent->_id);

    if (it == _nearEnems.end()) {
      // since we construct from NearAgent, we can do this
      // for NearestEnemData
      _updateTarget(agent, d, max_angle);
      d.timeout = Menge::SIM_TIME + delay;
      _nearEnems.insert(NearestEnemDataMap::value_type(agent->_id, d));
      // return d;
    } else if (it->second.timeout < Menge::SIM_TIME ||
               it->second.agent->isDead()) {
      // std::cout << "DELAY" << it->second.timeout << " " << Menge::SIM_TIME << std::endl;
      _updateTarget(agent, d, max_angle);
      d.timeout = Menge::SIM_TIME + delay;
      it->second = d;
      // return d;
    } else {
      d = it->second;
    }
    // _lock.releaseWrite();
    if (d.agent == 0x0) {
      // std::cout << "WARNING!!!!" << std::endl;
      return false;
    }
    // update the result.
    result.agent = d.agent;
    result.distanceSquared = d.distanceSquared;
    return true;

  }

  bool AimingTask::_updateTarget(const BaseAgent* agent, NearAgent& result, float max_angle) {

    // we can try nearest enemies first...
    NearestEnemTask* task = NearestEnemTask::getSingleton();
    // NearAgent d(1000000, 0x0);
    Vector2 dir;
    Vector2 pref_dir = agent->_velPref.getPreferred();
    float pref_angle = atan2(pref_dir.x(), pref_dir.y());
    // float max_angle = _angles[agent->_id];
    if (task->getTarget(agent, result)) {
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
    const BaseAgent* agt;
    for (size_t i = 0; i < Menge::SIMULATOR->getNumAgents(); ++i) {
      agt = Menge::SIMULATOR->getAgent(i);
      // check that it's enemy
      if (agt->_class == agent->_class) continue;
      dir = agt->_pos - agent->_pos;
      float angle = atan2(dir.y(), dir.x());
      if (std::abs(angle - pref_angle) < max_angle) {
        if (absSq(dir) < result.distanceSquared) {
          result.agent = agt;
          result.distanceSquared = absSq(dir);
        }
      }
    }

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
