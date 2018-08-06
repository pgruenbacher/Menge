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

#include "AttackAction.h"

#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/Agents/SimulatorInterface.h"
#include "MengeCore/BFSM/FSM.h"
#include "MengeCore/BFSM/Actions/PropertyAction.h"
#include "MengeCore/BFSM/GoalSet.h"

#include <iostream>

namespace Napoleon {

    using Menge::Logger;
    using Menge::logger;
    using Menge::Agents::BaseAgent;
    using Menge::BFSM::Action;

    /////////////////////////////////////////////////////////////////////
    //                   Implementation of PropertyXAction
    /////////////////////////////////////////////////////////////////////

    AttackAction::AttackAction() : Action() {
    }

    /////////////////////////////////////////////////////////////////////

    AttackAction::~AttackAction() {
    }

    /////////////////////////////////////////////////////////////////////

    void AttackAction::onEnter( BaseAgent * agent ) {
        float distSq = 1000.f * 1000.f;
        Menge::Math::Vector2 target(0.f, 0.f);
        size_t agentId = sizeof(int);
        for (Menge::Agents::NearAgent enem : agent->_nearEnems) {
          if (enem.distanceSquared < distSq) {
            distSq = enem.distanceSquared;
            target = enem.agent->_pos;
            agentId = enem.agent->_id;
            // NearAgent.agent is const so we have
            // to grab agent from the fsm.
          }
        }
        if (agentId == sizeof(int)) return;
        BaseAgent* finalEnem = Menge::SIMULATOR->getAgent(agentId);
        agent->attacking = target;
        agent->isAttacking = true;
        // this shouldn't be occurring concurrenlty...
        // i think...
        finalEnem->adjustHealth(-10);
    }

    /////////////////////////////////////////////////////////////////////

    void AttackAction::leaveAction( BaseAgent * agent ) {
        agent->isAttacking = false;
    }

    /////////////////////////////////////////////////////////////////////
    //                   Implementation of PropertyXActFactory
    /////////////////////////////////////////////////////////////////////

    bool AttackActionFactory::setFromXML( Action * action, TiXmlElement * node,
                                          const std::string & behaveFldr ) const {
        AttackAction * pAction = dynamic_cast< AttackAction * >( action );
        assert( pAction != 0x0 &&
                "Trying to set property action properties on an incompatible object" );
        if ( ! ActionFactory::setFromXML( action, node, behaveFldr ) ) {
            return false;
        }
        return true;
    }
}   // namespace Napoleon
