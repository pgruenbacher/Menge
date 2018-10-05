#include "SteadyFormation.h"

namespace Formations {

using Menge::logger;
using Menge::Logger;
using Menge::Resource;
using Menge::ResourceException;
using Menge::ResourceManager;
using Menge::Agents::BaseAgent;
using Menge::Agents::PrefVelocity;
using Menge::BFSM::FSM;
using Menge::BFSM::VelModFatalException;
using Menge::Math::Vector2;
using Menge::Math::sqr;

const std::string SteadyFormation::LABEL("steady-formation");

void SteadyFormation::mapAgentsToFormation(const FSM * fsm) {
  // only updates if either a) it hasn't been set the first time or b)
  // explictly caleld to "reset" the mapping of agents to points. e.g. when
  // formation flips more than 180 deg or game timer or num agents changes
  // drastically.

  // or just update every time agent enters/leaves maybe...
  if (_shouldUpdate || (getFormationPointAgent().size() == 0) || (getFormationPointAgent().size() != getCurrentAgents().size())) {
    _shouldUpdate = false;
    FreeFormation::mapAgentsToFormation(fsm);
  } else {
    return;
  }
}

SteadyFormation::SteadyFormation(const std::string& name) : FreeFormation(name), _shouldUpdate(true) {
};

/////////////////////////////////////////////////////////////////////

SteadyFormation::~SteadyFormation(){
  // ~FreeFormation base destructor should be called.
};

Resource * SteadyFormation::load( const std::string & fileName ) {
  // TODO: Change this to support comments.
  std::ifstream f;
  f.open( fileName.c_str(), std::ios::in );

  if ( !f.is_open() ) {
    logger << Logger::ERR_MSG << "Error opening formation file: " << fileName << ".";
    return 0x0;
  }

  // load vertices
  unsigned int borderCount;
  if ( ! ( f >> borderCount ) ) {
    logger << Logger::ERR_MSG << "Error in parsing formation: file didn't start with ";
    logger << "border vertex count.";
    return 0x0;
  }

  SteadyFormation * form = new SteadyFormation( fileName );

  float x, y, weight;
  for ( unsigned int v = 0; v < borderCount; ++v ) {
    if ( ! ( f >> x >> y >> weight ) ) {
      logger << Logger::ERR_MSG;
      logger << "Error in parsing formation: format error for vertex " << ( v + 1 );
      logger << ".";
      form->destroy();
      return 0x0;
    }
    form->addFormationPoint( Vector2( x, y ), true, weight );
  }

  //while we have points left, they are internal points
  if ( ( f >> x >> y >> weight ) ) {
    while ( !f.eof() ) {
      form->addFormationPoint(Vector2(x, y), false, weight);
      if ( f >> x ){
        if (!(f >> y >> weight)){
          logger << Logger::ERR_MSG;
          logger << "Error in parsing formation: format error for point.";
          form->destroy();
          return 0x0;
        }
      }
    }
  }

  //normalize the formation
  form->normalizeFormation();

  return form;
}
}
