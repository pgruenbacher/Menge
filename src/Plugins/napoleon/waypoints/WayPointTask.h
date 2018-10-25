
#ifndef _WAYPOINT_TASK_H_
#define _WAYPOINT_TASK_H_

#include "curve.h"

#include "MengeCore/BFSM/fsmCommon.h"
#include "MengeCore/BFSM/Tasks/Task.h"
#include "MengeCore/BFSM/Tasks/TaskFactory.h"
#include "Plugins/Formations/FreeFormation.h"

#include <string>

#include "thirdParty/tinyxml.h"

using namespace Menge;

namespace Napoleon {

  class WayPointsTask : public Menge::BFSM::Task {

    float getSpeed() { return 2.0; };
    float currentPosition;
    bool _canPerform;
    bool _isDirty;
    Formations::FreeFormation _formation;
    std::vector<PointFollow> _followPoints;
    std::vector<Vector2> _offsetPoints;

    void makeOffsetPoints();
    void updateFormation(const std::vector<PointFollow>& points);
    void makeFollowPoints(const std::vector<Vector2>& points);
    void updateFollowPoints(std::vector<PointFollow>& points);
    void mapAgentsToFollowPoints();
  public:

    WayPointsTask( CurvePtr & curve );
    ~WayPointsTask(  );

    void setCanPerform(bool v) { _canPerform = v; }
    void addAgent( const Menge::Agents::BaseAgent *agt );
    void removeAgent( const Menge::Agents::BaseAgent *agt );
    void reset() { currentPosition = 0.f; _isDirty = true; _canPerform = false; }

    virtual void doWork( const Menge::BFSM::FSM * fsm );

    float getCurrentPosition() { return currentPosition; }

    virtual std::string toString() const;
    Vector2 getGoalForAgent(const Menge::Agents::BaseAgent* agt);
    virtual bool isEquivalent( const Menge::BFSM::Task * task ) const;

    const CurvePtr getCurve() const { return _curve;}

  protected:

    CurvePtr _curve; //the formation
  };
} // namespace Napoleon
#endif // _WAYPOINT_TASK_H_
