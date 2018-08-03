#include "EnemyNearCondition.h"

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

#include "MengeCore/BFSM/Transitions/CondTimer.h"

#include "MengeCore/Core.h"
#include "MengeCore/Agents/BaseAgent.h"

namespace Napoleon {


    ///////////////////////////////////////////////////////////////////////////
    //                   Implementation of EnemyNearCondition
    ///////////////////////////////////////////////////////////////////////////

    // EnemyNearCondition::EnemyNearCondition() {
    // }

    ///////////////////////////////////////////////////////////////////////////

    // EnemyNearCondition::EnemyNearCondition( const EnemyNearCondition & cond ):Condition(cond) {
    // }

    ///////////////////////////////////////////////////////////////////////////

    // EnemyNearCondition::~EnemyNearCondition() {
    // }

    ///////////////////////////////////////////////////////////////////////////

    // void EnemyNearCondition::onEnter( BaseAgent * agent ) {
    // }

    // ///////////////////////////////////////////////////////////////////////////

    // void EnemyNearCondition::onLeave( BaseAgent * agent ) {
    // }

    ///////////////////////////////////////////////////////////////////////////

    bool EnemyNearCondition::conditionMet( BaseAgent * agent, const Goal * goal ) {
      return true;
    }

    ///////////////////////////////////////////////////////////////////////////

    EnemyNearCondition * EnemyNearCondition::copy() {
      return new EnemyNearCondition( *this );
    }

    ///////////////////////////////////////////////////////////////////////////
    //                   Implementation of EnemyNearCondFactory
    /////////////////////////////////////////////////////////////////////

    EnemyNearCondFactory::EnemyNearCondFactory() : ConditionFactory() {
      // _perAgentID = _attrSet.addBoolAttribute( "per_agent", true /*required*/ );
      // _durGenID = _attrSet.addFloatDistAttribute( "", true, 0.f, 1.f );
    }

    ///////////////////////////////////////////////////////////////////////////

    bool EnemyNearCondFactory::setFromXML( Condition * condition, TiXmlElement * node,
                       const std::string & behaveFldr ) const {
      EnemyNearCondition * tCond = dynamic_cast< EnemyNearCondition * >( condition );
      assert( tCond != 0x0 &&
          "Trying to set the properties of a timer condition on an incompatible "
          "object" );

      if ( !ConditionFactory::setFromXML( condition, node, behaveFldr ) ) return false;

      // bool useGlobal = !_attrSet.getBool( _perAgentID );
      // FloatGenerator * gen = _attrSet.getFloatGenerator( _durGenID );
      // if ( useGlobal ) {
      //   // This allows for a randomly generated const value.  As opposed to simply
      //   //  specifying a global const.
      //   tCond->_durGen = new ConstFloatGenerator( gen->getValue() );
      //   // TODO: determine if this is safe across dlls.  It may need to be a destroy.
      //   delete gen;
      // } else {
      //   tCond->_durGen = gen;
      // }
      return true;
    }

} // namespace Napoleon
