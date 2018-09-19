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
#ifndef _PIKE_ACTION_H_
#define _PIKE_ACTION_H_

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
  class PikeActionFactory;
  class MENGE_API PikeAction : public Menge::BFSM::Action {
  public:

    PikeAction();


    ~PikeAction();

    bool lowering;
    void onEnter( Menge::Agents::BaseAgent * agent );
    Menge::BFSM::Task* getTask();

    friend class PikeActionFactory;

  protected:

    void leaveAction( Menge::Agents::BaseAgent * agent );
    void resetAction(Menge::Agents::BaseAgent * agent) override;

  protected:
    Menge::SimpleLock _lock;
    // Menge::Math::UniformFloatGenerator _randGenerator;
  };

  class MENGE_API PikeActionFactory : public Menge::BFSM::ActionFactory {
  public:
    PikeActionFactory();

    virtual const char * name() const { return "set_pike"; }

    virtual const char * description() const {
      return "Sets agents pike status";
    };

  protected:
    // whether or not the lowered pike will be "added" into the scene.
    size_t _loweringID;

    Menge::BFSM::Action * instance() const { return new PikeAction(); }

    virtual bool setFromXML( Menge::BFSM::Action * action, TiXmlElement * node,
                 const std::string & behaveFldr ) const;
  };
} // namespace Napoleon

#endif  // _PIKE_ACTION_H_
