/*

License

Menge
Copyright � and trademark � 2012-14 University of North Carolina at Chapel Hill.
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

#include "MengeCore/BFSM/GoalSet.h"

#include "MengeCore/BFSM/fsmCommon.h"
#include "MengeCore/BFSM/Goals/Goal.h"
#include "MengeCore/Math/consts.h"

#include <cassert>
#include <cmath>

namespace Menge {

	namespace BFSM {

		/////////////////////////////////////////////////////////////////////
		//					Implementation of GoalSet
		/////////////////////////////////////////////////////////////////////

		GoalSet::GoalSet():_goals(),_goalIDs(),_totalWeight(0.f),_randVal(0.f,1.f) {
		}

		/////////////////////////////////////////////////////////////////////

		GoalSet::~GoalSet() {
			// std::map< size_t, GoalPtr >::iterator itr = _goals.begin();
			// for ( ; itr != _goals.end(); ++itr ) {
			// 	itr->second->destroy();
			// }
		}

		/////////////////////////////////////////////////////////////////////

		bool GoalSet::addGoal( size_t id, GoalPtr goal ) {
			bool valid = false;
			_lock.lockRead();
			if ( _goals.find( id ) == _goals.end() ) {
				valid = true;
				goal->_goalSet = this;
				_goals[ id ] = goal;
				_goalIDs.push_back( id );
				_totalWeight += goal->_weight;
			}
			_lock.releaseRead();
			return valid;
		}

		/////////////////////////////////////////////////////////////////////

		GoalPtr GoalSet::getGoalByID( size_t id ) {
			GoalPtr goal;
			std::map< size_t, GoalPtr >::const_iterator itr = _goals.find( id );
			if ( itr != _goals.end() && itr->second->hasCapacity() ) {
				goal = itr->second;
			}
			return goal;
		}

		/////////////////////////////////////////////////////////////////////

		GoalPtr GoalSet::getGoalByIDConcurrent( size_t id ) {
			_lock.lockRead();
			GoalPtr goal = getGoalByID( id );
			_lock.releaseRead();
			return goal;
		}

		/////////////////////////////////////////////////////////////////////

		GoalPtr GoalSet::getIthGoal( size_t i ) {
			GoalPtr goal;
			if ( i < _goalIDs.size() ) {
				size_t id = _goalIDs[ i ];
				std::map< size_t, GoalPtr >::const_iterator itr = _goals.find( id );
				if (itr != _goals.end() && itr->second->hasCapacity() ) {
					goal = itr->second;
				}
			}
			return goal;
		}

		/////////////////////////////////////////////////////////////////////

		GoalPtr GoalSet::getIthGoalConcurrent( size_t i ) {
			_lock.lockRead();
			GoalPtr goal = getIthGoal( i );
			_lock.releaseRead();
			return goal;
		}

		/////////////////////////////////////////////////////////////////////

		size_t GoalSet::sizeConcurrent() const {
			_lock.lockRead();
			size_t s = _goalIDs.size();
			_lock.releaseRead();
			return s;
		}

		/////////////////////////////////////////////////////////////////////

		GoalPtr GoalSet::getRandomGoal() {
			GoalPtr goal;
			const size_t GOAL_COUNT = _goalIDs.size();
			if ( GOAL_COUNT > 0 ) {
				size_t idx = (size_t)( GOAL_COUNT * _randVal.getValue() );
				idx = idx < GOAL_COUNT ? idx : GOAL_COUNT - 1;
				size_t id = _goalIDs[ idx ];
				std::map< size_t, GoalPtr >::const_iterator itr = _goals.find( id );
				assert( itr != _goals.end() && "Somehow failed to get a random goal" );
				goal = itr->second;
			}
			return goal;
		}

		/////////////////////////////////////////////////////////////////////

		GoalPtr GoalSet::getRandomWeightedGoal() {
			// TODO: Change this to use _goalIDs as the key interface of available goals
			GoalPtr tgtGoal;
			if ( _goalIDs.size() > 0 ) {
				const float TGT_WEIGHT = _totalWeight * _randVal.getValue();

				std::map< size_t, GoalPtr >::const_iterator itr = _goals.find( _goalIDs[ 0 ] );
				assert( itr != _goals.end() && "A goalID does not map to a goal" );
				tgtGoal = itr->second;
				float accumWeight = tgtGoal->_weight;
				for ( size_t i = 1; i < _goalIDs.size(); ++i ) {
					if ( accumWeight > TGT_WEIGHT ) break;
					itr = _goals.find( _goalIDs[ i ] );
					assert( itr != _goals.end() && "A goalID does not map to a goal" );
					tgtGoal = itr->second;
					accumWeight += tgtGoal->_weight;
				}
			}
			return tgtGoal;
		}

		/////////////////////////////////////////////////////////////////////

		void GoalSet::setGoalFull( const Goal* goal ) const {
			size_t i = 0;
			std::map< size_t, GoalPtr >::const_iterator itr;
			while ( i < _goalIDs.size() ) {
				itr = _goals.find( _goalIDs[ i ] );
				assert( itr != _goals.end() && "A goalID does not map to a goal" );
				const GoalPtr testGoal = itr->second;
				if ( testGoal.get() == goal ) {
					_totalWeight -= goal->_weight;
					_goalIDs.erase( _goalIDs.begin() + i ); // todo: should this just be itr?
					break;
				} else {
					++i;
				}
			}
		}

		/////////////////////////////////////////////////////////////////////

		void GoalSet::setGoalAvailable( const Goal* goal ) const {
			const size_t GOAL_ID = goal->getID();
			_lock.lockWrite();
			assert( _goals.find( GOAL_ID ) != _goals.end() &&
					"Trying to set a goal available that doesn't belong to the goal set" );
	#ifdef _DEBUG
			bool found = false;
			for ( size_t i = 0; i < _goalIDs.size(); ++i ) {
				if ( _goalIDs[ i ] == GOAL_ID ) {
					found = true;
					break;
				}
			}
			assert( !found && "Trying to reactivate a goal that was never marked unavailable" );
	#endif
			_goalIDs.push_back( GOAL_ID );
			_totalWeight += goal->_weight;
			_lock.releaseWrite();
		}

	}	// namespace BFSM
}	// namespace Menge
