#include "UserCommandTask.h"
#include "MengeCore/BFSM/FSM.h"
#include "MengeCore/Agents/SimulatorInterface.h"
#include "MengeCore/Agents/BaseAgent.h"
// #include "MengeCore/Core.h"
// #include "MengeCore/Agents/BaseAgent.h"

namespace Napoleon {
  UserCommandTask* UserCommandTask::USER_COMMAND_TASK = 0x0;

  using Menge::BFSM::FSM;
  using Menge::BFSM::Task;
  using Menge::BFSM::TaskException;

  // meant to synchronously apply damage to units.

  /////////////////////////////////////////////////////////////////////
  //                   Implementation of NearestEnemTask
  /////////////////////////////////////////////////////////////////////

  UserCommandTask::UserCommandTask() : Menge::BFSM::Task() {
  }
  UserCommandTask::~UserCommandTask() {
    USER_COMMAND_TASK = 0x0;
    // make sure to clear the singleton if deleting.
  }

  /////////////////////////////////////////////////////////////////////
  UserCommandTask* UserCommandTask::getSingleton() {
    if (USER_COMMAND_TASK == 0x0) {
      USER_COMMAND_TASK = new UserCommandTask();
    }
    return USER_COMMAND_TASK;
  }

  void UserCommandTask::doWork( const FSM * fsm ) {
    // std::cout << " USER TASK DO WORK " << std::endl;
  }

  std::string UserCommandTask::toString() const {
    return "User Command Task";
  }

  /////////////////////////////////////////////////////////////////////

  bool UserCommandTask::isEquivalent( const Task * task ) const {
    const UserCommandTask * other = dynamic_cast< const UserCommandTask * >( task );
    if ( other == 0x0 ) {
      return false;
    }
    // if (task == this) {
    //   std
    // }
    return true;
  }

  UserCommandTaskFactory::UserCommandTaskFactory() {
  }

  ///////////////////////////////////////////////////////////////////////////

  bool UserCommandTaskFactory::setFromXML( Menge::BFSM::Task* task, TiXmlElement * node,
                     const std::string & behaveFldr ) const {
    UserCommandTask * tCond = dynamic_cast< UserCommandTask * >( task );
    assert( tCond != 0x0 &&
        "Trying to set the properties of a user command task on an incompatible "
        "object" );

    if ( !Menge::BFSM::TaskFactory::setFromXML( task, node, behaveFldr ) ) return false;



    return true;
  }
  /////////////////////////////////////////////////////////////////////

} // namespace Napoleon
