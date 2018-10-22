#include "WayPointComponent.h"
#include "WayPointTask.h"
// #include "curve.h"
#include "MengeCore/resources/Resource.h"

#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/Agents/PrefVelocity.h"
#include "MengeCore/BFSM/GoalSelectors/GoalSelectorIdentity.h"
#include "MengeCore/BFSM/Goals/Goal.h"
#include "MengeCore/BFSM/State.h"
#include "MengeCore/Core.h"
#include "MengeCore/Runtime/os.h"
// #include "Plugins/Formations/FormationsTask.h"
// #include "Plugins/Formations/SteadyFormation.h"

namespace Napoleon {

/////////////////////////////////////////////////////////////////////
//                   Implementation of GoalVelComponent
/////////////////////////////////////////////////////////////////////

const char* WayPointComponent::NAME = "wayPointComp";

/////////////////////////////////////////////////////////////////////

void WayPointComponent::onEnter(BaseAgent* agent) {
  _current_num_agents++;
  if (_current_num_agents > _current_max_agents) {
    _current_max_agents = _current_num_agents;
  }
  _task->addAgent(agent);
}

void WayPointComponent::onExit(BaseAgent* agent) {
  _current_num_agents--;

  _task->removeAgent(agent);
}

/////////////////////////////////////////////////////////////////////

WayPointComponent::WayPointComponent()
    : VelComponent(),
      _loop(false),
      _auto(false),
      _task(0x0),
      _current_max_agents(0),
      _current_num_agents(0) {}

void WayPointComponent::setCurve(CurvePtr c) {
  _curve = c;
  // std::cout << "SET FORMATION " << std::endl;
}

/////////////////////////////////////////////////////////////////////

void WayPointComponent::setPrefVelocity(const BaseAgent* agent,
                                        const Goal* goal,
                                        PrefVelocity& pVel) const {
  if (!_task) return;
  if (!_curve.hasData()) return;
  // Vector2 target = _curve->interpolate_baked(_task->getCurrentPosition());
  Vector2 target = _task->getGoalForAgent(agent);

  Vector2 disp = target - agent->_pos;
  Vector2 dir(0.f, 0.f);

  const float distSq = absSq(disp);
  if (distSq > 1e-8) {
    // Distant enough that I can normalize the direction.
    dir.set(disp / sqrtf(distSq));
  }

  pVel.setSingle(dir);
  pVel.setTarget(target);

  float speed = agent->_prefSpeed;

  if (distSq <= 0.0001f) {
    // I've basically arrived -- speed should be zero.
    speed = 0.f;
  } else {
    const float speedSq = speed * speed;
    const float TS_SQD = SIM_TIME_STEP * SIM_TIME_STEP;
    if (distSq / speedSq < TS_SQD) {
      // The distance is less than I would travel in a single time step.
      speed = sqrtf(distSq) / SIM_TIME_STEP;
      // std::cout << "DO REDUCE" << std::endl;
    } else {
      // std:: cout << "?? " << distSq / speedSq << " " << TS_SQD << " " <<
      // SIM_TIME_STEP << std::endl;
    }
  }
  pVel.setSpeed(speed);
}

Task* WayPointComponent::getTask() {
  WayPointsTask* t = new WayPointsTask(_curve);
  _task = t;
  if (_auto) {
    _task->setCanPerform(true);
  }
  return t;
}

/////////////////////////////////////////////////////////////////////
WayPointComponentFactory::WayPointComponentFactory() : VelCompFactory() {
  // _fileNameID = _attrSet.addStringAttribute( "file_name", true /*required*/
  // ); _headingID = _attrSet.addFloatAttribute( "heading_threshold", false
  // /*required*/,
  //                      180.f );
  _loopID = _attrSet.addBoolAttribute("loop", false, false);
  _autoID = _attrSet.addBoolAttribute("auto", false, false);
  _fileNameID = _attrSet.addStringAttribute("file_name", true /*required*/);
}

/////////////////////////////////////////////////////////////////////

bool WayPointComponentFactory::setFromXML(VelComponent* component,
                                          TiXmlElement* node,
                                          const std::string& behaveFldr) const {
  WayPointComponent* waypointComp = dynamic_cast<WayPointComponent*>(component);
  assert(
      waypointComp != 0x0 &&
      "Trying to set property component properties on an incompatible object");
  if (!VelCompFactory::setFromXML(component, node, behaveFldr)) return false;

  if (_attrSet.getString(_fileNameID).empty()) {
    logger << Logger::ERR_MSG << "Require filename for velocity component";
    logger.close();
    throw Menge::BFSM::VelCompFatalException(
        "Require filename for velocity component.");
  }
  // get the absolute path to the file name

  std::string fName;
  std::string path = Menge::os::path::join(
      2, behaveFldr.c_str(), _attrSet.getString(_fileNameID).c_str());
  Menge::os::path::absPath(path, fName);
  // nav mesh
  CurvePtr curvePtr;
  try {
    curvePtr = loadCurve(fName);
    waypointComp->_auto = _attrSet.getBool(_autoID);
    waypointComp->_loop = _attrSet.getBool(_loopID);
    waypointComp->setCurve(curvePtr);
  } catch (Menge::ResourceException) {
    logger << Logger::ERR_MSG
           << "Couldn't instantiate the formation referenced on line ";
    logger << node->Row() << ".";
    return false;
  }
  return true;
}
}  // namespace Napoleon
