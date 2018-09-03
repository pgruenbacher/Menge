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

#include "DamageTask.h"
#include "MengeCore/BFSM/FSM.h"
#include "MengeCore/Agents/SimulatorInterface.h"
#include "MengeCore/Agents/BaseAgent.h"
// #include "MengeCore/Core.h"
// #include "MengeCore/Agents/BaseAgent.h"

namespace Napoleon {
  DamageTask* DamageTask::DAMAGE_TASK2 = 0x0;

  using Menge::BFSM::FSM;
  using Menge::BFSM::Task;
  using Menge::BFSM::TaskException;
  using Menge::BFSM::TaskFactory;

  // meant to synchronously apply damage to units.

  /////////////////////////////////////////////////////////////////////
  //                   Implementation of NearestEnemTask
  /////////////////////////////////////////////////////////////////////

  DamageTask::DamageTask() : Menge::BFSM::Task(), _randGenerator(-100.f, 0.f) {
  }

  /////////////////////////////////////////////////////////////////////
  DamageTask* DamageTask::getSingleton() {
    if (DAMAGE_TASK2 == 0x0) {
      DAMAGE_TASK2 = new DamageTask();
    }
    return DAMAGE_TASK2;
  }

  void DamageTask::adjustHealth(size_t agentId, float health) {
    _lock.lockWrite();
    if (_damages.find(agentId) == _damages.end()) {
      _damages[agentId] = health;
    } else {
      _damages[agentId] += health;
    }
    _lock.releaseWrite();
  }

  void DamageTask::doWork( const FSM * fsm ) throw( TaskException ) {
    // update the rand generator each timestep
    _randGenerator.getValue();
    // std::cout << " GET AGENT ATATC " << getAgentAttackValue(15) << std::endl;

    Menge::Agents::BaseAgent* finalEnem;
    for (auto entry : _damages) {
      size_t agentId = entry.first;
      float health = entry.second;
      finalEnem  = Menge::SIMULATOR->getAgent(agentId);
      std::cout << "ADJUST " << finalEnem->_id << " TIME " << Menge::SIM_TIME << std::endl;
      finalEnem->adjustHealth(health);
    }
    _damages.clear();
  }
  /////////////////////////////////////////////////////////////////////

  std::string DamageTask::toString() const {
    return "Damage Task";
  }

  /////////////////////////////////////////////////////////////////////

  bool DamageTask::isEquivalent( const Task * task ) const {
    const DamageTask * other = dynamic_cast< const DamageTask * >( task );
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
