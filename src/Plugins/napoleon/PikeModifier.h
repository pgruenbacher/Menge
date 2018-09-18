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
#ifndef _TURN_MODIFIER_H_
#define _TURN_MODIFIER_H_

#include "MengeCore/BFSM/FSMEnumeration.h"
#include "MengeCore/BFSM/VelocityModifiers/VelModifier.h"
#include "MengeCore/BFSM/VelocityModifiers/VelModifierFactory.h"
#include "MengeCore/Runtime/ReadersWriterLock.h"

class TiXmlElement;

namespace Napoleon {

  class PikeModifierFactory;

  class MENGE_API PikeModifier : public Menge::BFSM::VelModifier {
  public:

    PikeModifier();

  protected:

    virtual ~PikeModifier();

  public:

    Menge::BFSM::VelModifier* copy() const;

    void adaptPrefVelocity( const Menge::Agents::BaseAgent * agent,
                Menge::Agents::PrefVelocity & pVel );


    friend class PikeModifierFactory;

  protected:


  };

  ///////////////////////////////////////////////////////////////////////////////

  /*!
   *  @brief    The factory class for the PikeModifier
   */
  class MENGE_API PikeModifierFactory : public Menge::BFSM::VelModFactory {
  public:
    /*!
     *  @brief    Constructor.
     */
    PikeModifierFactory();

    virtual const char * name() const { return "turn"; }

    virtual const char * description() const {
      return "Perform turn steer limits for horsemen.";
    };

  protected:

    Menge::BFSM::VelModifier * instance() const { return new PikeModifier(); }


    virtual bool setFromXML( Menge::BFSM::VelModifier * modifier, TiXmlElement * node,
                 const std::string & behaveFldr ) const;


  };
} // namespace Napoleon
#endif  // _TURN_MODIFIER_H_
