#include "sound.hpp"
#include "map.hpp"

#define E_VILLAGER 0
#define E_ZOMBIE   1

class Entity;
class Projectile;

Entity* prot; //Protagonist

const uint8_t ENTITY_W = 32, ENTITY_H = 64;
const uint16_t GEN_VILLAGERS = 1024;
const uint16_t GEN_ZOMBIES = 256;
const float ANI_INTERVAL = 2;
const uint8_t SHOOT_DISTANCE = 12;
const uint8_t ATTACK_DISTANCE = 6;
const uint8_t LASHOUT_INTERVAL = 20;
const uint8_t MAX_HEALTH = 255;
const float NORMAL_SPEED = .032, ATTACK_SPEED = .08;
const uint8_t PROJECTILE_DAMAGE = 10;
const uint8_t ATTACK_DAMAGE = 45;
const float PROJECTILE_SPEED = .32;
const float LUX_HEAL = .2;
const uint8_t SOUND_INTERVAL = 40;
const uint8_t KILL_HP_REWARD = 20;
const uint8_t CAMPFIRE_DAMAGE = 20;

enum StepDir {
    dir_away,
    dir_toward
};

std::vector<Entity*> entity = std::vector<Entity*>();
std::vector<Projectile*> projectile = std::vector<Projectile*>();

class Entity {
  public:
    uint8_t type = 255; // 0 Villager, 1 Zombie
    uint16_t index_in_array;
    bool is_dead = false;

    float pos_X = 0, pos_Y = 0; // Entity position
    uint16_t targ_X = 0, targ_Y = 0; //Target position
    float rot = 0; // As degrees
    uint8_t frame = 0;
    bool had_moved = true;
    float opacity = 1;

    uint64_t targetted_at = 0; //Last time this Entity was targetted
    uint64_t prev_hurt = 0; //Last time this entity was hurt

    float health_score = MAX_HEALTH;
    float max_health = MAX_HEALTH;
    float speed = NORMAL_SPEED;
    float power_score = ATTACK_DAMAGE;
    uint16_t kill_count = 0;

    Entity (uint8_t, float, float);
    Entity (); //For dummy entity

    void think (bool);
    void moveTowards (uint16_t, uint16_t);
    bool tryDir (float, float);
    void move ();
    void harm (Entity*, uint8_t);
    void heal (float);
    void animate ();
    void shoot (Entity*);
    void shootDir ();

  private:
      void loiter ();
      void attack (Entity*);
      void lashOut ();
      void step (StepDir, float, float, float);
      Entity* target = NULL;
      uint8_t attack_timeout = 0;
      uint64_t last_lashout = 0;
      float animate_clock = 0;
      float sound_pitch;
};

class Projectile {
  public:
    float pos_X, pos_Y;
    float vel_X, vel_Y;
    bool had_hit = false;
    bool was_successful = false;
    float opacity = 1;
    Entity* shooter;

    void move();
    Projectile(float, float, float, Entity*);
    ~Projectile();

};

Entity::Entity (uint8_t type, float pos_X, float pos_Y)
{
    this->index_in_array = entity.size();
    this->type = type;
    this->pos_X = this->targ_X = pos_X;
    this->pos_Y = this->targ_Y = pos_Y;
    this->animate_clock = ri(0, ANI_INTERVAL);
    this->sound_pitch = rf(.75, 1.50);
}

Entity::Entity () { }

void Entity::attack (Entity* who)
{
    target = who;
    target->targetted_at = game_time;
    speed = ATTACK_SPEED;
    attack_timeout = 4;
}

void Entity::lashOut ()
{
    if (last_lashout + LASHOUT_INTERVAL < game_time) {
        last_lashout = game_time;
        if (target->type != E_ZOMBIE) {
            target->harm(this, power_score);
        } else {
            attack_timeout = 0;
        }
    }
}

void Entity::harm (Entity* attacker, uint8_t damage)
{
    bool is_fatal = false;
    int8_t sound_id = -1;
  //Select sound
    if (prev_hurt + SOUND_INTERVAL < game_time) {
        switch (type) {
            case E_VILLAGER: sound_id = AUD_VILLAGER_HURT; break;
            case E_ZOMBIE: sound_id = AUD_ZOMBIE_HURT; break;
        }
    }
    prev_hurt = game_time;
  //Deal damage
    health_score -= damage;
  //Check if dead
    if (health_score < 0) {
        if (type == E_VILLAGER) {
            type = E_ZOMBIE;
            health_score = MAX_HEALTH;
            is_fatal = true;
            kill_count = 0;
        } else if (type == E_ZOMBIE) {
            sound_id = AUD_ZOMBIE_DIE;
            is_dead = true;
            health_score = 0;
            frame = 0;
            speed = NORMAL_SPEED*3; //For the animation
            is_fatal = true;
        }
    }
  //If a zombie, attack the attacker
   if (type == E_ZOMBIE) {
       attack(attacker);
   }
  //Play sound
    if (sound_id > 0) {
        playSound(sound_id, sound_pitch * rf(.90, 1.10), pos_X, pos_Y, prot->pos_X, prot->pos_Y);
    }
  //Inc kill_count and reward
    attacker->kill_count += is_fatal;
    if (attacker->type == E_VILLAGER) { attacker->max_health += is_fatal * KILL_HP_REWARD; }
}

void Entity::heal (float amount)
{
    if (health_score + amount < max_health) {
        health_score += amount;
    } else {
        health_score = max_health;
    }
}


void Entity::loiter ()
{
    moveTowards(pos_X + ri(-3, 3), pos_Y + ri(-3, 3));
    moveTowards(pos_X + ri(-4, 4), pos_Y + ri(-4, 4));
    attack_timeout = 0;
}

void Entity::step (StepDir direction, float x, float y, float dist)
{
    float step_X, step_Y;
    targToVec(pos_X, pos_Y, x, y, step_X, step_Y);
    moveTowards(pos_X + (step_X * dist), pos_Y + (step_Y * dist));
}

bool enemyIsHere (Entity* here, uint16_t expected_x, uint16_t expected_y)
{
    if (here->index_in_array && !here->is_dead) {
        if (uint16_t(here->pos_X) == uint16_t(expected_x) && uint16_t(here->pos_Y) == uint16_t(expected_y)) {
            return true;
        }
    }
    return false;
}

uint16_t findEntity (uint8_t type, uint16_t mid_X, uint16_t mid_Y, uint8_t radius) //Finds entities 25% of the time
{
    const uint16_t x1 = (mid_X - radius) + ri(0, radius);
    const uint16_t y1 = (mid_Y - radius) + ri(0, radius);
    const uint16_t x2 = x1 + radius;
    const uint16_t y2 = y1 + radius;
    for (uint16_t y = y1; y < y2; ++y) {
        for (uint16_t x = x1; x < x2; ++x) {
            if (x == mid_X && y == mid_Y) { continue; }
            uint16_t test = getMapEntity(x, y);
            if (enemyIsHere(entity[test], x, y) && entity[test]->type == type) { return test; }
        }
    }
    return 0;
}

void Entity::think (bool is_nighttime)
{
    switch (type) {
        case 0: //Villager
          //Find zombie to shoot at
            {
                float shoot_dist = SHOOT_DISTANCE / (is_nighttime+1);
                uint16_t targ_id = findEntity(E_ZOMBIE, pos_X, pos_Y, shoot_dist);
                if (targ_id) {
                  //Check target is not blocked
                    if (!raycastBlocking(pos_X, pos_Y, entity[targ_id]->pos_X, entity[targ_id]->pos_Y, shoot_dist)) {
                        step(dir_toward, entity[targ_id]->pos_X, entity[targ_id]->pos_Y, .01); //Face the belligerent
                        shoot(entity[targ_id]);
                        return;
                    }
                }
            }
          //Loiter
            if (rb(.2)) { loiter(); }
            break;
        case 1: //Zombie
            if (attack_timeout) {
                --attack_timeout;
                if (!attack_timeout) { //Stop attacking
                    target = NULL;
                    speed = NORMAL_SPEED;
                }
            } else {
              //Find a Villager to attack
                float attack_dist = ATTACK_DISTANCE * (is_nighttime+1);
                uint16_t targ_id = findEntity(E_VILLAGER, pos_X, pos_Y, attack_dist);
                if (targ_id) {
                  //Check target is not blocked
                    if (!raycastBlocking(pos_X, pos_Y, entity[targ_id]->pos_X, entity[targ_id]->pos_Y, attack_dist)) {
                        if (entity[targ_id]->targetted_at + 50 < game_time) {
                            attack(entity[targ_id]);
                            return;
                        }
                    }
                }
              //Loiter
                loiter();
            }
            break;
    }
}

void Entity::moveTowards (uint16_t x, uint16_t y)
{
    targ_X = x;
    targ_Y = y;
}

bool Entity::tryDir (float dir_X, float dir_Y)
{
    float dist = eD_approx(pos_X, pos_Y, pos_X + dir_X, pos_Y + dir_Y);
    float d_X = dir_X * dist * speed;
    float d_Y = dir_Y * dist * speed;
    float new_X = pos_X + d_X, new_Y = pos_Y + d_Y;
    float check_X = new_X + dir_X, check_Y = new_Y + dir_Y;
    uint8_t check_sprite = getSprite(check_X, check_Y);
    if (!isSolid(check_sprite) && getBiome(check_X, check_Y) != B_WATER && !(type == E_VILLAGER && check_sprite == S_CAMPFIRE)) {
        pos_X = new_X;
        pos_Y = new_Y;
        if (check_sprite == S_CAMPFIRE) {
            harm(entity[0], CAMPFIRE_DAMAGE);
        }
        return true;
    } else {
      //Try pushing outwards
        if (type == E_VILLAGER) { tryPushCrate(check_X, check_Y, d_X, d_Y); }
      //Trigger a loiter
        loiter();
        return false;
    }
}

void Entity::move ()
{
    if (attack_timeout) {
        targ_X = target->pos_X;
        targ_Y = target->pos_Y;
    }
    if (abs(uint16_t(pos_X + .5) - targ_X) || abs(uint16_t(pos_Y + .5) - targ_Y)) { //Need to move?
        rot = vecToAng(targ_X - pos_X, targ_Y - pos_Y);
        float dir_X, dir_Y;
        angToVec(rot, dir_X, dir_Y);
        if (tryDir(dir_X, dir_Y)) {
            had_moved = true;
        } else {
            frame = 0;
            targ_X = pos_X;
            targ_Y = pos_Y;
        }
    } else {
        if (attack_timeout) {
            lashOut();
        }
        frame = 0;
    }
    setMapEntity(uint16_t(pos_X), uint16_t(pos_Y), index_in_array);
}

void Entity::animate ()
{
    animate_clock += speed * 4;
    if (animate_clock > ANI_INTERVAL) {
        animate_clock = 0;
        if (is_dead) { if (frame < 4) { ++frame; } return; }
        if (had_moved) { ++frame; had_moved = false; }
    }
}

void Entity::shoot (Entity* victim)
{
    float dir_X, dir_Y;
    targToVec(pos_X, pos_Y, victim->pos_X, victim->pos_Y, dir_X, dir_Y);
    float dir_ang = vecToAng(dir_X, dir_Y);
    playSound(AUD_SHOOT, rf(.75, 1.25), pos_X, pos_Y, prot->pos_X, prot->pos_Y);
    rot = dir_ang;
    projectile.push_back(new Projectile(pos_X, pos_Y, dir_ang, this));
}

void Entity::shootDir ()
{
    playSound(AUD_SHOOT, rf(.75, 1.25), pos_X, pos_Y, prot->pos_X, prot->pos_Y);
    projectile.push_back(new Projectile(pos_X + .25, pos_Y + .25, rot, this));
}





Projectile::Projectile (float pos_X, float pos_Y, float rot, Entity* shooter)
{
    this->pos_X = pos_X;
    this->pos_Y = pos_Y;
    this->shooter = shooter;
    angToVec(rot, vel_X, vel_Y);
    vel_X *= PROJECTILE_SPEED;
    vel_Y *= PROJECTILE_SPEED;
}

void Projectile::move ()
{
    pos_X += vel_X;
    pos_Y += vel_Y;
    if (isSolid(getSprite(pos_X, pos_Y))) {
        had_hit = true;
    } else {
      //Check if we've shot an Entity at this position
        uint16_t e = getMapEntity(uint16_t(pos_X), uint16_t(pos_Y)); //Entity here
        uint16_t e2 = getMapEntity(uint16_t(pos_X - 1), uint16_t(pos_Y + 1)); //Entity below
        Entity* here = entity[e];
        Entity* below = entity[e2];
        if (enemyIsHere(here, pos_X, pos_Y) && here->type == E_ZOMBIE && here->index_in_array != shooter->index_in_array) {
            had_hit = was_successful = true;
            here->harm(shooter, PROJECTILE_DAMAGE);
        }
        if (!isSolid(getSprite(pos_X - 1, pos_Y + 1))) {
            if (enemyIsHere(below, pos_X - 1, pos_Y + 1) && below->type == E_ZOMBIE && below->index_in_array != shooter->index_in_array) {
                had_hit = was_successful = true;
                below->harm(shooter, PROJECTILE_DAMAGE);
            }
        }
    }
}
