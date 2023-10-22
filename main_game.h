#if defined(USE_OVERLAYS)
#if defined(USE_FULL_OVERLAYS)
#pragma code ( bcodegame )
#pragma data ( bdatagame )
#endif
#endif
void game_draw_light();
#if defined(HAS_MUSIC)
u8 notetimer;
void sound_play(u8 note)
{
 POKE(EFFECTS,note);
 notetimer=5;
}
void sound_stop()
{
 POKE(EFFECTS,0);
}
#endif

void game_draw_characters()
{
 for(r=0;r<actors_count;r++)
  {
   u8 kind=actors[r].kind&0x7;
   if(actors[r].kind&efx_hit)
    {
     if(actors[r].tm)
      actors[r].tm--;
     else
      actors[r].kind-=efx_hit;
    }
   else
    switch(kind)
     {
      case HERO_KIND:
       game_move_hero();
      break;
      case ENEMY1_KIND:       
       if(actors[r].dx&move_dead)
        {
         if(actors[r].tm)
          actors[r].tm--;
         else
          readd_enemy(r);
        }
       else
        if(timer&1)
         game_move_enemy(r);
      break;
     }
   if(actors[r].dx&move_dead)
    ;
   else
    {     
     u8 efx=actors[r].kind&(efx_hit|efx_attack);
     u8 col=frmc[kind];     

     getnpos(r);
     video[npos]=frm[kind];  

     if(efx)
      {
       if(efx&efx_hit)
        {
         col=COLOR_WHITE;
         actors[r].kind-=efx_hit;
        }
       /*if(efx&efx_attack)
        {
         col=COLOR_YELLOW;
         actors[r].kind-=efx_attack;
        }*/
      }
     color[npos]=col;
    }
  }
 
#if defined(HAS_ENEMYAI)
 if((timer&15)==1)
  gui_update_legs_(ADDR(ENEMY1_LEGS));    
 //ovl1_update_legs(ADDR(ENEMY1_LEGS),2);
#endif
timer++;
if(notetimer)
 {
  notetimer--;
  if(notetimer==0)
   sound_stop();
 }
#if defined(HAS_LIGHTING)
 game_draw_light();
#endif
}

void ovl1_moveupdate(u8 movement)
{
 if(movement&move_left)
  x--;
 else
  if(movement&move_right)
   x++;
 if(movement&move_up)
  y--;
 else
  if(movement&move_down)
   y++;
}
u8 ovl1_getmovedest(u8 h)
{
 x=actors[h].x;y=actors[h].y;
 ovl1_moveupdate(actors[h].dx);//x+=dx;y+=dy;
 getnposxy();
 return video[npos];
}
void ovl1_update(u8 h)
{
 getnpos(h);
 actors[h].bkch=video[npos];
 actors[h].bkcol=color[npos];
}
void ovl1_move(u8 h)
{
 hide(h);

 ovl1_moveupdate(actors[h].dx); 
 actors[h].x=x;actors[h].y=y;

 ovl1_update(h);
}
void clean()
{
 ch=video[npos]=FLOOR_EMPTY; 
}
void game_move_hero()
{
 ch=1;
 getnpos(HERO);
 pos=npos;
 if(actors[HERO].dx&(move_up|move_down|move_left|move_right))
  {   
   ch=ovl1_getmovedest(HERO);   
   if((ch>=ITEM_RING)&&(ch<=ITEM_SWORD))
    {
     sound_play(C2);
     if(ch==ITEM_GOLD)
      score+=5;
     else
     if(ch==ITEM_RING)
      score++;
     else
     if(ch==ITEM_KEY)
      haskey=1;
     else
      {
       u8*ptr=0;
       if(ch==ITEM_POTION)
        ptr=(u8*)&actors[HERO].health;
       else
        if(ch==ITEM_SHIELD)
         ptr=(u8*)&charlevels[HERO_KIND].val_defence;
        else
         ptr=(u8*)&charlevels[HERO_KIND].val_attack;         
       if(ptr&&(*ptr<64))
        *ptr+=2;
      }     

     clean();    
    }
   else
   if(ch==WALL_DOOR)
    {
     if(haskey) 
      {clean();actors[HERO].dx|=move_levelcompleted;}
    }   
   if((ch==FLOOR_EMPTY)||(pos==npos))
    {ovl1_move(HERO);ch=2;}
   else    
    if(ch==ENEMY1_STAND)
     if(charlevels[HERO_KIND].val_attack)
      if( (actors[HERO].dx&move_fire) )// && ((actors[HERO].ldx&move_fire)==0) )
       if(actors[HERO].ldx==0)
        {
         u8  e;
         u16 pos=npos;
         for(e=1;e<actors_count;e++)
          {
           getnpos(e);
           if(pos==npos)
            {            
             ovl1_performattack(HERO,e);
             actors[HERO].ldx=1;
             break;
            }
          }
        }
   gui_update_sword(ADDR(HERO_SWORD),actors[HERO].dx);
  }
 
 gui_update_legs(ADDR(HERO_LEGS),ch); 
 
 if(actors[HERO].dx&move_fire)
  ;
 else
  actors[HERO].ldx=0;
  
}

void ovl1_performattack(u8 attacker,u8 attacked)
{
 u8 kindA=actors[attacker].kind&0x7,kindD=actors[attacked].kind&0x7;
 u8 a=charlevels[kindA].val_attack;
 u8 d=charlevels[kindD].val_defence;
 
 if(a>d)
  a-=d;
 else
 {
  if((kindD==HERO_KIND)&&(rnd_a&1)) charlevels[kindD].val_defence--;
  a=1;
 }
 //actors[attacker].kind|=efx_attack;
 sound_play(C1);
 actors[attacked].kind|=efx_hit;actors[attacked].tm=50;
 if(actors[attacked].health>a)
  actors[attacked].health-=a;
 else
  {
   if(rnd_a&1) charlevels[kindA].val_attack--;
   if(kindA==HERO_KIND) score+=(rnd_a&3);
   actors[attacked].health=0;
   actors[attacked].dx|=move_dead;
   actors[attacked].tm=200;
   hide(attacked);
  }
}

void ovl1_move_check(u8 r)
{
 if(actors[r].dx)
  {
  ch=ovl1_getmovedest(r);
  if((ch==WALL_HARD)||(ch==WALL_BRICK))
   actors[r].dx=0;
  }
}

void ovl1_view_hero(u8 r)
{
 u8 add;
 getnpos(r);
 pos=npos;
 getnpos(HERO);
 if(npos>pos)
  {
   u16 t=pos;
   pos=npos;npos=t;
  }
 if(pos>npos+VIDEO_W)
  add=VIDEO_W;
 else
  add=1;
 while(pos<npos)
  {
   pos+=add;
   ch=video[pos];
   if((ch==WALL_HARD)||(ch==WALL_BRICK))
    return;
  }
 if(pos==npos)
  actors[r].kind|=efx_hunt;
}
void ovl1_nextdir(u8 r)
{
 actors[r].frame<<=1;
 if(actors[r].frame==16) actors[r].frame=1;
}
u8 ovl1_checkhalfdir(u8 r,u8 dx)
{
 actors[r].dx=dx;
 ovl1_move_check(r);
 if(actors[r].dx==0)
  return 1;
 return 0;
}
void game_move_enemy(u8 r)
{
#if defined(HAS_ENEMYAI)
 
 /*if((actors[r].kind&efx_hunt)==0)
  ovl1_view_hero(r);  */

 getnpos(r);
 pos=npos;
 actors[r].ldx=actors[r].dx;
 actors[r].dx=0;
 ch=0; 

 if(actors[r].kind&efx_hunt)
  {
   u8 lr=0,ud=0,dx1=0,dx2=0;
   if(actors[r].x>actors[HERO].x)
    {dx1=move_left;lr=actors[r].x-actors[HERO].x;}
   else
    if(actors[r].x<actors[HERO].x)
     {dx1=move_right;lr=actors[HERO].x-actors[r].x;}
   
   if(dx1)
    if(ovl1_checkhalfdir(r,dx1))
     dx1=lr=0;         

   if(actors[r].y>actors[HERO].y)
    {dx2=move_up;ud=actors[r].y-actors[HERO].y;}
   else
    if(actors[r].y<actors[HERO].y)
    {dx2=move_down;ud=actors[HERO].y-actors[r].y;}

   if(dx2)
    if(ovl1_checkhalfdir(r,dx2))
     dx2=ud=0;         

   if(ud>lr)
    actors[r].dx=dx2;
   else
    actors[r].dx=dx1;
  }
 else
  {
   rand();
   if((rnd_a&0x3f)==0)
    actors[r].kind|=efx_hunt;
   else
    actors[r].dx=actors[r].frame;  
  } 

 ovl1_move_check(r);
 
 if(actors[r].dx)
  {   
   if(ch==HERO_STAND)
    {
     if((timer&7)==1)
      ovl1_performattack(r,HERO);
    }
   else
   if((ch==FLOOR_EMPTY)||((ch>=ITEM_RING)&&(ch<=ITEM_SWORD))||(pos==npos))
    ovl1_move(r);   
  }
 else
  {
   ovl1_nextdir(r);
   if(actors[r].kind&efx_hunt)
    actors[r].kind-=efx_hunt;
   else
    {
     if(actors[r].frame==1)
      actors[r].kind|=efx_hunt;
    }
  }
#endif 
}

void game_draw_light()
{
 getnpos(HERO);
 npos-=(VIDEO_W*2+1);
 for(y=0;y<5;y++)
 {
  for(x=0;x<3;x++)
   if((video[npos+x]>=WCOL_BASE))
    color[npos+x]=wcol[video[npos+x]-WCOL_BASE];
  npos+=VIDEO_W;
 }
}


#if defined(USE_OVERLAYS)
#if defined(USE_FULL_OVERLAYS)
#pragma code ( code )
#pragma data ( data )
#endif
#endif