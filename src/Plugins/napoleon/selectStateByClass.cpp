#include "selectStateByClass.h"
#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/BFSM/State.h"

namespace Menge {

  namespace BFSM {

    SelectStateByClassTarget::SelectStateByClassTarget():TransitionTarget(), _targets()  {
    }

    ///////////////////////////////////////////////////////////////////////////

    SelectStateByClassTarget::SelectStateByClassTarget(
        const SelectStateByClassTarget& tgt)
        : TransitionTarget(tgt),
          _typeName(tgt._typeName),
          _targets(tgt._targets) {}

    void SelectStateByClassTarget::onEnter( Agents::BaseAgent * agent ) {}

    void SelectStateByClassTarget::onLeave( Agents::BaseAgent * agent ) {}

    State * SelectStateByClassTarget::nextState( Agents::BaseAgent * agent ) {
      std::cout << "NEXT STATE" << agent->_class << std::endl;
      for (auto elem : _targets) {
        std::cout << "ELEM " << elem.first << std::endl;
      }
      if (!_targets[agent->_class]) {
        std::cout << "????" << std::endl;
      } else {
        std::cout << "AA " << _targets[agent->_class]->getName() << std::endl;
      }
      return _targets[agent->_class];
    }

    TransitionTarget * SelectStateByClassTarget::copy() {
      return new SelectStateByClassTarget( *this );
    }

    bool SelectStateByClassTarget::connectStates( std::map< std::string, State * > & stateMap ) {
      // if ( stateMap.find( _nextName ) == stateMap.end() ) {
      //   logger << Logger::ERR_MSG;
      //   logger << "SingleTarget with invalid to node name: " << _nextName << "\n";
      //   return false;
      // }
      // _next = stateMap[ _nextName ];
      _lock.lockWrite();
      for (std::map< std::string, State * >::const_iterator any = stateMap.begin(); any != stateMap.end(); ++any) {
        State* state = any->second;
        if (state->getType() == _typeName) {
          _targets[state->getClassId()] = state;
          std::cout << "NAM " << _targets[1] << std::endl;
          // std::cout << "NAM " << _targets[1]->getName() << std::endl;
        } else {
          std::cout << "SKIP " << state->getName() << " '" << state->getType() << "' '" << _typeName << std::endl;
        }
        if (!state || state == 0x0) {
          std::cout << "BAD INSTATN" << std::endl;
        } else {
          std::cout << "GODD " << state->getName() << std::endl;
        }
      }
      _lock.releaseWrite();
      // std::cout << " " << _targets[1]->getName() << std::endl;
      if (_targets.size() == 0) {
        logger << Logger::ERR_MSG;
        logger << "No states with type " << _typeName << " found\n";
      }
      return true;
    }

    bool SelectStateByClassTargetFactory::setFromXML( TransitionTarget * target, TiXmlElement * node,
                 const std::string & specFldr ) const {
      SelectStateByClassTarget * inst = dynamic_cast< SelectStateByClassTarget * >( target );
      _attrSet.clear();
      bool valid = _attrSet.extract( node );
      const char* typenamechars = node->Attribute("typename");
      if (typenamechars != 0x0) {
        inst->_typeName.assign(typenamechars);
      } else {
        return false;
      }
      return valid;
    }
  }  // namespace BFSM
}  // namespace Menge
