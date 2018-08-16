#include "MengeCore/BFSM/VelocityComponents/VelCompGoal.h"
#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/BFSM/Goals/Goal.h"
#include "MengeVis/Runtime/VisElementException.h"
#include "NearestEnemCompContext.h"
#include "NearestEnemyComponent.h"

#include <sstream>

namespace MengeVis {
  namespace Runtime {
    namespace VCContext {

      using Menge::Agents::BaseAgent;
      using Menge::Agents::PrefVelocity;
      using Menge::BFSM::Goal;
      // using Menge::BFSM::GoalVelComponent;
      using Menge::Math::Vector2;
      using Napoleon::NearestEnemComponent;

      /////////////////////////////////////////////////////////////////////
      //                   Implementation of TargetVelContext
      /////////////////////////////////////////////////////////////////////

      TargetVelContext::TargetVelContext() : VelCompContext(), _vc( 0x0 ) {
      }

      /////////////////////////////////////////////////////////////////////

      void TargetVelContext::setElement( const Menge::BFSM::VelComponent * vc ) {
        _vc = dynamic_cast<const NearestEnemComponent *>( vc );
        if ( _vc == 0x0 ) {
          throw VisElementException( "Trying to set a goal velocity component context "
                         "with an invalid value: either null or wrong "
                         "velocity component type." );
        }
      }

      /////////////////////////////////////////////////////////////////////

      std::string TargetVelContext::getElementName() const {
        return "enemy_near_comp";
      }

      /////////////////////////////////////////////////////////////////////

      std::string TargetVelContext::getUIText( const std::string & indent ) const {
        std::stringstream ss;
        ss << indent << "Goal velocity component";
        return ss.str();
      }

      /////////////////////////////////////////////////////////////////////

      void TargetVelContext::draw3DGL( const BaseAgent * agt, const Goal * goal ) {
        // draw goal point
        // TODO: Get the elevation of the goalPoint
        // Vector2 goalPoint = goal->getTargetPoint( agt->_pos, agt->_radius );
        // std::cout << " TargetVelContext?? " <<
        const Menge::Agents::NearAgent enem = _vc->getTargetEnem(agt);
        if (enem.agent == 0x0) return;
        Vector2 target = enem.agent->_pos;
        // Vector2 target(0.0, 0.0);
        // float distSq = 1000.f * 1000.f;
        // for (Menge::Agents::NearAgent enem : agt->_nearEnems) {
        //   if (enem.distanceSquared < distSq) {
        //     distSq = enem.distanceSquared;
        //     target = enem.agent->_pos;
        //   }
        // }


        drawGoal( target, agt );

        // draw preferred velocity
        PrefVelocity pVel;
        _vc->setPrefVelocity( agt, goal, pVel );
        drawPrefVel( pVel, agt->_pos );
      }
    } // namespace VCContext
  } // namespace Runtime
} // namespace MengeVis