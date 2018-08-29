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

// #include "MengeCore/BFSM/VelocityComponents/VelCompGoal.h"
#include "FormationsComponent.h"
#include "MengeCore/resources/Resource.h"

#include "MengeCore/Core.h"
#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/BFSM/State.h"
#include "MengeCore/Agents/PrefVelocity.h"
#include "MengeCore/BFSM/Goals/Goal.h"
#include "MengeCore/BFSM/GoalSelectors/GoalSelectorIdentity.h"
#include "MengeCore/Runtime/os.h"
#include "FormationsTask.h"
#include "SteadyFormation.h"


namespace Formations {

  /////////////////////////////////////////////////////////////////////
  //                   Implementation of GoalVelComponent
  /////////////////////////////////////////////////////////////////////

  const std::string FormationComponent::NAME = "formationComp";

  /////////////////////////////////////////////////////////////////////

  void FormationComponent::onEnter(BaseAgent* agent) {
    registerAgent(agent);
  }

  void FormationComponent::onExit(BaseAgent* agent) {
    unregisterAgent(agent);
  }

  void FormationComponent::registerAgent(const BaseAgent * agent) {
    _lock.lockWrite();
      _formation->addAgent(agent);
    _lock.releaseWrite();
    // std::cout << "register agent " << std::endl;
  };


  void FormationComponent::unregisterAgent(const BaseAgent * agent){
    _lock.lockWrite();
    _formation->removeAgent(agent);
    _lock.releaseWrite();
    // std::cout << "unregister agent " << std::endl;
  };

  /////////////////////////////////////////////////////////////////////

  FormationComponent::FormationComponent() : VelComponent() {
  }

  void FormationComponent::setFormation(FormationPtr form){
    _formation = form;
  };

  void FormationComponent::setDisplacement(Vector2 v){
    _displacment = v;
  };

  void FormationComponent::loadSetFormation(const std::string& fname) {
    try {
      setFormation(loadFormation(fname));
    } catch ( Menge::ResourceException ) {
      logger << Logger::ERR_MSG << "Couldn't instantiate formatoin " << fname;
    }
    return;
  };
  /////////////////////////////////////////////////////////////////////

  void FormationComponent::setPrefVelocity( const BaseAgent * agent, const Goal * goal,
                      PrefVelocity & pVel ) const {
    //adapt the agent's velocity according to the formation
    Vector2 target = Vector2( 0.f, 0.f );
    _lock.lockRead();
    bool modify = _formation->getStaticGoalForAgent( agent, pVel, target );
    _lock.releaseRead();
    if ( modify ) {

      // target + displacement - agent position
      Vector2 disp = target + _displacment - agent->_pos;
      const float distSq = absSq( disp );
      Vector2 dir;
      if ( distSq > 1e-8 ) {
        // Distant enough that I can normalize the direction.
        dir.set( disp / sqrtf( distSq ) );
      } else {
        dir.set( 0.f, 0.f );
      }
      pVel.setSingle(dir);
      pVel.setTarget(target);

      float speed = agent->_prefSpeed;

      if ( distSq <= 0.0001f ) {
        // I've basically arrived -- speed should be zero.
        speed = 0.f;
      } else {
        const float speedSq = speed * speed;
        const float TS_SQD = SIM_TIME_STEP * SIM_TIME_STEP;
        if ( distSq / speedSq < TS_SQD ) {
          // The distance is less than I would travel in a single time step.
          speed = sqrtf( distSq ) / SIM_TIME_STEP;
          // std::cout << "DO REDUCE" << std::endl;
        } else {
          // std:: cout << "?? " << distSq / speedSq << " " << TS_SQD << " " << SIM_TIME_STEP << std::endl;
        }
      }
      pVel.setSpeed( speed );

    } else {

      // std::cout << "no modify " << target._x << " " << target._y << std::endl;
    }
  }

  Task * FormationComponent::getTask(){
    return new FormationsTask( _formation );
  };

  Menge::BFSM::State* makeFormationState(int uniqueIndex, size_t classId) {
    // godot specific.
    Menge::BFSM::State* st = new Menge::BFSM::State("Formation" + std::to_string(uniqueIndex), "FormState", classId);
    st->setGoalSelector(new Menge::BFSM::IdentityGoalSelector());
    st->setFinal(false);
    std::string resourceName = "freeFormation" + std::to_string(uniqueIndex);
    Resource * rsrc = ResourceManager::getResource( resourceName, &SteadyFormation::make, SteadyFormation::LABEL );
    if ( rsrc == 0x0 ) {
      logger << Logger::ERR_MSG << "No resource available.";
      throw ResourceException();
    }
    SteadyFormation * form = dynamic_cast< SteadyFormation * >( rsrc );
    FormationComponent* fComp = new FormationComponent();
    fComp->setFormation(FormationPtr( form ));
    st->setVelComponent(fComp);
    return st;
  }

  /////////////////////////////////////////////////////////////////////
  FormationComponentFactory::FormationComponentFactory() : VelCompFactory() {
    // _fileNameID = _attrSet.addStringAttribute( "file_name", true /*required*/ );
    // _headingID = _attrSet.addFloatAttribute( "heading_threshold", false /*required*/,
    //                      180.f );
    _fileNameID = _attrSet.addStringAttribute( "file_name", true /*required*/ );
    _x = _attrSet.addFloatAttribute("x", false);
    _y = _attrSet.addFloatAttribute("y", false);
  }

  /////////////////////////////////////////////////////////////////////

  bool FormationComponentFactory::setFromXML( VelComponent * component, TiXmlElement * node,
                     const std::string & behaveFldr ) const {
      FormationComponent * formationComp = dynamic_cast<FormationComponent *>(component);
      assert( formationComp != 0x0 &&
      "Trying to set property component properties on an incompatible object" );
    if ( ! VelCompFactory::setFromXML( component, node, behaveFldr ) ) return false;

    if (_attrSet.getString(_fileNameID).empty()) {
      logger << Logger::ERR_MSG << "Require filename for velocity component";
      logger.close();
      throw Menge::BFSM::VelCompFatalException( "Require filename for velocity component." );
    }
    // get the absolute path to the file name

    std::string fName;
    std::string path = Menge::os::path::join(
        2, behaveFldr.c_str(), _attrSet.getString(_fileNameID).c_str());
    Menge::os::path::absPath(path, fName);
    // nav mesh
    FormationPtr formPtr;
    try {
      formationComp->setFormation(loadFormation( fName ));
      formationComp->setDisplacement(Vector2(_attrSet.getFloat(_x), _attrSet.getFloat(_y)));
    } catch ( Menge::ResourceException ) {
      logger << Logger::ERR_MSG << "Couldn't instantiate the formation referenced on line ";
      logger << node->Row() << ".";
      return false;
    }
    return true;
  }
} // namespace Formations
