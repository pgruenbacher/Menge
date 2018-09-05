
#ifndef __MENGE_NETWORK__
#define __MENGE_NETWORK__

#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/Math/Vector2.h"
#include "MengeCore/Agents/PrefVelocity.h"

namespace napoleon {

  using Menge::Agents::BaseAgent;
  using Menge::Agents::PrefVelocity;
  using Menge::Math::Vector2;

  struct Vector2Data {
    float x;
    float y;
    Vector2Data(const Menge::Math::Vector2&);
    bool operator==(const Vector2& v);
    bool operator!=(const Vector2& v);
    Vector2 vector();
  };

  struct PrefVelData {
      float         speed;
      Vector2Data   preferred;
      Vector2Data   target;
      // a bit more optional...
      Vector2Data   left;
      Vector2Data   right;
      PrefVelData(const PrefVelocity&);
      bool operator!=(const Menge::Agents::PrefVelocity& v);
      PrefVelocity prefVel();
  };

  struct AgentData {
    size_t id;
    Vector2Data pos;
    Vector2Data orient;
    Vector2Data vel;

    PrefVelData prefVel;

    AgentData(const BaseAgent& fromAgt);
    bool updateAgent(BaseAgent& toAgt);
  };


  struct AgentDataOptional {
    // optional values for initial loadding
    // but shouldn't have to be synced after...
    float _neighborDist;

    //added by paul for tracking enemy units
    //for combat fsm mechanics, not for crowd movement.
    float _enemDist;
    size_t _maxEnem;
    size_t _class;
    float _maxAngVel;
  };

}

#endif // __MENGE_NETWORK__