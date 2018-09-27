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

#include "StaggerModifier.h"
#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/Core.h"
#include "./PikeTask.h"
#include "StaggerTask.h"

namespace Napoleon {

  using Menge::logger;
  using Menge::Logger;
  using Menge::Agents::BaseAgent;
  using Menge::Agents::PrefVelocity;
  using Menge::BFSM::Task;
  using Menge::BFSM::VelModifier;
  using Menge::BFSM::VelModFactory;
  using Menge::Math::Vector2;

  /////////////////////////////////////////////////////////////////////
  //                   Implementation of StaggerModifier
  /////////////////////////////////////////////////////////////////////

  /* Pike modifier is used for enemy/ally movements moving around pike positions */

  StaggerModifier::StaggerModifier() {
  }

  /////////////////////////////////////////////////////////////////////

  StaggerModifier::~StaggerModifier(){
  };

  /////////////////////////////////////////////////////////////////////

  VelModifier* StaggerModifier::copy() const{
    return new StaggerModifier( );
  };

  /////////////////////////////////////////////////////////////////////

  void StaggerModifier::adaptPrefVelocity( const BaseAgent * agent, PrefVelocity & pVel ) {
  }

  Menge::BFSM::Task * StaggerModifier::getTask() {
    return StaggerTask::getSingleton();
  };


  /////////////////////////////////////////////////////////////////////

  void StaggerModifier::registerAgent(const Menge::Agents::BaseAgent* agent) {

  };

  /////////////////////////////////////////////////////////////////////

  void StaggerModifier::unregisterAgent(const Menge::Agents::BaseAgent* agent){
    StaggerTask* task = StaggerTask::getSingleton();
    task->setStaggerComplete(agent->_id);
  };

  /////////////////////////////////////////////////////////////////////
  //                   Implementation of FormationModFactory
  /////////////////////////////////////////////////////////////////////

  StaggerModifierFactory::StaggerModifierFactory() : VelModFactory() {
    //no properties yet
  }
  /////////////////////////////////////////////////////////////////////

  bool StaggerModifierFactory::setFromXML( VelModifier * modifier, TiXmlElement * node,
                         const std::string & behaveFldr ) const {
    StaggerModifier * stagMod = dynamic_cast<StaggerModifier *>(modifier);
        assert( stagMod != 0x0 &&
        "Trying to set property modifier properties on an incompatible object" );

    if ( ! Menge::BFSM::VelModFactory::setFromXML( modifier, node, behaveFldr ) ) return false;

    return true;
  }

  /////////////////////////////////////////////////////////////////////
} // namespace Napoleons
