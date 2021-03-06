/*

License

Menge
Copyright � and trademark � 2012-14 University of North Carolina at Chapel Hill.
All rights reserved.

Permission to use, copy, modify, and distribute this software and its
documentation for educational, research, and non-profit purposes, without fee,
and without a written agreement is hereby granted, provided that the above
copyright notice, this paragraph, and the following four paragraphs appear in
all copies.

This software program and documentation are copyrighted by the University of
North Carolina at Chapel Hill. The software program and documentation are
supplied "as is," without any accompanying services from the University of North
Carolina at Chapel Hill or the authors. The University of North Carolina at
Chapel Hill and the authors do not warrant that the operation of the program
will be uninterrupted or error-free. The end-user understands that the program
was developed for research purposes and is advised not to rely exclusively on
the program for any reason.

IN NO EVENT SHALL THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL OR THE AUTHORS
BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR
CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS
SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF NORTH CAROLINA AT
CHAPEL HILL OR THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL AND THE AUTHORS SPECIFICALLY
DISCLAIM ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE AND ANY STATUTORY
WARRANTY OF NON-INFRINGEMENT. THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
BASIS, AND THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL AND THE AUTHORS HAVE
NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
MODIFICATIONS.

Any questions or comments should be sent to the authors {menge,geom}@cs.unc.edu

*/

/*
 * Freestyle formation
 * http://graphics.cs.uh.edu/wp-content/papers/2013/2011_CGA-crowd-formation-generation-preprint.pdf
 */

#include "FreeFormation.h"

#include "MengeCore/Agents/SimulatorInterface.h"
#include "MengeCore/BFSM/VelocityModifiers/VelModifier.h"
#include "MengeCore/Core.h"

namespace Formations {

using Menge::Agents::BaseAgent;
using Menge::Agents::PrefVelocity;
using Menge::BFSM::FSM;
using Menge::BFSM::VelModFatalException;
using Menge::Logger;
using Menge::Math::Vector2;
using Menge::Math::sqr;
using Menge::Resource;
using Menge::ResourceException;
using Menge::ResourceManager;
using Menge::logger;

/////////////////////////////////////////////////////////////////////
//                   Implementation of FreeFormation
/////////////////////////////////////////////////////////////////////

const std::string FreeFormation::LABEL("formation");

// void FormationPoint::rotate(float rad) { // radians
//     const float cosRot = cos( rad );
//     const float sinRot = sin( rad );
//  auto tx = _pos.x();
//  auto ty = _pos.y();
//  _pos = Vector2((cosRot * tx) - (sinRot * ty), (sinRot * tx) + (cosRot *
//  ty)); _dir = _dist > 1e-5 ? -( _pos / _dist ) : Vector2(0.f, 0.f);
// };

/////////////////////////////////////////////////////////////////////

void FreeFormation::setRelativeOrient(const Menge::Math::Vector2& v) {
  _relativeOrient = v;
  _absoluteTarget = Vector2(0.f, 0.f);
  _inverseTarget = Vector2(0.f, 0.f);
}
void FreeFormation::setAbsoluteTarget(const Menge::Math::Vector2& v) {
  _relativeOrient = Vector2(0.f, 0.f);
  _absoluteTarget = v;
  _inverseTarget = Vector2(0.f, 0.f);
}
void FreeFormation::setInverseTarget(const Menge::Math::Vector2& v) {
  _relativeOrient = Vector2(0.f, 0.f);
  _absoluteTarget = Vector2(0.f, 0.f);
  _inverseTarget = v;
}

void FreeFormation::setTargetOrient(const Vector2& pos, PrefVelocity& pVel) {
  if (_relativeOrient) {
    pVel.setTarget(pos + _relativeOrient);
  } else if (_absoluteTarget) {
    pVel.setTarget(_absoluteTarget);
  } else if (_inverseTarget) {
    pVel.setTarget(-_inverseTarget);
  }
}

const Vector2& FreeFormation::getDisplacement() { return _displacement; }
void FreeFormation::setDisplacement(Vector2& v) { _displacement = v; }

FreeFormation::FreeFormation(const std::string& name) : Resource(name) {
  _speed = 0.0f;
  _direction = Vector2(1, 0);
  _pos = Vector2(0, 0);
  _displacement = Vector2(0, 0);
  // _pos is later calculated to be the average of all averagedAgent positions,
  // or something like that.
  _agentRadius = 0;

  // 0,0 values will be ignore
  _relativeOrient = Menge::Math::Vector2(0.f, 0.f);
  _absoluteTarget = Menge::Math::Vector2(0.f, 0.f);
  _inverseTarget = Menge::Math::Vector2(0.f, 0.f);
}

/////////////////////////////////////////////////////////////////////

FreeFormation::~FreeFormation() {
  // we're certainly not allowed to delete the agents!
  // but we should clear the vectors we control.
  // std::cout << "DELETE FORMAITON ? " << std::endl;
  logger << Logger::INFO_MSG << "delete formation " << getName();
  // std::vector<FormationPoint *>::iterator fpIter = _formationPoints.begin();
  // for ( ; fpIter != _formationPoints.end(); ++fpIter ) {
  //  delete *fpIter;
  // }

  // In all actuality, we don't have to do anything the data is constructed in
  // such a way to implicitly handle this

  // The problem with this is that this is TWO data structures that have been
  //  merged.  The *static* underlying formation definition, and the *dynamic*
  //  data mapping agents to that formation.
}

/////////////////////////////////////////////////////////////////////

void FreeFormation::addAgent(const BaseAgent* agt) {
  if (_agents.find(agt->_id) == _agents.end()) {
    _agents[agt->_id] = agt;
    _agentWeights[agt->_id] = 1.0f;  // default weight - this never changes
  }
}

/////////////////////////////////////////////////////////////////////

void FreeFormation::removeAgent(const BaseAgent* agt) {
  std::map<size_t, const BaseAgent*>::iterator itr = _agents.find(agt->_id);
  if (itr != _agents.end()) {
    _agents.erase(itr);
  }
}

/////////////////////////////////////////////////////////////////////

void FreeFormation::addFormationPoint(Vector2 point, bool borderPoint,
                                      float weight) {
  // now add the point
  // FormationPoint *pt = new FormationPoint();
  FormationPoint pt;

  pt._id = _formationPoints.size();
  pt._pos = point;
  pt._dist = abs(point);
  pt._dir = pt._dist > 1e-5 ? -(point / pt._dist) : Vector2(0.f, 0.f);
  pt._border = borderPoint;
  pt._weight = weight;

  _formationPoints.push_back(pt);
  if (pt._border) {
    // add this to the border list. DATA REDUNDANCY but we consider it a cache
    // for mapping
    _borderPoints.push_back(pt);
  }
}

void FreeFormation::clearFormationPoints() {
  // std::vector<FormationPoint *>::iterator fpIter = _formationPoints.begin();
  // for ( ; fpIter != _formationPoints.end(); ++fpIter ) {
  //  delete *fpIter;
  // }
  _formationPoints.clear();
  _borderPoints.clear();
}

/////////////////////////////////////////////////////////////////////

void FreeFormation::addAgentPoint(const BaseAgent* agt) {
  // make a sentinel point
  // FormationPoint* agtPoint = new FormationPoint();
  FormationPoint agtPoint;

  agtPoint._id = agt->_id;
  agtPoint._pos = agt->_pos;
  agtPoint._dir = _pos - agt->_pos;
  agtPoint._dist = abs(agtPoint._dir);
  agtPoint._border = false;
  agtPoint._weight = 0.0f;

  // normalize direction
  if (agtPoint._dist > 1e-5f) {
    agtPoint._dir /= agtPoint._dist;
  }

  // we'll convert to formation coordinates later
  if (agtPoint._dist > _agentRadius) {
    _agentRadius = agtPoint._dist;
  }

  // now store it
  _agentPoints[agt->_id] = agtPoint;
}

/////////////////////////////////////////////////////////////////////

void FreeFormation::normalizeFormation() {
  // std::vector<FormationPoint *>::iterator fpIter = _formationPoints.begin();

  // Compute weighted center of the reference formation
  Vector2 weightedCenter(0.f, 0.f);
  float totalWeight = 0.f;
  // for (; fpIter != _formationPoints.end(); ++fpIter){
  for (const FormationPoint& pt : _formationPoints) {
    weightedCenter += pt._pos * pt._weight;
    totalWeight += pt._weight;
    // }
  }
  // average the weighted center
  weightedCenter /= totalWeight;

  // Paul: I kinda get what they were aming for below
  // but it was leading to weird behavior if the formation didn't
  // match a center of 0,0. My code further below overrdies the _dist and _dir
  // to point to the weighted center. Seems to work ok. Maybe I'm missing
  // something
  // but *shrug* I'll till there's an issue in the behavior.\

  // ah I see it's for weight prioritization...
  /// we'll have to get a working expirment.

  // Translate to canonical formation space and determine formation size
  //  offset by weighted center and compute encompassing circle.
  // fpIter = _formationPoints.begin();
  // float formationRadius = 0.f;
  // for ( ; fpIter != _formationPoints.end(); ++fpIter ) {
  //  (*fpIter)->_pos -= weightedCenter;
  //  (*fpIter)->_dist = abs( (*fpIter)->_pos );
  //  if ( (*fpIter)->_dist > formationRadius ){
  //    formationRadius = (*fpIter)->_dist;
  //  }
  // }

  // float invDist = 1.f / formationRadius;
  // // Scale all distances
  // for (; fpIter != _formationPoints.end(); ++fpIter){
  //  (*fpIter)->_dist *= invDist;
  //  (*fpIter)->_pos *= invDist;
  // }

  for (int i = 0; i < _formationPoints.size(); i++) {
    FormationPoint& pt = _formationPoints[i];
    // std::cout << "Weighted Center " << weightedCenter.x() << " " <<
    // weightedCenter.y();
    pt._dist = abs(weightedCenter - pt._pos);
    // std::cout << "_DIR? " << pt->_dir.x() << " " << pt->_dir.y() <<
    // std::endl;
    pt._dir = pt._dist > 1e-5 ? (weightedCenter - pt._pos) / pt._dist
                              : Vector2(0.f, 0.f);
    // std::cout << "_DIR AFT? " << pt->_dir.x() << " " << pt->_dir.y() <<
    // std::endl;
  }
}

/////////////////////////////////////////////////////////////////////

void FreeFormation::mapAgentsToFormation() {
  // we need intermediate vars
  // std::vector<FormationPoint *>::const_iterator formationItr;
  const BaseAgent* agt;
  size_t exceptionCount = 0;  // delete me
  size_t agtCount = 0;
  std::map<size_t, const BaseAgent*>::iterator itr;     // agents
  std::map<size_t, FormationPoint>::iterator mapIter;  // agent points
  float totalWeight = 0.0f;
  // reset vars
  _pos.set(0.f, 0.f);
  _direction.set(0.f, 0.f);
  _speed = 0.0f;
  _agentRadius = 0.0f;

  // clear the relationships
  // TODO: Anything that maps agents -> value should NOT clear at each time
  //    step.  The structure of these objects should only change when the
  //    agents in the formation changes.  Not just to update values.
  _agent_formationPoint.clear();
  _formationPoint_agent.clear();
  // mapIter = _agentPoints.begin();
  // for (; mapIter != _agentPoints.end(); ++mapIter) {
  //   delete mapIter->second;
  // }
  _agentPoints.clear();

  // Compute formation world position, direction, and speed
  float totalSpeed = 0.f;
  for (itr = _agents.begin(); itr != _agents.end(); ++itr) {
    agt = itr->second;

    _pos += agt->_pos * _agentWeights[agt->_id];
    totalWeight += _agentWeights[agt->_id];
    // see if we have a cache
    // doesn't look like we actually use _agentPrefDirs itself...
    // just he agentPrefVels, we'll keep just in case though.
    if (_agentPrefDirs.find(agt->_id) == _agentPrefDirs.end()) {
      _direction += agt->_velPref.getPreferredVel();
    } else {
      _direction += _agentPrefVels[agt->_id];
    }
    totalSpeed += agt->_velPref.getSpeed();
    ++agtCount;
  }

  // now that we can localize and normalize the formation, let's do so.
  if (totalWeight <= 0.0) {
    totalWeight = 1.0;
    // throw VelModFatalException("total weight is zero");
  }
  _pos /= totalWeight;
  float mag = abs(_direction);
  _speed = totalSpeed > 0.f ? mag / totalSpeed : mag;
  if (mag > 1e-5) {
    _direction /= mag;
  }

  // Define "sentinel" points for the agents -- currently unnormalized.
  for (itr = _agents.begin(); itr != _agents.end(); ++itr) {
    agt = itr->second;
    // make a sentinel point
    addAgentPoint(agt);
  }

  // First select agents for the border points
  for (FormationPoint& pt : _borderPoints) {
    mapPointToAgent(pt);
  }
  // formationItr = _borderPoints.begin();
  // for ( ; formationItr != _borderPoints.end(); ++formationItr ){
  //  mapPointToAgent( *formationItr );
  // }

  // Finally, map formation points to the remaining agents
  itr = _agents.begin();
  for (; itr != _agents.end(); ++itr) {
    agt = itr->second;
    if (_agent_formationPoint.find(agt->_id) == _agent_formationPoint.end()) {
      mapAgentToPoint(agt);
    }
  }
}

/////////////////////////////////////////////////////////////////////

void FreeFormation::mapPointToAgent(FormationPoint& pt) {
  // This does a brute-force, linear search through the agents to find
  //  the "nearest" candidate.

  // iterate over the agents and find the best agent for this point
  std::map<size_t, const BaseAgent*>::const_iterator itr;
  // FormationPoint agtPoint;
  float distance, minDistance;
  distance = minDistance = 1000000.0;
  size_t minAgtID = -1;

  for (itr = _agents.begin(); itr != _agents.end(); ++itr) {
    const BaseAgent* agt = itr->second;
    // make sure the agent isn't already mapped to a border
    if (_agent_formationPoint.find(agt->_id) == _agent_formationPoint.end()) {
      // check distance
      // agtPoint = ;

      // check distance
      distance = formationDistance(pt, _agentPoints[agt->_id]);
      if (distance < minDistance) {
        minDistance = distance;
        minAgtID = itr->second->_id;
      }
    }
  }

  // ignore if minAgtID == -1:  This means there were insufficient agents
  //    for the formation.  This is not a problem.
  if (minAgtID > -1) {
    _formationPoint_agent[pt._id] = minAgtID;
    _agent_formationPoint[minAgtID] = pt._id;
    _agentWeights[minAgtID] = _formationPoints[pt._id]._weight;
  }
}

/////////////////////////////////////////////////////////////////////

void FreeFormation::mapAgentToPoint(const BaseAgent* agt) {
  // This uses a brute-force approach of teseting every formation
  //    point for the agent.  No spatial queries.

  // The corresponding sentinel point for this agent
  FormationPoint& agtPoint = _agentPoints[agt->_id];

  // std::vector<FormationPoint*>::iterator fpIter = _formationPoints.begin();
  size_t minPt = -1;  // min pt for mapping
  float distance, minDistance;
  distance = minDistance = 1000000.0f;

  // for (; fpIter != _formationPoints.end(); ++fpIter) {
  for (FormationPoint& formPt : _formationPoints) {
    // FormationPoint* formPt = *fpIter;
    if (_formationPoint_agent.find(formPt._id) == _formationPoint_agent.end()) {
      // so the keypoint is not mapped. get the distance
      distance = formationDistance(agtPoint, formPt);
      if (distance < minDistance) {
        minDistance = distance;
        minPt = formPt._id;
      }
    }
  }

  if (minPt == -1) {
    // TODO: Although this claims to be "fatal", it doesn't cause the
    //      program to crash.  Make the exception appropriate.
    logger << Logger::ERR_MSG << "Not enough points in formation. Size: "
           << _formationPoints.size() << " agents: " << _agents.size()
           << " formationPoitn_agentmap: " << _formationPoint_agent.size();
    logger.close();
    throw VelModFatalException("Not enough points in formation.");
  } else {
    _formationPoint_agent[minPt] = agtPoint._id;
    _agent_formationPoint[agtPoint._id] = minPt;
    _agentWeights[agt->_id] = _formationPoints[minPt]._weight;
  }
}

/////////////////////////////////////////////////////////////////////

bool FreeFormation::getPointIndexForAgent(const BaseAgent* agt, int& i) {
  if (_agent_formationPoint.find(agt->_id) != _agent_formationPoint.end()) {
    i = _agent_formationPoint[agt->_id];
    return true;
  }

  return false;
}

bool FreeFormation::getGoalForAgent(const BaseAgent* agt, PrefVelocity& pVel,
                                    Vector2& target) {
  // The goal point is the agent's corresponding sential point (with the point
  // moving the formations direction and speed.)

  // cache input pref dir
  _agentPrefDirs[agt->_id] = pVel.getPreferred();
  _agentPrefVels[agt->_id] = pVel.getPreferredVel();

  // assuming this is only called on agents in the formation
  assert(_agents.find(agt->_id) != _agents.end() &&
         "Trying to get a formation goal for an agent that is not in the "
         "formation");

  // the first frame an agent enters a formation does not guaruntee it has
  // been mapped.
  if (_agent_formationPoint.find(agt->_id) != _agent_formationPoint.end()) {
    target = _formationPoints[_agent_formationPoint[agt->_id]]._pos + _pos;
    target = target + _direction * _speed;
    return true;
  }

  return false;
}

int FreeFormation::getNumAgents() const { return _agents.size(); }

int FreeFormation::getNumArrived() const {
  int num = 0;
  for (const auto v : _agents) {
    size_t id = v.first;
    const BaseAgent* agt = v.second;
    if (_agent_formationPoint.find(agt->_id) != _agent_formationPoint.end()) {
      const Vector2& target =
          _formationPoints[_agent_formationPoint.find(agt->_id)->second]._pos;
      // std::cout << "DIST " << absSq(agt->_pos - target - _displacement) <<
      // " "<< agt->_pos << " " << target + _displacement << std::endl;
      if (absSq(agt->_pos - target - _displacement) < 1e-3) {
        ++num;
      }
    }
  }
  return num;
}

bool FreeFormation::getStaticGoalForAgent(const BaseAgent* agt,
                                          PrefVelocity& pVel, Vector2& target) {
  // The goal point is the agent's corresponding sential point (with the point
  // moving the formations direction and speed.)

  // cache input pref dir
  _agentPrefDirs[agt->_id] = pVel.getPreferred();
  _agentPrefVels[agt->_id] = pVel.getPreferredVel();

  // assuming this is only called on agents in the formation
  assert(_agents.find(agt->_id) != _agents.end() &&
         "Trying to get a formation goal for an agent that is not in the "
         "formation");

  // the first frame an agent enters a formation does not guaruntee it has
  // been mapped.
  if (_agent_formationPoint.find(agt->_id) != _agent_formationPoint.end()) {
    target = _formationPoints[_agent_formationPoint[agt->_id]]._pos;
    // target = target + _direction * _speed;
    return true;
  }

  return false;
}

/////////////////////////////////////////////////////////////////////

float FreeFormation::formationDistance(const FormationPoint& pt1,
                                       const FormationPoint& pt2) {
  Vector2 relDir(pt1._dir - pt2._dir);
  return sqrtf(absSq(relDir) + sqr(pt1._dist - pt2._dist));
}

/////////////////////////////////////////////////////////////////////

void FreeFormation::setPoints(const std::vector<Vector2>& points,
                              const std::vector<size_t>& weights,
                              unsigned int borderCount) {
  // TODO: Change this to support comments.
  if (points.size() != weights.size()) {
    throw VelModFatalException("Points and weights size must match. " +
                               std::to_string(points.size()) + " " +
                               std::to_string(weights.size()));
  }
  if (borderCount > points.size()) {
    throw VelModFatalException(
        "Pounts must be more than or equal to border count");
  }
  clearFormationPoints();

  unsigned int remaining = points.size();
  float weight;
  Vector2 v;
  for (unsigned int i = 0; i < borderCount; ++i) {
    v = points[i];
    weight = weights[i];
    addFormationPoint(v, true, weight);
    remaining--;
  }

  // while we have points left, they are internal points
  unsigned int i = borderCount;
  while (remaining) {
    weight = weights[i];
    v = points[i];
    addFormationPoint(v, false, weight);
    i++;
    remaining--;
  }
  // std::cout << "num formation points " << _formationPoints.size() <<
  // std::endl;
  // normalize the formation
  normalizeFormation();
}

Resource* FreeFormation::load(const std::string& fileName) {
  // TODO: Change this to support comments.
  std::ifstream f;
  f.open(fileName.c_str(), std::ios::in);

  if (!f.is_open()) {
    logger << Logger::ERR_MSG << "Error opening formation file: " << fileName
           << ".";
    return 0x0;
  }

  // load vertices
  unsigned int borderCount;
  if (!(f >> borderCount)) {
    logger << Logger::ERR_MSG
           << "Error in parsing formation: file didn't start with ";
    logger << "border vertex count.";
    return 0x0;
  }

  FreeFormation* form = new FreeFormation(fileName);

  float x, y, weight;
  for (unsigned int v = 0; v < borderCount; ++v) {
    if (!(f >> x >> y >> weight)) {
      logger << Logger::ERR_MSG;
      logger << "Error in parsing formation: format error for vertex "
             << (v + 1);
      logger << ".";
      form->destroy();
      return 0x0;
    }
    form->addFormationPoint(Vector2(x, y), true, weight);
  }

  // while we have points left, they are internal points
  if ((f >> x >> y >> weight)) {
    while (!f.eof()) {
      form->addFormationPoint(Vector2(x, y), false, weight);
      if (f >> x) {
        if (!(f >> y >> weight)) {
          logger << Logger::ERR_MSG;
          logger << "Error in parsing formation: format error for point.";
          form->destroy();
          return 0x0;
        }
      }
    }
  }

  // normalize the formation
  form->normalizeFormation();

  return form;
}

/////////////////////////////////////////////////////////////////////

FormationPtr loadFormation(const std::string& fileName) {
  Resource* rsrc = ResourceManager::getResource(fileName, &FreeFormation::load,
                                                FreeFormation::LABEL);
  if (rsrc == 0x0) {
    logger << Logger::ERR_MSG << "No resource available.";
    throw ResourceException();
  }
  FreeFormation* form = dynamic_cast<FreeFormation*>(rsrc);
  if (form == 0x0) {
    logger << Logger::ERR_MSG << "Resource with name " << fileName
           << " is not a formation.";
    throw ResourceException();
  }

  return FormationPtr(form);
}
}  // namespace Formations
