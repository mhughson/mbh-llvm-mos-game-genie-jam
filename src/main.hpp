#ifndef B78B5263_80F5_427B_82AB_4E62FB56CCA0
#define B78B5263_80F5_427B_82AB_4E62FB56CCA0

#include <cstdint>
#include <fixed_point.h>
using namespace fixedpoint_literals;

#define NUM_ENTITIES 8
#define MAX_AMMO 10

// Custom MIN/MAX macros that do not double evaluate the inputs
#define MMAX(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define MMIN(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _b : _a; })

#define MABS(a) \
   ({ __typeof__ (a) _a = (a); \
     _a < 0 ? -_a : _a; }) 

// The different states and any entity can be in.
enum Entity_States
{
    UNUSED = 0,
    ACTIVE = 1,
};

enum Entity_Types
{
    ENTITY_TYPE_NONE = 0,
    ENTITY_TYPE_ENEMY = 1,
    ENTITY_TYPE_AMMO = 2,
};

// Generic entity class for physical objects in the world.
class Entity
{
public:

    // position
    fu8_8 x = 0;
    fu8_8 y = 0;

    // velocity
    fs8_8 vel_x = 0;
    fs8_8 vel_y = 0;

    Entity_States cur_state = Entity_States::UNUSED;
    Entity_Types type = ENTITY_TYPE_NONE;

    uint8_t anim_counter = 0;
    uint8_t anim_frame = 0;

    bool facing_left = false;
};

enum Game_States
{
    STATE_TITLE = 0,
    STATE_TUTORIAL,
    STATE_GAMEPLAY,
    STATE_GAMEOVER,
};

extern Game_States cur_state;

#endif /* B78B5263_80F5_427B_82AB_4E62FB56CCA0 */
