#if defined(USE_OVERLAYS)
#pragma code ( bcodegenr )
#pragma data ( bdatagenr )
#endif

void genr_generate_room()
{
}

void genr_draw_border()
{
 video=ADDR(VIDEO_RAM);
 memset(video,WALL_HARD,VIDEO_W);
 memset(video+VIDEO_W,WALL_BRICK,VIDEO_W);
 memset(video+VIDEO_W*18,WALL_HARD,VIDEO_W);
 memset(video+VIDEO_W*19,WALL_BRICK,VIDEO_W);

 video=ADDR(VIDEO_RAM+VIDEO_W);
 for(y=0;y<17;y++)
  {
   video[0]=video[17]=WALL_HARD;
   video+=VIDEO_W;
  }
}
void genr_getrandomx(u8 y)
{
 while(1)
 {
  rand();x=1+(rnd_a&15);
  npos=(y<<4)+(y<<1)+x;
  //npos=x+ypos[y];
  if(video[npos]==FLOOR_EMPTY)
   break;
 }
}
void genr_getrandomxy()
{
 timer++;
 while(1)
 {
  rand();x=1+(rnd_a&7);
  rand();y=2+(rnd_a&7);
  if(timer&1) x+=8;
  if(timer&2) y+=8;
  //npos=x+ypos[y];
  npos=(y<<4)+(y<<1)+x;
  if(video[npos]==FLOOR_EMPTY)
   break;
 }
}
void adjustwalls(u8*video,u8 x)
{
 while(x--)
  if((video[x]==WALL_HARD)&&(video[x+VIDEO_W]==FLOOR_EMPTY))
   video[x]=WALL_BRICK;
}
void unmask(u8 ch,u8 e)
{
 if(e&2)
  x=1;
 else
  x=128;
 while(x)
  {
   if((ch&x)==0)
    {
     video[0]=video[VIDEO_W]=FLOOR_EMPTY;
     video[1]=video[VIDEO_W+1]=FLOOR_EMPTY;
    }
   video+=2;
   if(e&2)
    x<<=1;
   else
    x>>=1;
  }
}
void genr_draw_roominterior()
{ 
 u8 e;
 u8*room=ADDR(0x1f9e); 
 // ----------------------------
 rand();
 r=rnd_a&7;
 room+=r<<3;
 rand();e=rnd_a&3; 
 // ----------------------------
 memset(ADDR(VIDEO_RAM),WALL_HARD,VIDEO_W*(2+16+1)); 
 // ----------------------------
 video=ADDR(VIDEO_RAM+VIDEO_W*2);  
 if(e&1) room+=7;
 for(y=0;y<8;y++)
  {   
   video++;
   unmask(*room,e);
   video+=1+VIDEO_W;
   if(e&1)
    room--;
   else
    room++;
  }  
 // ----------------------------
 video+=VIDEO_W;
 memset(video,WALL_BRICK,VIDEO_W);
 adjustwalls(ADDR(VIDEO_RAM+VIDEO_W),VIDEO_W*9);
 adjustwalls(ADDR(VIDEO_RAM+VIDEO_W*10),VIDEO_W*9);
 video=ADDR(VIDEO_RAM);
 // ----------------------------
 // add HERO
 genr_getrandomx(2);
 entrX=x;
 video[x]=WALL_STAIRD; 
 video[VIDEO_W+x]=FLOOR_EMPTY; 
 y=1;add_char(HERO_KIND,0);
 // add EXIT
 genr_getrandomx(2+15);
 video[npos+VIDEO_W]=WALL_DOOR;
 video[npos+VIDEO_W*2]=WALL_STAIRD;
 // add ENEMIES
 e=2+(level>>1);
 if(e>6) e=6; 
 r=e;
 while(r--)
  {
   timer=2;
   genr_getrandomxy();   
   add_char(ENEMY1_KIND,4+level);video[npos]=ENEMY1_STAND;
  }
 // add ELEMENTS
 for(r=ITEM_POTION;r<=ITEM_SWORD;r++)
  {
   genr_getrandomxy();
   video[npos]=r;
  }
 r=8-e;
 while(r--)
  {
   genr_getrandomxy();
   video[npos]=ITEM_RING+(rnd_a&1);
  }  
}

void genr_draw_room()
{
 memset(ADDR(COLOR_RAM),0,VIDEO_W*VIDEO_H); 
 memset(ADDR(VIDEO_RAM),32,VIDEO_W*VIDEO_H);

 POKE(VICCHGEN,(PEEK(VICCHGEN)&0xF0)|0xF);

 haskey=actors_count=r=0;
 //memset(&actors[0],0,sizeof(actors));
  
 //genr_draw_border();
 genr_generate_room(); 
 genr_draw_roominterior();

}

#if defined(USE_OVERLAYS)
#pragma code ( code )
#pragma data ( data )
#endif