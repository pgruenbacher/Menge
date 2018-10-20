
#ifndef _WAYPOINT_TASK_H_
#define _WAYPOINT_TASK_H_

#include "curve.h"

#include "MengeCore/BFSM/fsmCommon.h"
#include "MengeCore/BFSM/Tasks/Task.h"
#include "MengeCore/BFSM/Tasks/TaskFactory.h"

#include <string>

#include "thirdParty/tinyxml.h"

using namespace Menge;

namespace Napoleon {


  class WayPointsTask : public Menge::BFSM::Task {

    float getSpeed() { return 0.5; };
    float currentPosition;
    bool _canPerform;

  public:

    WayPointsTask( CurvePtr & curve );
    ~WayPointsTask(  );

    void setCanPerform(bool v) { _canPerform = v; }

    virtual void doWork( const Menge::BFSM::FSM * fsm );

    float getCurrentPosition() { return currentPosition; }

    virtual std::string toString() const;

    virtual bool isEquivalent( const Menge::BFSM::Task * task ) const;

    const CurvePtr getCurve() const { return _curve;}

  protected:

    CurvePtr _curve; //the formation
  };
} // namespace Napoleon
#endif // _WAYPOINT_TASK_H_
