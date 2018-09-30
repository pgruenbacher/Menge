#ifndef _NAP_STATES_H_
#define _NAP_STATES_H_

#include<cstdlib>
#include<cwchar>

enum MENGE_STATE {
    NONE,
    ATTACK,
    IDLE,
    DYING,
    DEAD,
    WITHDRAWING,
    ADVANCING,
    AIMING,
    BRACING,
    FIRING,
};

MENGE_STATE get_state(size_t agent_id);

#endif  // _NAP_STATES_H_
