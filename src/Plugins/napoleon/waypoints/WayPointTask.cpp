#include "WayPointTask.h"
#include "MengeCore/BFSM/FSM.h"
#include "MengeCore/Core.h"

namespace Napoleon {

  using Menge::BFSM::FSM;
  using Menge::BFSM::Task;
  using Menge::BFSM::TaskException;
  using Menge::BFSM::TaskFactory;

  /////////////////////////////////////////////////////////////////////
  //                   Implementation of WayPointsTask
  /////////////////////////////////////////////////////////////////////

  WayPointsTask::WayPointsTask( CurvePtr & f) : Task(), _curve(f), _isDirty(false), currentPosition(0.f), _formation("filename") {
  }

  WayPointsTask::~WayPointsTask() {
    // std::cout << "DELETE TASK ? " << std::endl;
  }

  int roundUp(int x, int y) {
    // https://stackoverflow.com/questions/2745074/fast-ceiling-of-an-integer-division-in-c-c
    return (x + y - 1) / y;
  }

  void WayPointsTask::makeOffsetPoints() {
    const int numAgents = _formation.getNumAgents();
    const float spacing = 1.0;
    const float numWide = 4;

    float x = 0;
    float y = 0;

    const int numRows = roundUp(numAgents, numWide);

    _offsetPoints.clear();
    // _offsetPoints.reserve(numRows * numWide);

    int i = 0;
    for (int yi = 0; yi < numRows; ++yi) {
      y = - yi * spacing;
      for (int xi = 0; xi < numWide; ++xi) {
        x = xi * spacing - (spacing * numWide / 2);
        _offsetPoints.emplace(_offsetPoints.begin() + i, x, y);
        // _offsetPoints[i] = Vector2(x, y);
        // _offsetPoints.push_back(Vector2(x, y));
        ++i;
      }
    }

    // for (int i = 0; i < _offsetPoints.size(); ++i) {
    //   std::cout << " OFFSET POITN " << _offsetPoints[i].x() << " " << _offsetPoints[i].y() << std::endl;
    // }
  }

  void WayPointsTask::makeFollowPoints(const std::vector<Vector2>& points) {

    // std::vector<PointFollow> followPoints;
    std::vector<PointFollow>& followPoints = _followPoints;

    followPoints.clear();
    followPoints.reserve(points.size());

    float voffset = 0.f;
    float offset = 0.f;
    for (int i = 0; i < points.size(); ++i) {
      voffset = points[i].x();
      offset = points[i].y();
      followPoints.emplace(followPoints.begin() + i, offset, voffset);
    }
  }

  void WayPointsTask::updateFollowPoints(std::vector<PointFollow>& points) {
    for (PointFollow& follow : points) {
      follow.updateTransform(_curve, false, currentPosition);
    }
  }

  Vector2 WayPointsTask::getGoalForAgent(const Menge::Agents::BaseAgent* agt) {
    int index;
    bool valid = _formation.getPointIndexForAgent(agt, index);
    if (valid) {
      if (index < _followPoints.size()) {
        return _followPoints[index].getPosition();
      }
    }
    return Vector2(0.f, 0.f);
  }

  void WayPointsTask::addAgent( const Menge::Agents::BaseAgent *agt ) {
    _formation.addAgent(agt);
    _isDirty = true;
  };

  void WayPointsTask::removeAgent( const Menge::Agents::BaseAgent *agt ) {
    _formation.removeAgent(agt);
    _isDirty = true;
  };

  void WayPointsTask::updateFormation(const std::vector<PointFollow>& followPoints) {
    _formation.clearFormationPoints();
    for (int i = 0; i < followPoints.size(); ++i) {
      _formation.addFormationPoint(followPoints[i].getPosition(), true, 1.f);
    }
  }

  void WayPointsTask::mapAgentsToFollowPoints() {
    _formation.normalizeFormation();
    // for (const PointFollow& pt : _followPoints) {
    //   std::cout << "FOL " << pt.getPosition() << std::endl;
    // }
    _formation.mapAgentsToFormation();
  }

  /////////////////////////////////////////////////////////////////////

  void WayPointsTask::doWork( const FSM * fsm )  {

    if (_isDirty) {
      makeOffsetPoints();
      makeFollowPoints(_offsetPoints);
      updateFollowPoints(_followPoints);
      updateFormation(_followPoints);
      mapAgentsToFollowPoints();
      _isDirty = false;
    }

    if (!_canPerform) return;
    updateFollowPoints(_followPoints);
    currentPosition += (getSpeed() * Menge::SIM_TIME_STEP);
    // std::cout << currentPosition
    //   << " " << _curve->get_baked_length()
    //   << " " << _curve->interpolate_baked(currentPosition) << std::endl;
  }

  /////////////////////////////////////////////////////////////////////

  std::string WayPointsTask::toString() const {
    return "WayPoints Task";
  }

  /////////////////////////////////////////////////////////////////////

  bool WayPointsTask::isEquivalent( const Task * task ) const {
    const WayPointsTask * other = dynamic_cast< const WayPointsTask * >( task );
    if ( other == 0x0 ) {
      return false;
    } else {
      other->getCurve();
      if ( other->getCurve() == _curve){
        return true;
      } else {
          return false;
      }
    }
  }

  /////////////////////////////////////////////////////////////////////

} // namespace Napoleon
