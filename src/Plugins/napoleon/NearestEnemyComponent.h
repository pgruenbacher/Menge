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
#ifndef _NEAREST_ENEM_COMP_H_
#define _NEAREST_ENEM_COMP_H_

#include "MengeCore/BFSM/FSMEnumeration.h"
#include "MengeCore/BFSM/VelocityComponents/VelComponent.h"
#include "MengeCore/BFSM/VelocityComponents/VelComponentFactory.h"
#include "MengeCore/Runtime/ReadersWriterLock.h"
#include "MengeCore/Agents/BaseAgent.h"
#include "NearestEnemTask.h"
#include <vector>
#include <map>


namespace Napoleon {
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
  using Menge::Agents::NearAgent;

class MENGE_API NearestEnemComponent : public VelComponent {

 public:
  /*!
   *  @brief    Default constructor.
   */
  NearestEnemComponent();
  enum ActionType {
    ADVANCING,
    WITHDRAWING,
    IDLE,
  };


 protected:
  /*!
   *  @brief    Virtual destructor.
   */
  // Menge::ReadersWriterLock _lock;
  // std::map< size_t, const Menge::Agents::BaseAgent * > _agents;
  // std::map< size_t, const Menge::Agents::BaseAgent * > _to_enem_agents;

  virtual ~NearestEnemComponent() {}
  // VelComponent* parseVelComponent(TiXmlElement* node,
  //                                 const std::string& behaveFldr);
  void setAdvancingVelocity(const BaseAgent* enem, const BaseAgent* agent, PrefVelocity& pVel, float distSq) const;
  void setWithdrawingVelocity(const BaseAgent* enem, const BaseAgent* agent,
                              PrefVelocity& pVel, float distSq) const;
  void setIdleVelocity(const BaseAgent* enem, const BaseAgent* agent, const Goal* goal,
                               PrefVelocity& pVel) const;

 public:

  virtual void onEnter(BaseAgent* agent);
  virtual void onExit(BaseAgent* agent);
  void registerAgent(const BaseAgent* agent);
  void unregisterAgent(const BaseAgent* agent);
  const NearAgent getTargetEnem(const BaseAgent* agent) const;

  virtual void setPrefVelocity(const BaseAgent* agent, const Goal* goal,
                               PrefVelocity& pVel) const;

  // virtual std::string getStringId() const = 0;
  virtual std::string getStringId() const { return NAME; }

  NearestEnemMethod _targetMethod;
  ActionType _actionType;
  // whether unit should slow to arrive when approaching enemy. (cavalry)
  bool _slowToArrive;
  /*! The unique identifier used to register this type with run-time components.
   */
  static const std::string NAME;
  virtual Task * getTask();
  void loadSetFormation(const std::string& fname);
};

class MENGE_API NearestEnemComponentFactory : public VelCompFactory {
 public:
  NearestEnemComponentFactory();

  virtual const char* name() const { return NearestEnemComponent::NAME.c_str(); }

  virtual const char* description() const { return "Aiming component"; };

 protected:
  VelComponent* instance() const { return new NearestEnemComponent(); }
  size_t _methodID;
  size_t _targetMethodID;
  size_t _slowToArriveID;
  virtual bool setFromXML(VelComponent* vc, TiXmlElement* node,
                          const std::string& behaveFldr) const;

};
}  // namespace Napoleon

#endif  // _NEAREST_ENEM_COMP_H_
