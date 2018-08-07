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
 *  @file   AircraftAction.h
 *  @brief    Definition of actions used in Aircraft
 *        loading and unloading.
 */
#ifndef __ATTACK_ACTION_H_
#define __ATTACK_ACTION_H_

#include "MengeCore/BFSM/FSMEnumeration.h"
#include "MengeCore/BFSM/Actions/Action.h"
#include "MengeCore/BFSM/Actions/ActionFactory.h"
#include "MengeCore/Runtime/SimpleLock.h"
#include "MengeCore/Math/RandGenerator.h"
#include <map>

//forward declaration
class TiXmlElement;

namespace Napoleon {
  // forward declaration
  class AttackActionFactory;
  class MENGE_API AttackAction : public Menge::BFSM::Action {
  public:
    /*!
     *  @brief    Constructor
     */
    AttackAction();

    /*!
     *  @brief    Virtual destructor.
     */
    ~AttackAction();

    /*!
     *  @brief    Upon entering the state, this is called -- it is the main work
     *        of the action.
     *
     *  @param    agent   The agent to act on.
     */
    void onEnter( Menge::Agents::BaseAgent * agent );

    friend class AttackActionFactory;

  protected:

    /*!
     *  @brief    The work to do upon state exit.
     *
     *  @param    agent   The agent to act on.
     */
    void leaveAction( Menge::Agents::BaseAgent * agent );

  protected:
    Menge::SimpleLock _lock;
    Menge::Math::UniformFloatGenerator _randGenerator;
  };

  class MENGE_API AttackActionFactory : public Menge::BFSM::ActionFactory {
  public:
    /*!
     *  @brief    The name of the action.
     *
     *  The action's name must be unique among all registered actions.
     *  Each action factory must override this function.
     *
     *  @returns  A string containing the unique action name.
     */
    virtual const char * name() const { return "attack_action"; }

    /*!
     *  @brief    A description of the action.
     *
     *  Each action factory must override this function.
     *
     *  @returns  A string containing the action description.
     */
    virtual const char * description() const {
      return "Sets agents attack properties";
    };

  protected:
    /*!
     *  @brief    Create an instance of this class's action.
     *
     *  All ActionFactory sub-classes must override this by creating (on the heap)
     *  a new instance of its corresponding action type.  The various field values
     *  of the instance will be set in a subsequent call to ActionFactory::setFromXML.
     *  The caller of this function takes ownership of the memory.
     *
     *  @returns    A pointer to a newly instantiated Action class.
     */
    Menge::BFSM::Action * instance() const { return new AttackAction(); }

    /*!
     *  @brief    Given a pointer to an Action instance, sets the appropriate fields
     *        from the provided XML node.
     *
     *  It is assumed that the value of the `type` attribute is this Action's type.
     *  (i.e. ActionFactory::thisFactory has already been called and returned true.)
     *  If sub-classes of ActionFactory introduce *new* Action parameters, then the
     *  sub-class should override this method but explicitly call the parent class's
     *  version.
     *
     *  @param    action    A pointer to the action whose attributes are to be set.
     *  @param    node    The XML node containing the action attributes.
     *  @param    behaveFldr  The path to the behavior file.  If the action references
     *              resources in the file system, it should be defined relative
     *              to the behavior file location.  This is the folder containing
     *              that path.
     *  @returns  A boolean reporting success (true) or failure (false).
     */
    virtual bool setFromXML( Menge::BFSM::Action * action, TiXmlElement * node,
                 const std::string & behaveFldr ) const;
  };
} // namespace Napoleon

#endif  // __ATTACK_ACTION_H_
