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

  WayPointsTask::WayPointsTask( CurvePtr & f) : Task(), _curve(f), currentPosition(0.f) {
  }

  WayPointsTask::~WayPointsTask() {
    // std::cout << "DELETE TASK ? " << std::endl;
  }

  /////////////////////////////////////////////////////////////////////

  void WayPointsTask::doWork( const FSM * fsm )  {
    if (_canPerform) {
      currentPosition += (getSpeed() * Menge::SIM_TIME_STEP);
      std::cout << currentPosition
        << " " << _curve->get_baked_length()
        << " " << _curve->interpolate_baked(currentPosition) << std::endl;
    }
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
