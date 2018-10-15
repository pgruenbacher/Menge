#ifndef _USSER_COMMAND_TASK_H_
#define _USSER_COMMAND_TASK_H_

#include "MengeCore/BFSM/Tasks/Task.h"
#include "MengeCore/BFSM/Tasks/TaskFactory.h"
#include <map>

using namespace Menge;
namespace Napoleon {

struct UserGroupCommand {
  bool move_to_formation;
  bool can_fire;
};

/*!
* @brief  Task responsible for updating agent data for maintaining a formation.
*/
class UserCommandTask : public Menge::BFSM::Task {
 private:

  static UserCommandTask* USER_COMMAND_TASK;

  std::map<int, UserGroupCommand> _group_commands;

 public:
  UserCommandTask();
  ~UserCommandTask();

  void setGroupCommand(int id, UserGroupCommand cmd) { _group_commands[id] = cmd; }

  virtual void doWork(const Menge::BFSM::FSM* fsm) throw(
      Menge::BFSM::TaskException);

  virtual std::string toString() const;

  virtual bool isEquivalent(const Menge::BFSM::Task* task) const;

  static UserCommandTask* getSingleton();
};

class MENGE_API UserCommandTaskFactory : public Menge::BFSM::TaskFactory {
public:
  /*!
   *  @brief    Constructor.
   */
  UserCommandTaskFactory();

  virtual const char * name() const { return "user_command_task"; }

  virtual const char * description() const {
    return "User task.";
  }

protected:

  virtual Menge::BFSM::Task * instance() const { return new UserCommandTask(); }
  virtual bool setFromXML( Menge::BFSM::Task * condition, TiXmlElement * node,
               const std::string & behaveFldr ) const;
};
}  // namespace Napoleon
#endif  // _USSER_COMMAND_TASK_H_
