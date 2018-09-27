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

#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/Math/Geometry2D.h"
#include "MengeCore/Agents/Obstacle.h"
#include "MengeCore/Core.h"
#include <algorithm>

// may want to remove this when integrating with godot...
// #include "MengeVis/SceneGraph/Select.h"

namespace Menge {

	namespace Agents {

		using Math::Vector2;

		////////////////////////////////////////////////////////////////
		//					Implementation of BaseAgent
		////////////////////////////////////////////////////////////////

		BaseAgent::BaseAgent() {
			_maxSpeed = 2.5f;
			_maxAccel = 2.f;
			_prefSpeed = 1.34f;
			_pos = Vector2( 0.f, 0.f );
			_vel = Vector2( 0.f, 0.f );
			_velPref = PrefVelocity( Vector2(1.f,0.f), _prefSpeed, Vector2(0.f,0.f) );
			_velNew = Vector2( 0.f, 0.f );
			_orient = Vector2( 1.f, 0.f );
			_maxAngVel = TWOPI;	// 360 degrees/sec
			_maxNeighbors = 10;
			_neighborDist = 5.f;
			// the max number of enem agents to track
			_maxEnem = 10;
			// the max dist of enem agents to track
			_enemDist = 15.f;
			_nearAgents.clear();
			_nearEnems.clear();
			_nearObstacles.clear();
			_class = 0;
			_obstacleSet = 0xFFFFFFFF;
			_priority = 0.f;
			_id = 0;
			_radius = 0.19f;
			_health = 50;
			visible = true;

			isAttacking = false;
			attacking = Vector2(0.f, 0.f);
		}

		////////////////////////////////////////////////////////////////

		void BaseAgent::initialize() {
			_orient = _velPref.getPreferred();
			if (_orient.x() == 0 && _orient.y() == 0) {
				_orient = Vector2(1.f, 0.f);
			}
		}

		////////////////////////////////////////////////////////////////

		void BaseAgent::update( float timeStep ) {
			float delV = abs( _vel - _velNew );
			//Check to see if new velocity violates acceleration constraints...
			// TODO: Make the acceleration constraint respect collisions (particularly with
			//		obstacles.  I.e. confirm that the new velocity STILL won't collide
			//		with the neighboring obstacles.
			//	The slick way to do this is to replace the float in the _nearObstacles to
			//		represent the minimum acceleration required to avoid collision with the
			//		obstacle in the next time step.  Then I can simply take the larger of the
			///		user-define max acceleration and the smallest required acceleration
			if ( delV > _maxAccel * timeStep ) {
				float w = _maxAccel * timeStep / delV;
				_vel = (1.f - w ) * _vel + w * _velNew;
			} else {
				_vel = _velNew;
			}
			_pos += _vel * timeStep;

			updateOrient( timeStep );
			postUpdate();
		}

		void BaseAgent::adjustHitChance( float& hitChance, const BaseAgent* enem) const {
			Vector2 dir = _pos - enem->_pos;
			dir.normalize();
			// std::cout << "DIR " << dir << " " << "_orient " << _orient << std::endl;
			float dotProduct = dir * _orient;

			float dotVel = _vel * dir;
			// std::cout << "DOT VEL " << dotVel << " " << (_vel) << std::endl;
			// if charging and moving towards opponent...
			if (abs(_vel) > 0.5 && dotVel > 0) {
				hitChance += 50.0;
			}

			if (std::abs(dotProduct) < 0.5) {
				// sideways
				// std::cout << "SIDE " << dotProduct << " " << std::abs(dotProduct) << std::endl;
				hitChance += 0.0;
			} else if (dotProduct <= -0.5) {
				// facing directly
				// std::cout << "FRONT " << dotProduct << std::endl;
				hitChance -= 30.0;
			} else if (dotProduct >= 0.5) {
				// behind agent.
				// std::cout << "BEHIN " << dotProduct << std::endl;
				hitChance += 80.0;
			} else {
				std::cout << "UNEPXECTED VALUE " << dotProduct << std::endl;
			}
		}

		////////////////////////////////////////////////////////////////

		void BaseAgent::updateOrient( float timeStep ) {
			// This stabilizes orientation
			//	As the agent slows down, the target orientation becomes preferred direction.
			//	We define a threshold as a fraction of preferred speed.
			//	If the agents speed is at or above that threshold, orientation is defined by the
			//  movement vector.
			//  If the speed is zero, the orientation is the direction of preferred velocity.
			//  The transition function is designd such that the transition from movement direction
			//  to preferred movement direction falls off slowly (initially) and rapidly at low
			// speeds.
			if (_orient.x() == 0 && _orient.y() == 0) {
				_orient.set(Vector2(1.f, 0.f));
			}
			float speed = abs( _vel );
			const float speedThresh = _prefSpeed / 3.f;
			Vector2 prefVel = _velPref.getPreferredVel();
			Vector2 newOrient( _orient );	// by default new is old
			Vector2 moveDir = _vel / speed;
			bool hasMinPrefVel = absSq(prefVel) > 0.000001f;
			Vector2 targetDir = _velPref.getTarget() - _pos;
			if ( speed >= speedThresh ) {
				newOrient = moveDir;
				newOrient.normalize();
			} else if (speed == 0.0 && hasMinPrefVel) {
				newOrient = prefVel;
				newOrient.normalize();
			} else if (speed < speedThresh && _velPref.hasTarget() && !(targetDir.x() == 0 && targetDir.y() == 0)) {
				// if slow enough, face target.
				// make sure we're not already on the target...
				newOrient = targetDir;
				newOrient.normalize();
			} else {
				newOrient = _orient;
				// newOrient = targetDir;
				// newOrient.normalize();
				// prefDir *can* be zero if we've arrived at goal.  Only use it if it's non-zero.
				// if ( hasMinPrefDir ) {
				// 	// so if travelling backwards...
				// 	float frac = sqrtf( speed / speedThresh );
				// 	newOrient = frac * moveDir + ( 1.f - frac ) * prefDir;
				// 	newOrient.normalize();
				// }
			}

			// if (_id == 40) {
			// 	std::cout << "NEW ORI " << newOrient.x() << " " << newOrient.y() << std::endl;
			// 	std::cout << "ORI " << _orient.x() << " " << _orient.y() << std::endl;
			// 	std::cout << "hasMin " << int(hasMinPrefDir) << std::endl;
			// 	std::cout << "hasMin " << int(_velPref.hasTarget()) << std::endl;
			// 	std::cout << "targetDir " << targetDir.x() << " " << targetDir.y() << std::endl;
			// 	std::cout << "Ve " << _vel.x() << " " << _vel.y() << std::endl;
			// 	std::cout << "CT " << newOrient * _orient << std::endl;
			// }
			// unsigned int name = MengeVis::SceneGraph::Selectable::getSelectedName();
			// if (name == _id) {
			// 	std::cout << "SELECTED " << _id << std::endl;
			// 	std::cout << "NEW ORIENT " << newOrient << " cur " << _orient << std::endl;
			// }


			// TODO(curds01): At low speeds, small movement perturbations cause radically different
			// orientation changes.  It seems *reasonable* to scale maximum angular velocity
			// by the travel speed (in some sense) to prevent this.  HOWEVER, this would break
			// agents that have a sense of facing direction that they actively control.
			// std::cout << " MAX ANGLE VEL " << _maxAngVel;
			// Now limit angular velocity.
			const float MAX_ANGLE_CHANGE = timeStep * _maxAngVel;
			float ct = newOrient * _orient;
			if ( acos(ct) > MAX_ANGLE_CHANGE ) {
				// std::cout << "ACOS " << _maxAngVel << std::endl;
				// changing direction at a rate greater than _maxAngVel
				float maxSt = sin( MAX_ANGLE_CHANGE );
				float maxCt = cos( MAX_ANGLE_CHANGE );
				if ( det( _orient, newOrient ) > 0.f ) {
					// rotate _orient left
					_orient.set( maxCt * _orient._x - maxSt * _orient._y,
								 maxSt * _orient._x + maxCt * _orient._y );
				} else {
					// rotate _orient right
					_orient.set( maxCt * _orient._x + maxSt * _orient._y,
								 -maxSt * _orient._x + maxCt * _orient._y );
				}
			} else {
				_orient.set( newOrient );
			}
			// if (_orient.x() == 0 && _orient.y() == 0) {
			// 	std::cout << " NE W ORIENT ZERO! " << newOrient << _orient << " " << " I " << _id << std::endl;
			// }
		}

		////////////////////////////////////////////////////////////////

		void BaseAgent::setPreferredVelocity(PrefVelocity &velocity) {
			std::vector< BFSM::VelModifier * >::iterator vItr = _velModifiers.begin();
			for ( ; vItr != _velModifiers.end(); ++vItr ) {
				(*vItr)->adaptPrefVelocity(this, velocity);
			}

			//set my velocity to be the given one
			_velPref = velocity;
		}

		////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////

		float BaseAgent::getMeleeRange() const {
			return 1.0;
		}

		float BaseAgent::getTurnRadius() const {
    	const float prefVel = abs(_vel);
	    float rotationSpeedInRadians = _maxAngVel;
	    float radius = prefVel / rotationSpeedInRadians;
	    return radius;
		}

		Math::Vector2 BaseAgent::getTurnCircleDelta() const {
	    float radius = getTurnRadius();
	    float unitAngle = atan(_vel.y() / _vel.x());
	    float dX = cos(unitAngle + Math::PI / 2) * radius;
	    float dY = sin(unitAngle + Math::PI / 2) * radius;
	    // override dx and dy;
	    // dx = (circlesCenterX);
	    // dy = (circlesCenterY);
	    return Math::Vector2(dX, dY);
		}

		bool BaseAgent::targetInTurnCircle(const Math::Vector2& target) const {
			Vector2 dV;
			// add 2.0 margin
			float radius = getTurnRadius();
			dV = target - _pos - getTurnCircleDelta();
			if (absSq(dV) < (radius * radius)) return true;
			dV = target - _pos + getTurnCircleDelta();
			if (absSq(dV) < (radius * radius)) return true;
			return false;
		}

		void BaseAgent::computeNewVelocity() {
			throw AgentImplementationException();
		}

		////////////////////////////////////////////////////////////////

		void BaseAgent::insertEnemNeighbor(const BaseAgent* agent, float distSq) {
			if (_nearEnems.size() != _maxEnem || distSq <= getMaxEnemRange()) {
				if (_nearEnems.size() != _maxEnem) {
					_nearEnems.push_back(NearAgent(distSq, agent));
				}
				size_t i = _nearEnems.size() - 1;
				while (i != 0 && distSq < _nearEnems[i-1].distanceSquared) {
					_nearEnems[i] = _nearEnems[i-1];
					--i;
				}
				_nearEnems[i] = NearAgent(distSq, agent);
			}
		}

		void BaseAgent::insertAgentNeighbor(const BaseAgent* agent, float distSq) {
			if (this != agent) {
				if (_nearAgents.size() != _maxNeighbors || distSq <= getMaxNeighborRange()) {
					if (_nearAgents.size() != _maxNeighbors) {
						_nearAgents.push_back(NearAgent(distSq, agent));
					}
					size_t i = _nearAgents.size() - 1;
					while (i != 0 && distSq < _nearAgents[i-1].distanceSquared) {
						_nearAgents[i] = _nearAgents[i-1];
						--i;
					}
					_nearAgents[i] = NearAgent(distSq, agent);

				}
			}
		}


		void BaseAgent::insertFriendNeighbor(const BaseAgent* agent, float distSq) {
			if (this != agent) {
				if (_nearFriends.size() != _maxNeighbors || distSq <= getMaxFriendRange()) {
					if (_nearFriends.size() != _maxNeighbors) {
						_nearFriends.push_back(NearAgent(distSq, agent));
					}
					size_t i = _nearFriends.size() - 1;
					while (i != 0 && distSq < _nearFriends[i-1].distanceSquared) {
						_nearFriends[i] = _nearFriends[i-1];
						--i;
					}
					_nearFriends[i] = NearAgent(distSq, agent);

				}
			}
			// std::cout << "NEAR FRIENDS " << _nearFriends.size() << std::endl;
		}

		////////////////////////////////////////////////////////////////

		void BaseAgent::insertObstacleNeighbor( const Obstacle* obstacle, float distSq ) {
			// the assumption is that two obstacle neighbors MUST have the same classID
			if ( obstacle->_class & _obstacleSet ) {

				if ( distSq < _neighborDist * _neighborDist) {

					_nearObstacles.push_back( NearObstacle(distSq, obstacle) );

					size_t i = _nearObstacles.size() - 1;
					while ( i != 0 && distSq < _nearObstacles[i-1].distanceSquared ) {
						_nearObstacles[i] = _nearObstacles[i-1];
						--i;
					}
					_nearObstacles[i] = NearObstacle( distSq, obstacle );
				}
			}
		}

		///////////////////////////////////////////////////////////
		void BaseAgent::addVelModifier( BFSM::VelModifier * v ) { _velModifiers.push_back( v ); }

		///////////////////////////////////////////////////////////

		void BaseAgent::startQuery(){
			_nearAgents.clear();
			_nearEnems.clear();
			_nearFriends.clear();
			_nearObstacles.clear();
		};

		///////////////////////////////////////////////////////////

		void BaseAgent::filterAgent(const BaseAgent *agent, float distance) {
			if(isDead()) return;
			if (agent->isDead()) return;
			if (isEnemy(agent)) {
				insertEnemNeighbor(agent, distance);
			} else {
				insertFriendNeighbor(agent, distance);
			}
			insertAgentNeighbor(agent, distance);
		};

		///////////////////////////////////////////////////////////

		void BaseAgent::filterObstacle(const Obstacle * obstacle, float distance){
			insertObstacleNeighbor(obstacle, distance);
		};

		bool BaseAgent::isEnemy(const BaseAgent* agent) const {
			return _class != agent->_class;
		}

		///////////////////////////////////////////////////////////

		float BaseAgent::getMaxNeighborRange() {
			// ah so if, max agents, then it bumps down to current farthest.
			if (_nearAgents.size() == _maxNeighbors){

				return _nearAgents.back().distanceSquared;

			}

			// can probably cache this...
			return _neighborDist * _neighborDist;
			// return _neighborDistSquared;
		}

		float BaseAgent::getMaxAgentRange() {
			// we're just assuming the _nearEnems min distance is larger
			// since this function is being used by the spatial query.
			// if (_nearEnems.size() == _maxEnem){
			// 	return _nearEnems.back().distanceSquared;
			// }

			// // can probably cache this...
			// return _enemDist * _enemDist;
			return std::max(
				std::max(getMaxNeighborRange(), getMaxEnemRange()),
				getMaxFriendRange()
			);
		}

		float BaseAgent::getMaxEnemRange() {
			// ah so if, max agents, then it bumps down to current farthest.
			if (_nearEnems.size() == _maxEnem){
				return _nearEnems.back().distanceSquared;
			}

			// can probably cache this...
			return _enemDist * _enemDist;
			// return _neighborDistSquared;
		}

		float BaseAgent::getMaxFriendRange() {
			// ah so if, max agents, then it bumps down to current farthest.
			if (_nearFriends.size() == _maxEnem){
				return _nearFriends.back().distanceSquared;
			}

			// can probably cache this...
			return _enemDist * _enemDist;
			// return _neighborDistSquared;
		}

	}	// namespace Agents
}	// namespace Menge
