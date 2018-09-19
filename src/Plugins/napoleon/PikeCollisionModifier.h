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
#ifndef _PIKE_COLLISION_MODIFIER_H_
#define _PIKE_COLLISION_MODIFIER_H_

#include "MengeCore/BFSM/FSMEnumeration.h"
#include "MengeCore/BFSM/VelocityModifiers/VelModifier.h"
#include "MengeCore/BFSM/VelocityModifiers/VelModifierFactory.h"
#include "MengeCore/Runtime/ReadersWriterLock.h"

class TiXmlElement;

namespace Napoleon {

  class PikeCollisionModifierFactory;

  class MENGE_API PikeCollisionModifier : public Menge::BFSM::VelModifier {
  public:

    PikeCollisionModifier();

  protected:

    virtual ~PikeCollisionModifier();

  public:

    Menge::BFSM::VelModifier* copy() const;

    Menge::BFSM::Task * getTask() override;
    void adaptPrefVelocity( const Menge::Agents::BaseAgent * agent,
                Menge::Agents::PrefVelocity & pVel );


    friend class PikeCollisionModifierFactory;

  protected:


  };

  ///////////////////////////////////////////////////////////////////////////////

  /*!
   *  @brief    The factory class for the PikeCollisionModifier
   */
  class MENGE_API PikeCollisionModifierFactory : public Menge::BFSM::VelModFactory {
  public:
    /*!
     *  @brief    Constructor.
     */
    PikeCollisionModifierFactory();

    virtual const char * name() const { return "pike_collision"; }

    virtual const char * description() const {
      return "Perform collision movmeent with pike points.";
    };

  protected:

    Menge::BFSM::VelModifier * instance() const { return new PikeCollisionModifier(); }


    virtual bool setFromXML( Menge::BFSM::VelModifier * modifier, TiXmlElement * node,
                 const std::string & behaveFldr ) const;


  };
} // namespace Napoleon
#endif  // _PIKE_COLLISION_MODIFIER_H_
