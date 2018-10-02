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
#include "PikeTask.h"

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
    : NearAgent(obj), doWork(false), timeout(0.f), method(MELEE) {}

  bool NearestEnemData::isStillRecent() {
    if (Menge::SIM_TIME - timeout < 5.f) return true;
    return false;
  }

  NearestEnemTask* NearestEnemTask::getSingleton() {
    if (TASK_PTR == 0x0) {
      TASK_PTR = new NearestEnemTask();
    }
    return TASK_PTR;
  }

  NearestEnemTask::NearestEnemTask() {

  }

  NearestEnemTask::~NearestEnemTask() {
    // make sure to clear teh singleton.
    TASK_PTR = 0x0;
  }

  void NearestEnemTask::doWork( const FSM * fsm ) throw( TaskException ) {
    _numTargetedBy.clear();
    NearestEnemDataMap::iterator it;
    for (it = _nearEnems.begin(); it != _nearEnems.end(); it++) {
      if (!it->second.doWork) continue;
      if (_doWorkData(it->first, it->second)) {
        if (it->second.agent == 0x0) {
          std::cout << " !!!!!??!?!?!" << std::endl;
        }
        _numTargetedBy[it->second.agent->_id] += 1;
      }
    }
  }
  /////////////////////////////////////////////////////////////////////

  std::string NearestEnemTask::toString() const {
    return "NearestEnem Task";
  }


  bool NearestEnemTask::getCurrentTarget(size_t id, NearAgent& result) const {
    NearestEnemData d(Menge::Agents::NearAgent(100, 0x0));
    NearestEnemDataMap::const_iterator it = _nearEnems.find(id);
    if (it == _nearEnems.end()) {
      return false;
    } else {
      result = it->second;
    }
    if (result.agent == 0x0) return false;
    if (result.agent->isDead()) return false;
    return true;

  }
  bool NearestEnemTask::getCurrentTarget(const BaseAgent* agt, NearAgent& result) const {
    // const float delay = 1.f;

    // Not sure if we really need the lock safety
    // _lock.lockWrite();
    NearestEnemData d(Menge::Agents::NearAgent(100, 0x0));
    NearestEnemDataMap::const_iterator it = _nearEnems.find(agt->_id);
    if (it == _nearEnems.end()) {
      return false;
    } else {
      result = it->second;
    }
    if (result.agent == 0x0) return false;
    if (result.agent->isDead()) return false;
    return true;
  }

  void NearestEnemTask::addAgent(size_t id, NearestEnemMethod method) {
    _lock.lockWrite();
    NearestEnemDataMap::iterator it = _nearEnems.find(id);
    if (it == _nearEnems.end()) {
      NearestEnemData d = NearestEnemData(NearAgent(1000, 0x0));
      d.doWork = true;
      d.method = method;
      _nearEnems.insert(NearestEnemDataMap::value_type(id, d));
    } else {
      it->second.doWork = true;
      it->second.method = method;
      // it->secon
    }
    _lock.releaseWrite();
  }

  void NearestEnemTask::removeAgent(size_t id) {
    _lock.lockWrite();
    // _nearEnems.erase(id);
    NearestEnemDataMap::iterator it = _nearEnems.find(id);
    if (it != _nearEnems.end()) {
      it->second.doWork = false;
    }
    _lock.releaseWrite();
  }

  bool NearestEnemTask::_doWorkData(size_t agent_id, NearestEnemData& result) const {
    const float delay = 1.f;
    BaseAgent* agent = Menge::SIMULATOR->getAgent(agent_id);

    if (
      result.agent == 0x0 ||
      result.timeout < Menge::SIM_TIME ||
      result.agent->isDead()
    ) {
      if (result.method == MELEE) {
        _getNearestTarget(agent, result);
      } else if (result.method == AIMING) {
        _updateAimingTarget(agent, result);
      } else if (result.method == PIKE) {
        _updatePikeTarget(agent, result);
      }
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
  /////////////////////////////////////////////////////////////////////

  bool NearestEnemTask::isEquivalent( const Task * task ) const {
    const NearestEnemTask * other = dynamic_cast< const NearestEnemTask * >( task );
    if ( other == 0x0 ) {
      return false;
    }
    return true;
  }


  void NearestEnemTask::_getNearestTarget(const BaseAgent* agent, NearAgent& result) const {
    float distSq = 1000.f * 1000.f;
    // NearAgent targetEnem(distSq, 0x0);
    for (Menge::Agents::NearAgent enem : agent->_nearEnems) {
      if (enem.distanceSquared < distSq) {
        distSq = enem.distanceSquared;
        result = enem;
      }
    }
    return;
  }

  void NearestEnemTask::_updatePikeTarget(const Menge::Agents::BaseAgent* agent, Menge::Agents::NearAgent& result, float max_angle) const {
    float distSq = 1000.f * 1000.f;
    // NearAgent targetEnem(distSq, 0x0);
    Vector2 dir;
    Vector2 pref_dir = agent->_velPref.getPreferred();
    float pref_angle = atan2(pref_dir.x(), pref_dir.y());
    float current_max = max_angle;
    const float MIN_PIKE = 1.5 * 1.5;

    // for (Menge::Agents::NearAgent enem : agent->_nearEnems) {
    std::vector<NearAgent> enems;
    PikeTask::getSingleton()->getNearbyAgents(agent->_id, enems);
    for (const Menge::Agents::NearAgent& enem : enems) {
      if (enem.distanceSquared < distSq) {
        dir = enem.agent->_pos - agent->_pos;
        float angle = atan2(dir.y(), dir.x());
        float angle_diff = std::abs(angle - pref_angle);

        // claculate enemDistanceSquared manually since the query for pike task
        // is baed on distance to the tip of hte pike.
        const float distanceSquared = absSq(dir);

        if (angle_diff > std::min(current_max, max_angle)) continue;
        if (distanceSquared < (MIN_PIKE)) continue;
        distSq = enem.distanceSquared;
        result = enem;
      }
    }

    // if (result.agent && agent->_id == Menge::SIMULATOR->getSelectedAgentId()) {
    //   std::cout << " PIKE TARGET " << result.agent->_id << std::endl;
    // }

    return;
  }

  void NearestEnemTask::_updateAimingTarget(const Menge::Agents::BaseAgent* agent, Menge::Agents::NearAgent& result, float max_angle) const {
    // we can try nearest enemies first...
    const int MAX_TARGETED = 8;

    // if ()
    // NearestEnemTask* task = NearestEnemTask::getSingleton();
    Vector2 dir;
    Vector2 pref_dir = agent->_velPref.getPreferred();
    float pref_angle = atan2(pref_dir.x(), pref_dir.y());
    // // float max_angle = _angles[agent->_id];
    // // std::cout << "GET TASK "<< std::endl;

    // First we'll iterate over all nearby agents
    // and find if any are close enough within the angle.
    float current_max = max_angle;
    for (Menge::Agents::NearAgent enem : agent->_nearEnems) {
      result = enem;
      dir = result.agent->_pos - agent->_pos;
      float angle = atan2(dir.y(), dir.x());
      float angle_diff = std::abs(angle - pref_angle);
      if (angle_diff < std::min(current_max, max_angle)) {
        current_max = angle_diff;
      }
    }
    // success
    if (result.agent != 0x0) return;

    // no luck? ok we'll need to iterate through all agents then I guess to find
    // the closest ones that are with the angle of aiming.
    // since this only needs to be called once per agent when entering aiming
    // state, we don't need to optimize this with kdtree or anything, just iterate.
    // i don't think parallel is necessary either.

    int num_targeted = 0;

    const BaseAgent* agt;
    float diff_angle;
    float localDistSquared = 1e15;
    for (size_t i = 0; i < Menge::SIMULATOR->getNumAgents(); ++i) {
      agt = Menge::SIMULATOR->getAgent(i);
      // check that it's enemy
      if (agt->_class == agent->_class) continue;
      dir = agt->_pos - agent->_pos;
      float angle = atan2(dir.y(), dir.x());
      diff_angle = std::abs(angle - pref_angle);
      if ( diff_angle < max_angle) {
        if (_numTargetedBy.count(agt->_id) > 0) {
          num_targeted = _numTargetedBy.find(agt->_id)->second;
        }
        if (absSq(dir) < localDistSquared && num_targeted < MAX_TARGETED) {
          result.agent = agt;
          result.distanceSquared = absSq(dir);
          localDistSquared = result.distanceSquared;
        }
      }
    }
    // std::cout << "AGENT ? " << result.agent << " " << diff_angle << " " << max_angle << result.distanceSquared << std::endl;
    // if (result.agent == 0x0) return false;

    // return true;
  }
  /////////////////////////////////////////////////////////////////////

} // namespace Formations
