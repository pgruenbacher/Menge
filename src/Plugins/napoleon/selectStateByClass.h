#ifndef _SELECT_STATE_BY_CLASS_
#define _SELECT_STATE_BY_CLASS_

#include "MengeCore/BFSM/Transitions/Target.h"
#include "MengeCore/BFSM/Transitions/TargetFactory.h"
#include "MengeCore/Runtime/ReadersWriterLock.h"

namespace Menge {


  namespace BFSM {

    class MENGE_API SelectStateByClassTarget : public TransitionTarget {
    public:
      SelectStateByClassTarget();

      SelectStateByClassTarget( const SelectStateByClassTarget & tgt);

      virtual void onEnter( Agents::BaseAgent * agent );

      virtual void onLeave( Agents::BaseAgent * agent );

      virtual State * nextState( Agents::BaseAgent * agent );

      virtual bool connectStates( std::map< std::string, State * > & stateMap );

      virtual TransitionTarget * copy();

      /*!
       *  @brief    The type of the state to which this transition leads.
       */
       std::string  _typeName;

    protected:
      /*!
       *  @brief    A mapping between an agent id and its return state.
       */
      std::map< size_t, State * > _targets;

      /*!
       *  @brief    Lock to protect _targets;
       */
      ReadersWriterLock _lock;
    };

    /*!
     *  @brief    The factory for creating the ReturnTarget
     */
    class MENGE_API SelectStateByClassTargetFactory : public TargetFactory {
    public:
      virtual const char * name() const { return "by_class"; }
      virtual const char * description() const {
        return "Gets class by type and class id";
      }
      virtual bool setFromXML( TransitionTarget * target, TiXmlElement * node,
                       const std::string & specFldr ) const;
    protected:
      // virtual SelectStateByClassTarget * instance() {return new SelectStateByClassTarget();};
      virtual TransitionTarget * instance() const { return new SelectStateByClassTarget(); }
    };

  } // namespace BFSM
} // namespace Menge
#endif  // _SELECT_STATE_BY_CLASS_
