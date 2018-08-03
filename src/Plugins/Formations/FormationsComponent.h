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
#ifndef _FORMATIONS_COMP_
#define _FORMATIONS_COMP_

// #include "MengeCore/BFSM/VelocityComponents/VelComponent.h"
// #include "MengeCore/BFSM/VelocityComponents/VelComponentFactory.h"
// #include "MengeCore/MengeException.h"
// #include "MengeCore/Agents/PrefVelocity.h"
// #include "MengeCore/BFSM/fsmCommon.h"
// #include "MengeCore/PluginEngine/Element.h"
// #include "FormationsConfig.h"
// #include "FreeFormation.h"
// #include "MengeCore/Runtime/ReadersWriterLock.h"
#include "FormationsConfig.h"
#include "FreeFormation.h"
#include "MengeCore/BFSM/FSMEnumeration.h"
#include "MengeCore/BFSM/VelocityComponents/VelComponent.h"
#include "MengeCore/BFSM/VelocityComponents/VelComponentFactory.h"
#include "MengeCore/Runtime/ReadersWriterLock.h"

#include <vector>


namespace Formations {
  using Menge::Agents::BaseAgent;
  using Menge::Agents::PrefVelocity;
  using Menge::BFSM::Goal;
  using Menge::BFSM::Task;
  using Menge::BFSM::VelCompFactory;
  using Menge::BFSM::VelComponent;
  using Menge::BFSM::Task;
  using Menge::Logger;
  using Menge::Math::Vector2;
  using Menge::logger;

class MENGE_API FormationComponent : public VelComponent {
 public:
  /*!
   *  @brief    Default constructor.
   */
  FormationComponent();

 protected:
  /*!
   *  @brief    Virtual destructor.
   */
  virtual ~FormationComponent() {}
  FormationPtr _formation;
  Menge::ReadersWriterLock _lock;
  VelComponent* parseVelComponent(TiXmlElement* node,
                                  const std::string& behaveFldr);

  Vector2 _displacment;

 public:

  virtual void onEnter(BaseAgent* agent);
  virtual void onExit(BaseAgent* agent);
  void registerAgent(const BaseAgent* agent);
  void unregisterAgent(const BaseAgent* agent);
  void setFormation(FormationPtr form);
  void setDisplacement(Vector2 v);

  virtual void setPrefVelocity(const BaseAgent* agent, const Goal* goal,
                               PrefVelocity& pVel) const;

  // virtual std::string getStringId() const = 0;
  virtual std::string getStringId() const { return NAME; }

  /*! The unique identifier used to register this type with run-time components.
   */
  static const std::string NAME;
  virtual Task * getTask();
  void loadSetFormation(const std::string& fname);
};

class MENGE_API FormationComponentFactory : public VelCompFactory {
 public:
  FormationComponentFactory();

  virtual const char* name() const { return FormationComponent::NAME.c_str(); }

  virtual const char* description() const { return "Formation Vel component"; };

 protected:
  VelComponent* instance() const { return new FormationComponent(); }

  virtual bool setFromXML(VelComponent* vc, TiXmlElement* node,
                          const std::string& behaveFldr) const;

  size_t _fileNameID;
  // displacmeent of the formation
  float _x;
  float _y;
};
}  // namespace Formations

#endif  // _FORMATIONS_COMP_
