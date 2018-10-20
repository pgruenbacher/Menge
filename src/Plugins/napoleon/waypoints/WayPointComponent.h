
#ifndef _WAYPOINT_COMPONENT_H_
#define _WAYPOINT_COMPONENT_H_


#include "MengeCore/BFSM/FSMEnumeration.h"
#include "MengeCore/BFSM/VelocityComponents/VelComponent.h"
#include "MengeCore/BFSM/VelocityComponents/VelComponentFactory.h"
#include "MengeCore/Runtime/ReadersWriterLock.h"
#include "Plugins/Formations/FreeFormation.h"
#include "Plugins/napoleon/napoleonConfig.h"
#include "curve.h"
#include <vector>

namespace Napoleon {
  class WayPointsTask;

using Menge::Agents::BaseAgent;
using Menge::Agents::PrefVelocity;
using Menge::BFSM::Goal;
using Menge::BFSM::Task;
using Menge::BFSM::Task;
using Menge::BFSM::VelCompFactory;
using Menge::BFSM::VelComponent;
using Menge::Logger;
using Menge::Math::Vector2;
using Menge::logger;

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
  int _current_num_agents;
  int _current_max_agents;

  virtual ~WayPointComponent() {}
  CurvePtr _curve;
  WayPointsTask* _task;
  Menge::ReadersWriterLock _lock;
  VelComponent* parseVelComponent(TiXmlElement* node,
                                  const std::string& behaveFldr);

  // Vector2 _displacment;

 public:
  bool _loop;
  bool _auto;
  virtual void onEnter(BaseAgent* agent);
  virtual void onExit(BaseAgent* agent);
  void setCurve(CurvePtr form);
  CurvePtr getCurve() const { return _curve; }
  // void setDisplacement(Vector2 v);

  virtual void setPrefVelocity(const BaseAgent* agent, const Goal* goal,
                               PrefVelocity& pVel) const;

  // virtual std::string getStringId() const = 0;
  virtual std::string getStringId() const { return NAME; }

  /*! The unique identifier used to register this type with run-time components.
   */
  static const std::string NAME;
  virtual Task* getTask();
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
  size_t _autoID;
  size_t _loopID;
};
}  // namespace Napoleon

#endif  // _WAYPOINT_COMPONENT_H_
