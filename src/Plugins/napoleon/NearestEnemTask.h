/*
 Menge Crowd Simulation Framework

 Copyright and trademark 2012-17 University of North Carolina at Chapel Hill

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0
 or
    LICENSE.txt in the root of the Menge repository.

 Any questions or comments should be sent to the authors menge@cs.unc.edu

 <http://gamma.cs.unc.edu/Menge/>
*/

/*!
 *  @file   FormationsTask.h
 *  @brief    Task used to maintain a single formation.
 */

#ifndef __NEAREST_ENEM_TASK_H__
#define __NEAREST_ENEM_TASK_H__

#include "MengeCore/BFSM/fsmCommon.h"
#include "MengeCore/BFSM/Tasks/Task.h"
#include "MengeCore/BFSM/Tasks/TaskFactory.h"

#include <string>

#include "thirdParty/tinyxml.h"

using namespace Menge;
namespace Napoleon {
    /*!
   *  @brief  Task responsible for updating agent data for maintaining a formation.
   */
  class NearestEnemTask : public Menge::BFSM::Task {
  public:
    /*!
     *  @brief    Constructor
     *
     */

    /*!
     *  @brief    The work performed by the task.
     *
     *  @param    fsm   The finite state machine for the task to operate on.
     *  @throws   A TaskException if there was some non-fatal error
     *        in execution.  It should be logged.
     *  @throws   A TaskFatalException if there is a fatal error that
     *        should arrest execution of the simulation.
     */
    virtual void doWork( const Menge::BFSM::FSM * fsm ) throw( Menge::BFSM::TaskException );

    /*!
     *  @brief    String representation of the task
     *
     *  @returns  A string containing task information.
     */
    virtual std::string toString() const;

    /*!
     *  @brief    Reports if this task is "equivalent" to the given task.
     *        This makes it possible for a task to be redundantly added
     *        to the fsm without fear of duplication as the equivalent
     *        duplicates will be culled.
     *
     *  @param    task    The task to test against this one.
     *  @returns  A boolean reporting if the two tasks are equivalent (true)
     *        or unique (false).
     */
    virtual bool isEquivalent( const Menge::BFSM::Task * task ) const;
  };
} // namespace Napoleon
#endif // __NEAREST_ENEM_TASK_H__
