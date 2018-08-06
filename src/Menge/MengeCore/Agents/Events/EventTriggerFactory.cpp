#include "MengeCore/Agents/Events/EventTriggerFactory.h"

namespace Menge {

	/////////////////////////////////////////////////////////////////////
	//					Implementation of EventTriggerFactory
	/////////////////////////////////////////////////////////////////////



	EventTriggerFactory::EventTriggerFactory() {
		_nameID = _attrSet.addStringAttribute("name", true, "");
		_firePeriodID = _attrSet.addFloatAttribute("fire_period", false, 0.f);
	}

	/////////////////////////////////////////////////////////////////////

	bool EventTriggerFactory::setFromXML(EventTrigger * trigger, TiXmlElement * node,
										 const std::string & specFldr) const {
		if (!ElementFactory<EventTrigger>::setFromXML(trigger, node, specFldr)) return false;
		trigger->_name = _attrSet.getString(_nameID);
		trigger->_firePeriod = _attrSet.getFloat(_firePeriodID);
		return true;
	}

}	// namespace Menge
