#include "sound.hpp"

#define E_VILLAGER 0
#define E_ZOMBIE   1

class Entity;
class Projectile;

Entity* prot; //Protagonist

const uint8_t ENTITY_W = 32, ENTITY_H = 64;
const uint16_t GEN_VILLAGERS = 1024;
const uint16_t GEN_ZOMBIES = 64;
const float ANI_INTERVAL = 2;
const uint8_t SHOOT_DISTANCE = 8;
const uint8_t ATTACK_DISTANCE = 6;
const uint8_t LASHOUT_INTERVAL = 20;
const uint8_t MAX_HEALTH = 255;
const float NORMAL_SPEED = .02, ATTACK_SPEED = .06;
const uint8_t PROJECTILE_DAMAGE = 12;
const uint8_t ATTACK_DAMAGE = 45;
const float PROJECTILE_SPEED = .25;
const float LUX_HEAL = .2;
const uint8_t SOUND_INTERVAL = 40;

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

    double pos_X = 0, pos_Y = 0; // Entity position
    uint16_t targ_X = 0, targ_Y = 0; //Target position
    float rot = 0; // As degrees
    uint8_t frame = 0;
    bool had_moved = true;

    uint64_t targetted_at = 0; //Last time this Entity was targetted
    uint64_t prev_hurt = 0; //Last time this entity was hurt

    float health_score = MAX_HEALTH, speed = NORMAL_SPEED, power_score = ATTACK_DAMAGE;
    uint16_t kill_count = 0;

    Entity (uint8_t, double, double);
    Entity (); //For dummy entity

    void think (bool);
    void moveTowards (uint16_t, uint16_t);
    bool tryDir (float, float);
    void move ();
    void harm (Entity*, uint8_t);
    void heal (float);
    void animate ();
    void shoot(Entity*);
    void shootDir();

  private:
      void loiter ();
      void attack (Entity*);
      void lashOut ();
      void step (StepDir, double, double, float);
      Entity* target = NULL;
      uint8_t attack_timeout = 0;
      uint64_t last_lashout = 0;
      float animate_clock = 0;
      float sound_pitch;
};

class Projectile {
  public:
    float vel_X, vel_Y;
    double pos_X, pos_Y;
    bool had_hit = false;
    bool was_successful = false;
    float opacity = 1;
    Entity* shooter;

    void move();
    Projectile(double, double, float, Entity*);
    ~Projectile();

};

Entity::Entity (uint8_t type, double pos_X, double pos_Y)
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
  //Inc kill_count
    attacker->kill_count += is_fatal;
}

void Entity::heal (float amount)
{
    if (health_score + amount < MAX_HEALTH) {
        health_score += amount;
    } else {
        health_score = MAX_HEALTH;
    }
}

void Entity::loiter ()
{
    moveTowards(pos_X + ri(-3, 3), pos_Y + ri(-3, 3));
    attack_timeout = 0;
}

void Entity::step (StepDir direction, double x, double y, float dist)
{
    double step_X, step_Y;
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

uint16_t findEntity (uint8_t type, uint16_t mid_X, uint16_t mid_Y, uint8_t radius)
{
    for (uint16_t y = mid_Y - radius, ylen = mid_Y + radius; y < ylen; ++y) {
        for (uint16_t x = mid_X - radius, xlen = mid_X + radius; x < xlen; x += 2) { //+=2 to dirty remove bias
            if (x == mid_X || y == mid_Y) { continue; }
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
            if(rb(.5)) {
                uint16_t targ_id = findEntity(E_ZOMBIE, pos_X, pos_Y, SHOOT_DISTANCE / (is_nighttime+1));
                if (targ_id) {
                    step(dir_toward, entity[targ_id]->pos_X, entity[targ_id]->pos_Y, .01); //Face the belligerent
                    shoot(entity[targ_id]);
                    return;
                }
            }
          //Loiter
            if (rb(.4)) { loiter(); }
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
                uint16_t targ_id = findEntity(E_VILLAGER, pos_X, pos_Y, ATTACK_DISTANCE * (is_nighttime+1));
                if (targ_id) {
                    if (entity[targ_id]->targetted_at + 50 < game_time) {
                        attack(entity[targ_id]);
                        return;
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
    double d_X = dir_X * dist * speed;
    double d_Y = dir_Y * dist * speed;
    double new_X = pos_X + d_X, new_Y = pos_Y + d_Y;
    double check_X = new_X + dir_X, check_Y = new_Y + dir_Y;
    uint8_t check_sprite = getSprite(check_X, check_Y);
    if (!isSolid(check_sprite) && getBiome(check_X, check_Y) != B_WATER && !(type == E_VILLAGER && check_sprite == S_CAMPFIRE)) {
        pos_X = new_X;
        pos_Y = new_Y;
        if (check_sprite == S_CAMPFIRE) {
            harm(entity[0], 10);
        }
        return true;
    } else {
      //Try pushing outwards
        if (type == E_VILLAGER) { pushCrate(check_X, check_Y, d_X, d_Y); }
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
    double dir_X, dir_Y;
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





Projectile::Projectile (double pos_X, double pos_Y, float rot, Entity* shooter)
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
