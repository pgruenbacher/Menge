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

#include "PikeTask.h"
#include "MengeCore/BFSM/FSM.h"
#include "MengeCore/Agents/SimulatorInterface.h"
#include "MengeCore/Agents/SpatialQueries/SpatialQuery.h"

namespace Napoleon {
  PikeTask* PikeTask::PIKE_TASK = 0x0;

  using Menge::BFSM::FSM;
  using Menge::BFSM::Task;
  using Menge::BFSM::TaskException;
  using Menge::BFSM::TaskFactory;



  // 1.04719 radians == 60 degrees == 0.5 dot product.
  PikeProximityQuery::PikeProximityQuery(Vector2 pos, Vector2 dir) :
  queryPoint(pos), queryDirection(dir), queryDotProduct(0.5)
  {}


  bool PikeProximityQuery::containsAgent(const Menge::Agents::BaseAgent* agent) const {
    for (const Menge::Agents::NearAgent& agt : _agentResults) {
      if (agt.agent == agent) return true;
    }
    return false;
  }

  void PikeProximityQuery::filterAgent(const Menge::Agents::BaseAgent * agent, float distanceSquared){
    using Menge::Agents::NearAgent;
    //if we have enough agents, just exit cause there's likely some sort of issue
    // going on. the maxAgnetResults is just to make sure we don't overwhelm the list.
    // it should be a big number since we carea bout all agents that are near a pike danger area.
    if (distanceSquared > _maxAgentResultDistance) return;
    if (_agentResults.size() == _maxAgentResults) return;
    Vector2 agentDir = agent->_pos - queryPoint;
    agentDir.normalize();
    // check if dot product. if > 0.5, then pike if facing 60 deg angle of agent.
    if (queryDirection * (agentDir) < queryDotProduct) return;
    _agentResults.push_back(NearAgent(distanceSquared,agent));

    // we don't care about hte order of the nearest agents....
    // //we are trusting that results are in order
    // size_t i = _agentResults.size() - 1;
    // while (i != 0 && distanceSquared < _agentResults[i-1].distanceSquared) {
    //   _agentResults[i] = _agentResults[i-1];
    //   --i;
    // }

    // _agentResults[i] = NearAgent(distanceSquared,agent);

    // if (distanceSquared > _maxAgentResultDistance)
    //   _maxAgentResultDistance = distanceSquared;

  };

  Pike::Pike(Vector2 pos, Vector2 dir) : pos(pos), direction(dir), query(pos, dir) {

  }


  PikeTask::PikeTask() : Menge::BFSM::Task(), _pikes() {
  }

  /////////////////////////////////////////////////////////////////////
  PikeTask* PikeTask::getSingleton() {
    if (PIKE_TASK == 0x0) {
      PIKE_TASK = new PikeTask();
    }
    return PIKE_TASK;
  }

  void PikeTask::adjustPike(const Menge::Agents::BaseAgent& agent,
    bool is_lowered, Vector2 pos, Vector2 dir) {
    _lock.lockWrite();

    PikeMap::iterator it = _pikes.find(agent._id);
    if (it != _pikes.end() && !is_lowered) {
      _pikes.erase(agent._id);

    } else if (it == _pikes.end()) {
      _pikes.insert(PikeMap::value_type(agent._id, Pike(pos, dir)));

    } else {
      it->second = Pike(pos, dir);
    }

    _lock.releaseWrite();
  }

  void PikeTask::doWork( const FSM * fsm ) throw( TaskException ) {
    Menge::Agents::SpatialQuery* sp = Menge::SIMULATOR->getSpatialQuery();
    /* For each iteration we'll get all nearby agents that are within a certain area of the pikes and
    cache the result */
    PikeMap::iterator it = _pikes.begin();
    for ( ; it != _pikes.end(); ++it) {
      Pike& v = it->second;
      sp->agentQuery(&v.query);
    }

  }
  /////////////////////////////////////////////////////////////////////

  std::string PikeTask::toString() const {
    return "Pike Task";
  }

  bool PikeTask::isAgentFacingPike(const Menge::Agents::BaseAgent* agt) const {
    PikeMap::const_iterator it = _pikes.begin();
    for ( ; it != _pikes.end(); ++it) {
      if (it->second.query.containsAgent(agt)) return true;
    }
    return false;
  }

  /////////////////////////////////////////////////////////////////////

  bool PikeTask::isEquivalent( const Task * task ) const {
    const PikeTask * other = dynamic_cast< const PikeTask * >( task );
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
