

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
 *  @file   FreeFormation.h
 *  @brief    Implementatin of freestyle formations.
 *
 * An implemenatation of the paper at:
 * http://graphics.cs.uh.edu/wp-content/papers/2013/2011_CGA-crowd-formation-generation-preprint.pdf
 */

#ifndef _STEADY_FORMATION_H_
#define _STEADY_FORMATION_H_

#include "FreeFormation.h"
#include <map>
#include <vector>

/*!
 *  @namespace    Formations
 *  @brief      The name space for the Formation Model
 *
 *  This formation model is based off of
 *http://graphics.cs.uh.edu/wp-content/papers/2013/2011_CGA-crowd-formation-generation-preprint.pdf
 *
 * We use input files which contain the formation coordinates to
 * define a formation, and a task and velocity modifier to execute them.
 */

namespace Formations {
  using Menge::Math::Vector2;

  /*!
   * @brief   The class for modeling a freestyle formation.
   */
  class SteadyFormation : public FreeFormation {
  public:
    /*!
     *  @brief    Constructor
     *
     *  @param  name    The name of the file containing the target formation.
     */
    SteadyFormation(const std::string& name);
    static Menge::Resource * load( const std::string & fileName );
    static Menge::Resource * make( const std::string & name) { return new SteadyFormation(name);}

    /*!
     *  @brief    Destructor
     */
    ~SteadyFormation();

    /*!
     *  @brief    Reports the message label for this resource.
     *
     *  @returns  The label for formations.
     */
    virtual const std::string & getLabel() const { return LABEL; }
    static const std::string LABEL;
    void mapAgentsToFormation() override;
    bool setShouldUpdate() { _shouldUpdate = true; };
    protected:
      bool _shouldUpdate;
  };

} // namespace Formations
#endif  // _STEADY_FORMATION_H_
