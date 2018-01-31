uint8_t isBlocking (double pos_X, double pos_Y, float vel_X, float vel_Y) //2-prong collision detection with Sprites
{
    pos_X -= vel_X/5;
    pos_Y -= vel_Y/5;
    uint16_t x1, y1, x2, y2;
    {
        float fx1, fy1, fx2, fy2;
        float ang = vecToAng(vel_X, vel_Y);
        //Create two 'prongs' at a 45 degree angle to velocity
        angToVec(ang - 85, fx1, fy1);
        fx1 = pos_X + fx1/5;
        fy1 = pos_Y + fy1/5;
        angToVec(ang + 85, fx2, fy2);
        fx2 = pos_X + fx2/5;
        fy2 = pos_Y + fy2/5;
        x1 = fx1;
        y1 = fy1;
        x2 = fx2;
        y2 = fy2;
    }
    //Check if the end of those prongs are solid
    uint8_t check_sprite = getSprite(x1, y1);
    if (isSolid(check_sprite)) {
        return check_sprite;
    }
    if (!(x2 == x1 && y2 == y1)) {
        check_sprite = getSprite(x1, y1);
        if (isSolid(check_sprite)) {
            return check_sprite;
        }
    }
    return 0;
}


uint8_t isBlocking (double pos_X, double pos_Y, float vel_X, float vel_Y) //3-prong collision detection with Sprites
{
    uint8_t check_sprite = getSprite(pos_X + vel_X, pos_Y + vel_Y);
    if (isSolid(check_sprite)) { return check_sprite; }
    pos_X -= vel_X/5;
    pos_Y -= vel_Y/5;
    uint16_t x1, y1, x2, y2;
    {
        float fx1, fy1, fx2, fy2;
        float ang = vecToAng(vel_X, vel_Y);
        //Create two 'prongs' at a 45 degree angle to velocity
        angToVec(ang - 90, fx1, fy1);
        fx1 = pos_X + fx1/5;
        fy1 = pos_Y + fy1/5;
        angToVec(ang + 90, fx2, fy2);
        fx2 = pos_X + fx2/5;
        fy2 = pos_Y + fy2/5;
        x1 = fx1;
        y1 = fy1;
        x2 = fx2;
        y2 = fy2;
    }
    //Check if the end of those prongs are solid
    check_sprite = getSprite(x1, y1);
    if (isSolid(check_sprite)) {
        return check_sprite;
    }
    if (!(x2 == x1 && y2 == y1)) {
        check_sprite = getSprite(x1, y1);
        if (isSolid(check_sprite)) {
            return check_sprite;
        }
    }
    return 0;
}


uint8_t isBlocking (double pos_X, double pos_Y, float vel_X, float vel_Y) //Diag test
{
    uint8_t check_sprite = getSprite(pos_X + vel_X, pos_Y + vel_Y);
    if (isSolid(check_sprite)) { return check_sprite; }
    //Check if passing diagonally between block corners
    float ang = vecToAng(vel_X, vel_Y);
    if (uint16_t(ang + 23) / 45 % 2) {
      //Select two sprites between the diagonals
        int x = vel_X+.5, y = vel_Y+.5;
        uint16_t x1 = pos_X, y1 = pos_Y, x2 = pos_X, y2 = pos_Y;
        if (x == 1) {
            if (y == -1) { y1 -= 1; x2 += 1; }
            else         { x1 += 1; y2 += 1; }
        } else {
            if (y ==  1) { x1 -= 1; y2 += 1; }
            else         { x1 -= 1; y2 -= 1; }
        }
        if (isSolid(getSprite(x1, y1)) || isSolid(getSprite(x2, y2))) { return true; }
    }
    return false;
}


uint8_t isBlocking (double pos_X, double pos_Y, float vel_X, float vel_Y) //Flank-mid-flank approach collision detection (unfinished)
{
    uint8_t check_sprite;
  //Middle
    check_sprite = getSprite(pos_X + vel_X, pos_Y + vel_Y);
    if (isSolid(check_sprite)) { return check_sprite; }
  //Flanks
    float ang;
    double flank_X, flank_Y;
    //Left flank
    ang = vecToAng(vel_X, vel_Y) - 90;
    check_sprite = getSprite(pos_X, pos_Y);
    //Right flank
    return 0;
}


//Returns blocking sprite
uint8_t isBlocking (double &pos_X, double &pos_Y, float vel_X, float vel_Y) //2-side collision detection approach
{
    uint16_t new_X = pos_X + new_X, new_Y = pos_Y + new_Y;
    uint8_t check_sprite;
  //Middle
    check_sprite = getSprite(pos_X + vel_X, pos_Y + vel_Y);
    if (isSolid(check_sprite)) { return check_sprite; }
  //Flanks
    float ang;
    double flank_X, flank_Y;
    //Left flank
    ang = vecToAng(vel_X, vel_Y) - 90;
    check_sprite = getSprite(pos_X, pos_Y);
    //Right flank
    return 0;
}

byte hitX = worldMap[uint(nX)][uint(posY)];
byte hitY = worldMap[uint(posX)][uint(nY)];
if ((!hitX || (!spriteWallHit && hitX > INVISIWALLS)) && hitX != M_RANDOM)
{
    posX = nX;
} else { ret = 1; }
if ((!hitY || (!spriteWallHit && hitY > INVISIWALLS)) && hitY != M_RANDOM)
{
    posY = nY;
} else { ret = 2; }
