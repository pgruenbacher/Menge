
#ifndef _USER_EVENT_H_
#define _USER_EVENT_H_

#include "MengeCore/Core.h"
#include "MengeCore/Agents/BaseAgent.h"
#include <vector>

class UserEvents {
  struct UserEvent {
    virtual void perform() const = 0;
  };

  typedef std::vector<const UserEvent*> UserEventVector;
  // napoleon specific stuff. not sure where to put...
  // enum UserEventType {
  //  PROJECTILE_COLLISION,
  // };
  UserEventVector _userEvents;

public:

  struct ProjectileCollision : UserEvent {
    size_t agentId;
    float damage;
    void perform() const override;
    ProjectileCollision(size_t agentId, float dmg) : agentId(agentId), damage(dmg) {}
  };

  void addUserEvent(const UserEvent* evt);
  void perform();
};

#endif // _USER_EVENT_H_
