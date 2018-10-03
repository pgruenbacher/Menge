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

#include "MengeCore/BFSM/Transitions/Transition.h"

#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/BFSM/FSM.h"
#include "MengeCore/BFSM/Transitions/Condition.h"
#include "MengeCore/BFSM/Transitions/ConditionDatabase.h"
#include "MengeCore/BFSM/Transitions/Target.h"
#include "MengeCore/BFSM/Transitions/TargetDatabase.h"

#include "thirdParty/tinyxml.h"

namespace Menge {

	namespace BFSM {

		/////////////////////////////////////////////////////////////////////
		//					Implementation of Transition
		/////////////////////////////////////////////////////////////////////

		Transition::Transition( const Transition & trans ) :
			_conditions( trans._conditions.begin(), trans._conditions.end() ), _target( trans._target->copy() ) {
		}

		/////////////////////////////////////////////////////////////////////

		Transition::Transition( std::vector<ConditionPtr>& conditions, TransitionTarget * target ) :
			_conditions(conditions), _target( target ) {
			// std::cout << "CONSTRUCTED" << std::endl;
			// for (Condition* cond : _conditions) {
			// 	cond->getTask();
			// }
		}

		/////////////////////////////////////////////////////////////////////

		Transition::~Transition() {
			// if ( _condition ) _condition->destroy();
			// for (Condition* cond : _conditions) {
			// 	cond->destroy();
			// }
			if ( _target ) _target->destroy();
		}

		/////////////////////////////////////////////////////////////////////

		bool Transition::connectStates( std::map< std::string, State * > & stateMap ) {
			assert( _target && "Connecting states for a transition with no target" );
			return _target->connectStates( stateMap );
		}

		/////////////////////////////////////////////////////////////////////

		void Transition::onEnter( Agents::BaseAgent * agent ) {
			assert( _conditions.size() > 0 && "Entering transition with no defined condition" );
			assert( _target && " Entering transition with no defined target" );
			for (const ConditionPtr cond : _conditions) {
				cond->onEnter( agent );
			}
			_target->onEnter( agent );
		}

		/////////////////////////////////////////////////////////////////////

		void Transition::onLeave( Agents::BaseAgent * agent ) {
			assert( _conditions.size() > 0 && "Leaving transition with no defined condition" );
			assert( _target && " Leaving transition with no defined target" );
			for (const ConditionPtr cond : _conditions) {
				cond->onLeave( agent );
			}
			_target->onLeave( agent );
		}

		/////////////////////////////////////////////////////////////////////

		State * Transition::test( Agents::BaseAgent * agent, const Goal * goal ) {
			bool anyFalse = false;
			for (const ConditionPtr cond : _conditions) {
				if (!cond->conditionMet(agent, goal)) {
					anyFalse = true;
				}
			}
			if ( !anyFalse ) {
				for (const ConditionPtr cond : _conditions) {
					cond->transitionWillPerform(agent);
				}

				return _target->nextState( agent );
			}
			return 0x0;
		}

		/////////////////////////////////////////////////////////////////////

		void Transition::getTasks( FSM * fsm ) {
			// std::cout << "GET TASKS" << _conditions.size();
			for (const ConditionPtr cond : _conditions) {
				Task* task = cond->getTask();
				fsm->addTask(task);
			}
			fsm->addTask( _target->getTask() );
		}

		/////////////////////////////////////////////////////////////////////

		Transition * Transition::copy() {
			return new Transition( *this );
		}

		/////////////////////////////////////////////////////////////////////

		Transition * parseTransition( TiXmlElement * node, const std::string & behaveFldr,
									  std::string & fromName ) {
			// 1) test "from" name - store if valid
			bool valid = true;
			const char * fromCStr = node->Attribute( "from" );
			if ( fromCStr == 0x0 ) {
				logger << Logger::ERR_MSG << "Transition defined on line " << node->Row();
				logger << " missing \"from\" attribute.";
				valid = false;
			}
			fromName = fromCStr;

			// 2) test "to" name - create SingleTarget if valid (and store)
			// Condition * condition = 0x0;
			TransitionTarget * target = 0x0;
			const char * toCStr = node->Attribute("to");
			if ( toCStr != 0x0 ) {
				target = new SingleTarget( std::string( toCStr ) );
			}
			// 3) Look for child tags: Condition and Target

			std::vector<ConditionPtr> conditions;
			for ( TiXmlElement * child = node->FirstChildElement();
				  child;
				  child = child->NextSiblingElement() ) {
				if ( child->ValueStr() == "Condition" ) {
					// condition = ConditionDB::getInstance( child, behaveFldr );
					ConditionPtr cond = ConditionPtr(ConditionDB::getInstance(child, behaveFldr));
					if (cond == 0x0) {
						logger << Logger::ERR_MSG << "Unrecognized child tag of a Condition on line ";
						logger << child->Row() << ": " << child->ValueStr() << ".";
						valid = false;

					}
					conditions.push_back(cond);
				} else if ( child->ValueStr() == "Target" ) {
					if ( target ) target->destroy();
					target = TargetDB::getInstance( child, behaveFldr );
				} else {
					logger << Logger::ERR_MSG << "Unrecognized child tag of a Transition on line ";
					logger << child->Row() << ": " << child->ValueStr() << ".";
					valid = false;
				}
			}

			valid = valid && conditions.size() > 0 && target != 0x0;
			// 4) If no Condition tag exists, it fails, if no Target tag exists && there was no to
			//		tag, it fails
			if ( ! valid ) {
				logger << Logger::ERR_MSG << "Missing target and/or condition specification for "
					"the Transition defined on line " << node->Row() << ".";
				// no need to destroy manually, the shared ptrs should get them out of the scope.
				// if ( conditions.size () > 0 ) {
				// 	for (Condition* cond : conditions) {
				// 		cond->destroy();
				// 	}
				// }
				if ( target ) target->destroy();
				return 0x0;
			}
			// If everything is valid, instantiate the transition, and the Transition
			// Transition* result = new Transition( conditions, target );
			// std::cout << "AFTER INIT" << std::endl;
			return new Transition( conditions, target );;
		}
	}	// namespace BFSM
}	// namespace Menge
