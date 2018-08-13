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
 *  @file   VelComponent.h
 *  @brief    The definition of how preferred velocity is computed in a state.
 */
#ifndef _IDLE_COMPONENT_H_
#define _IDLE_COMPONENT_H_

#include "MengeCore/BFSM/FSMEnumeration.h"
#include "MengeCore/BFSM/VelocityComponents/VelComponent.h"
#include "MengeCore/BFSM/VelocityComponents/VelComponentFactory.h"
#include "MengeCore/Agents/BaseAgent.h"
// #include <vector>


namespace Napoleon {
  using Menge::Agents::BaseAgent;
  using Menge::Agents::PrefVelocity;
  using Menge::BFSM::Goal;
  using Menge::BFSM::VelCompFactory;
  using Menge::BFSM::VelComponent;

class MENGE_API IdleComponent : public VelComponent {

 public:


  virtual void setPrefVelocity(const BaseAgent* agent, const Goal* goal,
                               PrefVelocity& pVel) const;
  virtual std::string getStringId() const { return NAME; }
  static const std::string NAME;
  void loadSetFormation(const std::string& fname);
};

class MENGE_API IdleComponentFactory : public VelCompFactory {
 public:
  IdleComponentFactory();

  virtual const char* name() const { return IdleComponent::NAME.c_str(); }

  virtual const char* description() const { return "Idle component"; };

 protected:
  VelComponent* instance() const { return new IdleComponent(); }
  size_t _method;
  virtual bool setFromXML(VelComponent* vc, TiXmlElement* node,
                          const std::string& behaveFldr) const;

};
}  // namespace Napoleon

#endif  // _IDLE_COMPONENT_H_
