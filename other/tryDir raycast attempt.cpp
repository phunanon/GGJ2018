bool Entity::tryDir (float dir_X, float dir_Y)
{
    float next_X = pos_X + dir_X, next_Y = pos_Y + dir_Y;
    float dist = eD_approx(pos_X, pos_Y, next_X, next_Y);
    float check_X = pos_X + dir_X, check_Y = pos_Y + dir_Y;
    uint8_t check_sprite = raycastBlocking(pos_X, pos_Y, next_X, next_Y, 1, isntWalkable);
    if ( (!check_sprite || (type == E_ZOMBIE && check_sprite == S_CAMPFIRE)) && getBiome(check_X, check_Y) != B_WATER) {
        pos_X += dir_X * dist * speed;
        pos_Y += dir_Y * dist * speed;
        if (check_sprite == S_CAMPFIRE) {
            harm(entity[0], CAMPFIRE_DAMAGE);
        }
        return true;
    } else {
      //Try pushing outwards
        if (type == E_VILLAGER) { tryPushCrate(next_X + dir_X, next_Y + dir_Y, dir_X, dir_Y); setBiome(next_X + (dir_X*2), next_Y + (dir_Y*2), B_SAND); }
      //Trigger a loiter
        loiter();
        return false;
    }
}
