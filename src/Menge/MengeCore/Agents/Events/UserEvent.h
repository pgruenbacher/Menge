
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

  struct SpawnAgents : UserEvent {
    size_t groupId;
    const char* profileName;
    const std::vector<Menge::Math::Vector2> points;
    void perform() const override;
    SpawnAgents(int groupId, const std::vector<Menge::Math::Vector2>& pts) :
      groupId(groupId), points(pts) {};
  };

  struct ProjectileCollision : UserEvent {
    size_t agentId;
    float damage;
    void perform() const override;
    ProjectileCollision(size_t agentId, float dmg) : agentId(agentId), damage(dmg) {}
  };

  struct AddFormation : UserEvent {
    int groupId;
    const std::vector<Menge::Math::Vector2> points;
    void perform() const override;
    AddFormation(int groupId, const std::vector<Menge::Math::Vector2>& pts) :
      groupId(groupId), points(pts) {}
  };

  struct ToFormation : UserEvent {
    int groupId;
    int formationId;
    void perform() const override;
    ToFormation(int groupId, int formationId) :
      groupId(groupId), formationId(formationId) {}
  };

  struct AddWaypoints : UserEvent {
    int groupId;
    bool autoStart;
    const std::vector<Menge::Math::Vector2> points;
    void perform() const override;
    AddWaypoints(int groupId) : groupId(groupId) {};
    AddWaypoints(int groupId, const std::vector<Menge::Math::Vector2>& pts) :
      groupId(groupId), points(pts) {};
  };

  struct ToWaypoints : UserEvent {
    int groupId;
    void perform() const override;
    ToWaypoints(int groupId) : groupId(groupId) {}
  };

  struct CanFire : UserEvent {
    int groupId;
    void perform() const override;
    CanFire(int groupId) : groupId(groupId) {}
  };

  void addUserEvent(const UserEvent* evt);
  void perform();
};

#endif // _USER_EVENT_H_
