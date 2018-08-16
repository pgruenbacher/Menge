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

#include "NearestEnemTask.h"
#include "MengeCore/BFSM/FSM.h"
#include "MengeCore/Core.h"
#include <algorithm>

namespace Napoleon {

  using Menge::BFSM::FSM;
  using Menge::BFSM::Task;
  using Menge::BFSM::TaskException;
  using Menge::BFSM::TaskFactory;
  using Menge::Agents::BaseAgent;
  using Menge::Agents::NearAgent;

  /////////////////////////////////////////////////////////////////////
  //                   Implementation of NearestEnemTask
  /////////////////////////////////////////////////////////////////////

  // NearestEnemTask::NearestEnemTask() : Task(),{
  // }

  /////////////////////////////////////////////////////////////////////

  NearestEnemTask* NearestEnemTask::TASK_PTR = 0x0;

  NearestEnemData::NearestEnemData(const Menge::Agents::NearAgent obj )
    : NearAgent(obj) {}

  NearestEnemTask* NearestEnemTask::getSingleton() {
    if (TASK_PTR == 0x0) {
      TASK_PTR = new NearestEnemTask();
    }
    return TASK_PTR;
  }

  void NearestEnemTask::doWork( const FSM * fsm ) throw( TaskException ) {
    _numTargetedBy.clear();
    NearestEnemDataMap::iterator it;
    for (it = _nearEnems.begin(); it != _nearEnems.end(); it++) {
      _numTargetedBy[it->second.agent->_id] += 1;
    }
  }
  /////////////////////////////////////////////////////////////////////

  std::string NearestEnemTask::toString() const {
    return "NearestEnem Task";
  }

  NearAgent NearestEnemTask::_getNearestTarget(const BaseAgent* agent) {
    float distSq = 1000.f * 1000.f;
    NearAgent targetEnem(distSq, 0x0);
    for (Menge::Agents::NearAgent enem : agent->_nearEnems) {
      if (enem.distanceSquared < distSq) {
        distSq = enem.distanceSquared;
        targetEnem = enem;
      }
    }
    return targetEnem;
  }

  NearestEnemData NearestEnemTask::getCurrentTarget(const BaseAgent* agt) {
    const float delay = 1.f;

    // Not sure if we really need the lock safety
    // _lock.lockWrite();
    NearestEnemData d(Menge::Agents::NearAgent(100, 0x0));
    NearestEnemDataMap::iterator it = _nearEnems.find(agt->_id);
    if (it == _nearEnems.end()) {
      // std::cout << "WARNING!!!!" << std::endl;
    } else {
      d = it->second;
    }
    return d;
  }

  NearestEnemData NearestEnemTask::getTarget(const BaseAgent* agt) {
    const float delay = 1.f;

    // Not sure if we really need the lock safety
    // _lock.lockWrite();
    NearestEnemData d(Menge::Agents::NearAgent(100, 0x0));
    NearestEnemDataMap::iterator it = _nearEnems.find(agt->_id);

    if (it == _nearEnems.end()) {
      // since we construct from NearAgent, we can do this
      // for NearestEnemData
      d = _getNearestTarget(agt);
      d.timeout = Menge::SIM_TIME + delay;
      _nearEnems.insert(NearestEnemDataMap::value_type(agt->_id, d));
      // return d;
    } else if (it->second.timeout < Menge::SIM_TIME ||
               it->second.agent->isDead()) {
      // std::cout << "DELAY" << it->second.timeout << " " << Menge::SIM_TIME << std::endl;
      d = _getNearestTarget(agt);
      d.timeout = Menge::SIM_TIME + delay;
      it->second = d;
      // return d;
    } else {
      d = it->second;
    }
    // _lock.releaseWrite();
    if (d.agent == 0x0) {
      std::cout << "WARNING!!!!" << std::endl;
    }
    return d;
  }
  /////////////////////////////////////////////////////////////////////

  bool NearestEnemTask::isEquivalent( const Task * task ) const {
    const NearestEnemTask * other = dynamic_cast< const NearestEnemTask * >( task );
    if ( other == 0x0 ) {
      return false;
    }
    return true;
  }

  /////////////////////////////////////////////////////////////////////

} // namespace Formations
