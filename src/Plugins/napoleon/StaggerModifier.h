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
 *  @file   FormationsModifier.h
 *  @brief    definition of a VelocityModifier to enforce formation behavior
 */
#ifndef _STAGGER_MODIFIER_H_
#define _STAGGER_MODIFIER_H_

#include "MengeCore/BFSM/FSMEnumeration.h"
#include "MengeCore/BFSM/VelocityModifiers/VelModifier.h"
#include "MengeCore/BFSM/VelocityModifiers/VelModifierFactory.h"
#include "MengeCore/Runtime/ReadersWriterLock.h"
#include <map>

class TiXmlElement;

namespace Napoleon {

class StaggerModifierFactory;

class MENGE_API StaggerModifier : public Menge::BFSM::VelModifier {
 public:
  StaggerModifier();

 protected:
  // std::vector<Vector2> _stagger_dirs;
  virtual ~StaggerModifier();
  std::map<size_t, float> _local_dist_travelled;
  Menge::ReadersWriterLock _lock;
  void adjustDistTravelled(const Menge::Agents::BaseAgent* agent);
  float maxDistTravelled;

 public:
  Menge::BFSM::VelModifier* copy() const;
  Menge::BFSM::Task* getTask() override;
  void setMaxDistTravelled(float v) { maxDistTravelled = v * v; }
  void adaptPrefVelocity(const Menge::Agents::BaseAgent* agent,
                         Menge::Agents::PrefVelocity& pVel);

  void registerAgent(const Menge::Agents::BaseAgent* agent);
  void unregisterAgent(const Menge::Agents::BaseAgent* agent);
  friend class StaggerModifierFactory;

 protected:
};

///////////////////////////////////////////////////////////////////////////////

/*!
 *  @brief    The factory class for the StaggerModifier
 */
class MENGE_API StaggerModifierFactory : public Menge::BFSM::VelModFactory {
 public:
  /*!
   *  @brief    Constructor.
   */
  StaggerModifierFactory();

  virtual const char* name() const { return "stagger"; }

  virtual const char* description() const {
    return "Perform stagger movement.";
  };

 protected:
  Menge::BFSM::VelModifier* instance() const { return new StaggerModifier(); }

  virtual bool setFromXML(Menge::BFSM::VelModifier* modifier,
                          TiXmlElement* node,
                          const std::string& behaveFldr) const;
};
}  // namespace Napoleon
#endif  // _STAGGER_MODIFIER_H_
