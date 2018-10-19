
#ifndef _WAYPOINT_COMPONENT_H_
#define _WAYPOINT_COMPONENT_H_

// #include "MengeCore/BFSM/VelocityComponents/VelComponent.h"
// #include "MengeCore/BFSM/VelocityComponents/VelComponentFactory.h"
// #include "MengeCore/MengeException.h"
// #include "MengeCore/Agents/PrefVelocity.h"
// #include "MengeCore/BFSM/fsmCommon.h"
// #include "MengeCore/PluginEngine/Element.h"
// #include "FormationsConfig.h"
// #include "FreeFormation.h"
// #include "MengeCore/Runtime/ReadersWriterLock.h"
#include "Plugins/napoleon/napoleonConfig.h"
#include "Plugins/Formations/FreeFormation.h"
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

Menge::BFSM::State* makeFormationState(int uniqueIndex, size_t classId);

class MENGE_API WayPointComponent : public VelComponent {
 public:
  /*!
   *  @brief    Default constructor.
   */
  WayPointComponent();

 protected:
  /*!
   *  @brief    Virtual destructor.
   */
  virtual ~WayPointComponent() {}
  FormationPtr _formation;
  Menge::ReadersWriterLock _lock;
  VelComponent* parseVelComponent(TiXmlElement* node,
                                  const std::string& behaveFldr);

  // Vector2 _displacment;

 public:

  virtual void onEnter(BaseAgent* agent);
  virtual void onExit(BaseAgent* agent);
  void registerAgent(const BaseAgent* agent);
  void unregisterAgent(const BaseAgent* agent);
  void setFormation(FormationPtr form);
  FormationPtr getFormation() const { return _formation; }
  // void setDisplacement(Vector2 v);

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

class MENGE_API WayPointComponentFactory : public VelCompFactory {
 public:
  WayPointComponentFactory();

  virtual const char* name() const { return WayPointComponent::NAME.c_str(); }

  virtual const char* description() const { return "Waypoint Vel component"; };

 protected:
  VelComponent* instance() const { return new WayPointComponent(); }

  virtual bool setFromXML(VelComponent* vc, TiXmlElement* node,
                          const std::string& behaveFldr) const;

  size_t _fileNameID;
  // displacmeent of the formation
  float _x;
  float _y;
};
}  // namespace Formations

#endif  // _WAYPOINT_COMPONENT_H_
