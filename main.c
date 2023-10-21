#include "vic20_defines.h"

//#define HAS_MESSAGES
#define HAS_PAGES
#define HAS_PETPAGES
#define HAS_GUI
#define HAS_ENEMYAI

#define USE_OVERLAYS
#define HAS_MUSIC
//#define USE_FULL_OVERLAYS
//#define HAS_LIGHTING

#if defined(OSCAR64)

#pragma stacksize( 64 )
#pragma heapsize( 4 )

#pragma region( stack, 0x0100, 0x01f0, , , {stack} )
#pragma region( bss, 0x033c, 0x03a7, , , {bss} )

//#pragma region( data, 0x03a8, 0x03fe, , , {data} )
#pragma region( data, 0x1de2, 0x1dff, , , {data} )


#if !defined(USE_OVERLAYS)

#pragma region( main, 0x1080, 0x1cff, , , {code   } )

#else

#if defined(USE_FULL_OVERLAYS)
#define BASEMEMEND 0x17ff
#else
#define BASEMEMEND 0x198f
#endif

#pragma region( main, 0x1080, BASEMEMEND, , , {code   } )

// 0x02a8, 0x02ff
#if defined(USE_FULL_OVERLAYS)
#pragma overlay( ovl1, 1 )
#pragma section( bcode1, 0 )
#pragma section( bdata1, 0 )
#pragma region(bank1, BASEMEMEND+1, 0x1cff, ,1, { bcode1, bdata1 } )
#endif

#pragma overlay( genr, 2 )
#pragma section( bcode2, 0 )
#pragma section( bdata2, 0 )
#pragma region(bank2,  BASEMEMEND+1, 0x1cff, ,2, { bcode2, bdata2 } )


#pragma overlay( page, 3 )
#pragma section( bcode3, 0 )
#pragma section( bdata3, 0 )
#pragma region(bank3,  BASEMEMEND+1, 0x1cff, ,3, { bcode3, bdata3 } )


#pragma overlay( gui, 4 )
#pragma section( bcode4, 0 )
#pragma section( bdata4, 0 )
#pragma region(bank4,  BASEMEMEND+1, 0x1cff, ,4, { bcode4, bdata4 } )

#endif

#endif


#if defined(USE_OVERLAYS)
#if defined(WIN32)
void load(const char * fname)
{
}
#else
#include <c64/kernalio.h>
void load(const char * fname)
{
	krnio_setnam(fname);
	krnio_load(1, 8, 1);
}
#endif
#endif

#if defined(RAND_1)
u8 rnd_x=7, rnd_y=3, rnd_z=31, rnd_a=62;
void rand(void)
{
 u8 t = rnd_x ^ (rnd_x << 4);
	rnd_x=rnd_y;
	rnd_y=rnd_z;
	rnd_z=rnd_a;
	rnd_a = rnd_z ^ t ^ ( rnd_z >> 1) ^ (t << 1);
}

#else

u16 seed = 31232;
u8  rnd_a;
void rand(void)
{
    seed ^= seed << 7;
    seed ^= seed >> 9;
    seed ^= seed << 8;
 rnd_a=seed&0xFF;
	//return seed;
}
void srand(u16 s)
{
	seed = s;
}
#endif
// -------------------------------------------------------------

#define BASE_VIDEO_W 22
#define VIDEO_W      18
#define VIDEO_H      23

// -------------------------------------------------------------

#define PAGE_TITLESCREEN   1
#define PAGE_ENTERINGLEVEL 4
#define PAGE_GAMEOVER      3

#define POSFIXEDPOINT 3

#define FLOOR_EMPTY  32

#define WCOL_BASE    (GUI_NUMBER+10)
#define WALL_BRICK   (WCOL_BASE+0)
#define WALL_HARD    (WCOL_BASE+1)
#define WALL_DOOR    (WCOL_BASE+2)
#define WALL_STAIRD  (WCOL_BASE+3)
#define ITEM_RING    (WCOL_BASE+4)
#define ITEM_GOLD    (WCOL_BASE+5)
#define ITEM_POTION  (WCOL_BASE+6)
#define ITEM_KEY     (WCOL_BASE+7)
#define ITEM_SHIELD  (WCOL_BASE+8)
#define ITEM_SWORD   (WCOL_BASE+9)

#define GUI_ALPHA       1
#define GUI_NUMBER     33

#define GUI_HEALTH     (WCOL_BASE+10)
#define GUI_DEFENCE    (WCOL_BASE+8)
#define GUI_ATTACK     (WCOL_BASE+9)
#define GUI_GOLD       (WCOL_BASE+11)
#define GUI_LEVEL      (WCOL_BASE+12)

#define GUI_VSEPARATOR (WCOL_BASE+13)

#define ITEM_ARROWLR (GUI_ALPHA+('Z'-'A')+1)
#define ITEM_ARROWUD (ITEM_ARROWLR+0)

#define HERO_STAND     (GUI_VSEPARATOR+1)
#define ENEMY1_STAND   (GUI_VSEPARATOR+2)

#define HERO_LEGS      (0x1C00+(HERO_STAND*8+7))
#define ENEMY1_LEGS    (0x1C00+(ENEMY1_STAND*8+7))

#define SCREEN_COLOUR	COLOR_BLACK
#define BORDER_COLOUR COLOR_BLACK

#define SCREENBORDER_VALUE 	(SCREEN_COLOUR <<4) + 0x08 + BORDER_COLOUR

// -------------------------------------------------------------

#define MAX_ACTORS     8
#define HERO           0

#define HERO_KIND      0

#define ENEMY1_KIND    1

#define ARROW_KIND     2

#define MAX_KIND       3

#define move_up    1
#define move_right 2
#define move_down  4
#define move_left  8
#define move_fire 16
#define move_quit 32
#define move_dead       64
#define move_levelcompleted 128

#define efx_hit   8
#define efx_hunt  16

#define MAP_WALL1  1
#define MAP_WALL2  2

// -------------------------------------------------------------

void ovl1_performattack(u8 attacker,u8 attacked);
void ovl1_update(u8 h);
void readd_enemy(u8 h);
void add_char_(u8 h,u8 hp);
void add_char(u8 kind,u8 hp);
u8   getkey();

// -------------------------------------------------------------

typedef struct{
 u8 val_defence,val_attack;
}_charlevels;

typedef struct{
 u8  kind;
 u8  frame;
 u8  x,y; 
 u8  dx;
 u8  health;
 u8  bkch,bkcol;
}_actor;

// -------------------------------------------------------------

#if !defined(WIN32)
//#define USELOW
#endif

#if defined(USELOW)
#pragma section( lowmem, 0)
#pragma region( lowmem, 0x02a1, 0x02ff, , , {lowmem} )
#pragma data(lowmem)
#endif

u8        wcol[]={COLOR_RED,COLOR_RED,
                  COLOR_BLUE,
                  COLOR_CYAN,
                  COLOR_YELLOW,COLOR_YELLOW,
                  COLOR_RED,
                  COLOR_VIOLET,
                  COLOR_BLUE,
                  COLOR_GREEN,
                  COLOR_RED,COLOR_YELLOW,COLOR_VIOLET,COLOR_YELLOW,COLOR_VIOLET,COLOR_GREEN};

u8        frm[]={HERO_STAND,ENEMY1_STAND};
u8        frmc[]={COLOR_VIOLET,COLOR_GREEN};

#if defined(USELOW)
#pragma data(data)
#endif

__striped _actor      actors[MAX_ACTORS];
__striped _charlevels charlevels[MAX_KIND];
u8        actors_count,haskey;
u8        entrX;

u8*       video=ADDR(VIDEO_RAM);
u8*       color=ADDR(COLOR_RAM);
#if defined(WIN32)
u8* orig_video=ADDR(VIDEO_RAM);
u8* orig_color=ADDR(COLOR_RAM);
char      res[]="R00";
#endif

u8        x,y;
u8        r,ch;
u16       score;
u8        level;
u8        timer;
u8        movement;
u16       pos,npos;

#define room_max_size ((0x1fff-0x1e00)-(VIDEO_W*VIDEO_H))

// -------------------------------------------------------------

#if defined(WIN32)
void loaddata(const char*name,u8*mem);
extern int win_joystick;
u8 getkey()
{
 return win_joystick; 
}
#else
//CBM entry for computers other than the PET.

void loadres(u8 kind,u8 which)
{
 u8*res=ADDR(0x02a1);
 res[0]='R';res[1]=kind;res[2]='0'+which;
 __asm {
		lda #$00
		ldx #$08
		ldy #$01
		jsr SETLFS
		lda #$03
		ldx #$a1 //<res
		ldy #$02 //>res
		jsr SETNAM
		lda #$00
		jsr LOAD
  BCS  error 
  rts
error:
  sta VICCOLOR
  rts
 }
}
void wait()
{
 __asm{
 		LDA JIFFYL
		STA $FF			// ; save this jiffy
loop3:	LDA JIFFYL
		SEC
		SBC $FF
		CMP #$80		// ; wait 2+ seconds
		BNE loop3
 }
}
void sync()
{
 __asm{
ll:
       lda $9004
       bne ll
 } 
 }

#endif

void getnposxy()
{
 u8 t=((y>>POSFIXEDPOINT));
 npos=(t<<4)+(t<<1);
 npos+=(x>>POSFIXEDPOINT);
}
void getnpos(u8 h)
{
 x=actors[h].x;y=actors[h].y;
 getnposxy();
}
void hide(u8 h)
{
 getnpos(h);
 video[npos]=actors[h].bkch;
 color[npos]=actors[h].bkcol; 
}
void get_input()
{ 
#if defined(WIN32)
 movement=getkey();
#else
 u8 ch;
 u8 a=PEEK(VIA1DDR);
 POKE(VIA2DDR,0x7F);
 POKE(VIA1DDR,0x00);  
 ch=PEEK(JOY0);
 movement=0;
 if((ch&JOYUP)==0)
  movement|=move_up;
 if((ch&JOYDWN)==0)
  movement|=move_down;
 if((ch&JOYL)==0)
  movement|=move_left;
 if((ch&JOYT)==0)
  movement|=move_fire;
 ch=PEEK(JOY0B);
 if((ch&JOYR)==0)
  movement|=move_right;
 POKE(VIA1DDR,a);   
#endif 
 actors[HERO].dx=movement;
 
}

#if defined(USE_OVERLAYS)
#if defined(USE_FULL_OVERLAYS)
#pragma code ( bcode1 )
#pragma data ( bdata1 )
#endif
#endif

void ovl1_update_legs(u8*ptr,u8 ch)
{
 if(ch==2)
  {
   if((timer&7)==1)
    {
    ch=*ptr;
    if(ch==32) ch=4; else ch=32;
    *ptr=ch;
    }
  }
 else
  *ptr=32|4;
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

void ovl1_move_hero()
{
 ch=1;
 getnpos(HERO);
 pos=npos;
 if(actors[HERO].dx&(move_up|move_down|move_left|move_right))
  {   
   ch=ovl1_getmovedest(HERO);   
   if((ch>=ITEM_RING)&&(ch<=ITEM_SWORD))
    {
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

     ch=video[npos]=FLOOR_EMPTY;     
    }
   else
   if(ch==WALL_DOOR)
    {
     if(haskey) 
      {ch=video[npos]=FLOOR_EMPTY;actors[HERO].dx|=move_levelcompleted;}
    }   
   if((ch==FLOOR_EMPTY)||(pos==npos))
    {ovl1_move(HERO);ch=2;}
   else    
    if(ch==ENEMY1_STAND)
     if(actors[HERO].dx&move_fire)
      {
       u8  e;
       u16 pos=npos;
       for(e=1;e<actors_count;e++)
        {
         getnpos(e);
         if(pos==npos)
          {
           ovl1_performattack(HERO,e);
           break;
          }
        }
      }
  }
 
 ovl1_update_legs(ADDR(HERO_LEGS),ch);
 
 if(actors[HERO].dx&move_fire)
  {
  }
}

void ovl1_performattack(u8 attacker,u8 attacked)
{
 u8 kindA=actors[attacker].kind&0x7,kindD=actors[attacked].kind&0x7;
 u8 a=charlevels[kindA].val_attack;
 u8 d=charlevels[kindD].val_defence;
 if(a>d)
  a-=d;
 else
  a=1;
 actors[attacked].kind|=efx_hit;
 if(actors[attacked].health>a)
  actors[attacked].health-=a;
 else
  {
   actors[attacked].health=0;
   actors[attacked].dx|=move_dead;
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
void ovl1_move_enemy(u8 r)
{
#if defined(HAS_ENEMYAI)
 
 /*if((actors[r].kind&efx_hunt)==0)
  ovl1_view_hero(r);  */

 getnpos(r);
 pos=npos;
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
    ovl1_nextdir(r);    
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
  if(actors[r].kind&efx_hunt)
   {
    ovl1_nextdir(r);
    actors[r].kind-=efx_hunt;
   }
  else
   actors[r].kind|=efx_hunt;
#endif 
}

void ovl1_draw_light()
{
 video=ADDR(VIDEO_RAM);
 color=ADDR(COLOR_RAM);
 for(y=0;y<20;y++)
 {
  for(x=0;x<VIDEO_W;x++)
   {
    u8 xx=(actors[HERO].x>>POSFIXEDPOINT),dx;
    u8 yy=(actors[HERO].y>>POSFIXEDPOINT),dy;
    if(xx>x)
     dx=xx-x;
    else
     dx=x-xx;
    if(yy>y)
     dy=yy-y;
    else
     dy=y-yy;
    if(((dx<8)&&(dy<8))&&(video[x]>=WCOL_BASE))
     color[x]=wcol[video[x]-WCOL_BASE];
    else
     color[x]=COLOR_BLACK;
   }
  color+=VIDEO_W;video+=VIDEO_W;
 }
 video=ADDR(VIDEO_RAM);
 color=ADDR(COLOR_RAM);
}

void ovl1_draw_characters()
{
 for(r=0;r<actors_count;r++)
  {
   u8 kind=actors[r].kind&0x7,efx=actors[r].kind&efx_hit;
   switch(kind)
    {
     case HERO_KIND:
      ovl1_move_hero();
     break;
     case ENEMY1_KIND:
      if(timer&1)
       if(actors[r].dx&move_dead)
        readd_enemy(r);
       else
        ovl1_move_enemy(r);
     break;
    }
   if(actors[r].dx&move_dead)
    ;
   else
    {
     u8 col;
     getnpos(r);
     video[npos]=frm[kind];
     if(efx)
      {
       col=COLOR_WHITE;
       actors[r].kind-=0x8;
      }
     else
      col=frmc[kind];
     color[npos]=col;
    }
  }
#if defined(HAS_ENEMYAI)
 ovl1_update_legs(ADDR(ENEMY1_LEGS),2);
#endif
 timer++;
#if defined(HAS_LIGHTING)
 ovl1_draw_light();
#endif
}

#if defined(USE_OVERLAYS)
#pragma code ( code )
#pragma data ( data )
#endif

void readd_enemy(u8 h)
{
 hide(h);
 actors[h].dx=0;
 x=entrX;y=0;
 add_char_(h,4+level);
 actors[h].bkch=WALL_STAIRD;
 actors[h].bkcol=wcol[WALL_STAIRD-WCOL_BASE];
}

void add_char_(u8 h,u8 hp)
{
 
 actors[h].dx=0;actors[h].frame=move_left;
 
 actors[h].x=x<<POSFIXEDPOINT;
 actors[h].y=y<<POSFIXEDPOINT;
 if(hp) actors[h].health=hp; 
 
 actors[h].bkch=FLOOR_EMPTY;//video[2+VIDEO_W*2];
 actors[h].bkcol=COLOR_BLACK;//color[2+VIDEO_W*2];
}
void screenptr_reset()
{
 video=ADDR(VIDEO_RAM);
 color=ADDR(COLOR_RAM);
}
void draw_colors_(u8 x)
{
 while(x--)
  if(video[x]>=WCOL_BASE)
   color[x]=wcol[video[x]-WCOL_BASE];
}
void draw_colors()
{
 screenptr_reset();
#if defined(HAS_LIGHTING)
 video+=20*VIDEO_W;color+=20*VIDEO_W;
 draw_colors_(VIDEO_H*2);
#else
 draw_colors_(255);
 video+=255;color+=255;
 draw_colors_(VIDEO_H*VIDEO_W-255);
#endif
 screenptr_reset();
}

void add_char(u8 kind,u8 hp)
{
 actors[actors_count].kind=kind;
 add_char_(actors_count,hp);
 actors_count++;
}

#if defined(USE_OVERLAYS)
#pragma code ( bcode2 )
#pragma data ( bdata2 )
#endif

void genr_generate_room()
{
 rand();
 r=rnd_a&7; 
#if defined(WIN32)
 res[1]='1';res[2]='1'+r;
 loaddata(res,ADDR(0)); 
 res[1]='0';
#else 
 loadres('1',1+r);
#endif 
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
void genr_draw_roominterior()
{ 
 u8 e;
 u8*room=ADDR(0x1e00+VIDEO_W*VIDEO_H);
 video=ADDR(VIDEO_RAM+VIDEO_W*2+1); 
 r=0;
 for(y=0;y<8;y++)
  {
   x=0;
   while(x<8*2)
    {
     u8 ch=room[r++],b=0;
     if(ch==MAP_WALL1)
      b=WALL_HARD;
     else
      if(ch==MAP_WALL2)
       b=WALL_BRICK;      
     if(b)
      {
       ch=2;
       while(ch--)
       {video[x]=WALL_HARD;video[x+VIDEO_W]=b;x++;}
      }
     else
      x+=2;
    }
   video+=VIDEO_W*2;
  }  
 video=ADDR(VIDEO_RAM);
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
  
 genr_draw_border();
 genr_generate_room(); 
 genr_draw_roominterior();

}

#if defined(USE_OVERLAYS)
#pragma code ( code )
#pragma data ( data )
#endif

#if defined(HAS_GUI)
void set_num(u8 val)
{
 *video=ch;
 while(val>9)
 {
  val-=10;  
  *video=*video+1;
 } 
 video++;
 *video=(ch+val); 
 video++;
}

#if defined(USE_OVERLAYS)
#pragma code ( bcode4 )
#pragma data ( bdata4 )
#endif

void enter_gui()
{
 video=ADDR(VIDEO_RAM+20*VIDEO_W);
 color=ADDR(COLOR_RAM+20*VIDEO_W);
}
void update_gui()
{
 enter_gui();
 ch=GUI_NUMBER;
 video+=VIDEO_W+1;
 set_num(actors[HERO].health);
 video++;
 set_num(charlevels[HERO_KIND].val_defence);
 video++;
 set_num(charlevels[HERO_KIND].val_attack);
 video+=2;
 set_num(((u8*)&score)[1]);
 set_num(((u8*)&score)[0]); 
 
 if(haskey)
  video[1]=ITEM_KEY;
 else
  video[1]=' ';
 
 screenptr_reset();
}

void draw_gui()
{
 enter_gui();

 memset(video,GUI_VSEPARATOR,VIDEO_W);
 memset(color+VIDEO_W,COLOR_WHITE,VIDEO_W);
 
 video[VIDEO_W]=GUI_HEALTH;
 
 video[3+VIDEO_W]=GUI_DEFENCE;
 
 video[6+VIDEO_W]=GUI_ATTACK;
 
 video[10+VIDEO_W]=GUI_GOLD;

 video[16+VIDEO_W]=ITEM_KEY;
 //video[15+VIDEO_W]=GUI_LEVEL;
 
 screenptr_reset();
}
#if defined(USE_OVERLAYS)
#pragma code ( code )
#pragma data ( data )
#endif

#else
void update_gui() {}
void draw_gui() {}
#endif

void game_updatelevels()
{
 charlevels[ENEMY1_KIND].val_attack=5+level;
 charlevels[ENEMY1_KIND].val_defence=5+level; 
}
void game_reset()
{
 level=1;
 charlevels[HERO_KIND].val_attack=charlevels[HERO_KIND].val_defence=5; 
 score=0; 
 actors[0].health=8;
 game_updatelevels();
}

void game_init()
{
 POKE(VICCOLOR,SCREENBORDER_VALUE);

 // load small custom charset
#if defined(WIN32)
 res[1]='0';res[2]='2';
 loaddata(res,ADDR(0)); 
#else 
 loadres('0',2);
#endif
 
 POKE(VICCOLNC,VIDEO_W | 0x80);
#if defined(WIN32)
 POKE(VICSCRHO,PEEK(VICSCRHO)+(BASE_VIDEO_W-VIDEO_W)*8);
#else
 POKE(VICSCRHO,PEEK(VICSCRHO)+(BASE_VIDEO_W-VIDEO_W));
#endif
 

}

void music_play();
void music_stop();

void wait_fire()
{  
#if defined(WIN32)
 while(1)
  {
   get_input();
   timer++;
   if((timer&0xf)<7)
    ch=COLOR_WHITE;
   else
    ch=COLOR_BLACK;
    memset(ADDR(COLOR_RAM+19*VIDEO_W+4),ch,VIDEO_W-8);
   REFRESH
   if(actors[HERO].dx&move_fire)
    break;
   #if defined(HAS_MUSIC)
   music_play();
   #endif
  } 
#else
 u8 portaconfig;
 __asm{
            lda PORTAVIA1d
            sta portaconfig
            lda #0          //; Many thanks to @BedfordLvlExp for the joystick
            sta PORTAVIA1d  //; code here!
loopchar:   
#if defined(HAS_MUSIC)
            jsr music_play
#endif
 jsr sync
 lda timer
 cld
 adc #$01
 sta timer 
 and #$1f
 cmp #$07
 bmi white
 lda #COLOR_WHITE
 jmp go
white:
 lda #COLOR_BLACK
go:
 sta $0f 

 lda #$5a
 sta $0d
 lda #$97
 sta $0e

 lda #$00
 sta $12
 lda #$0a
 sta $11

 jsr memset

            lda PORTAVIA1   //; Load joystick state
            and #32         //; Check for the fire button
            cmp #32
            bne doload
            jsr GETIN       //; Main loop waiting for keyboard events
            beq loopchar
doload:     lda portaconfig //; Put back configuration for port A of VIA 1
            sta PORTAVIA1d  //; to avoid loading issues
 }
#endif
 #if defined(HAS_MUSIC)
 music_stop();
 #endif
}


#if defined(USE_OVERLAYS)
#pragma code ( bcode3 )
#pragma data ( bdata3 )
#endif

#if defined(HAS_MUSIC)
u8 notes[]={C1,0,C1,A1,C1,0,C1,A1,C1,0,C1,A1,F1,G1,A1,F1,0};
u8 n=0,nt=16;

void music_play()
{
 u8 note=notes[n];   
 nt--;
 if(nt==0)
 {
  POKE(VOLUME,0xF);
  POKE(VOICE2,note);
  nt=16;
  n++;
  if(n==sizeof(notes))
   n=0;
 }
}
void music_stop()
{
 POKE(VOLUME,0x0);
}
#endif

void page_unpack(u8*ptr)
{ 
 u8*vd=ADDR(VIDEO_RAM);
 u8*cl=ADDR(COLOR_RAM); 
 while(*ptr)
 {
  y=ptr[1]&0x7;
  r=ptr[1]>>3; 
  vd+=ptr[0];
  cl+=ptr[0];
  ptr+=2;x=0;
  while(x<r)
   {
    vd[x]=ptr[x];
    cl[x]=y;
    x++;
   }
  ptr+=x;
 }
}

void page_draw_page(u8 r)
{
 #if defined(HAS_PAGES)
 u8 pg=r;

 memset(ADDR(COLOR_RAM),0,VIDEO_W*VIDEO_H); 
 
#if defined(WIN32)
 res[2]='0'+r;
 loaddata(res,ADDR(0)); 
#else 
 loadres('0',r);
#endif
 #endif

 #if defined(HAS_PAGES)
 #if defined(HAS_PETPAGES)
 POKE(VICCHGEN,(PEEK(VICCHGEN)&0xF0)|0x0);
 page_unpack(ADDR(0x1b00));
 #else
 POKE(VICCHGEN,(PEEK(VICCHGEN)&0xF0)|0xF);
 unpack(ADDR(0x1C00+24*8));
 #endif
 #endif

 ch='0'; 
 if(pg==PAGE_ENTERINGLEVEL)
  {
   video=ADDR(VIDEO_RAM+18*VIDEO_W)+11;
   set_num(level);  
   wait();
  }
 else
  {
   if(pg==PAGE_GAMEOVER)
    {
     video=ADDR(VIDEO_RAM+16*VIDEO_W)+5;set_num(level);
     video+=2;set_num(((u8*)&score)[1]);set_num(((u8*)&score)[0]);
    }    
    wait_fire();
  }
}

#if defined(USE_OVERLAYS)
#pragma code ( code )
#pragma data ( data )
#endif

#if defined(HAS_MESSAGES) 
void draw_message(u8 r)
{ 
#if defined(WIN32)
 res[2]='0'+r;
 loaddata(res,ADDR(0)); 
#else 
 loadres('0',r);
#endif
 unpack(ADDR(0x1F9e)); 
 wait(); 
}
#endif

#if defined(WIN32)
int VIC20_main()
#else
int main()
#endif
{ 
#if defined(USE_OVERLAYS)
 load("PAGE");
#endif

 game_init();
  
 // main game lopp
 while(1)
  {
   // home
   page_draw_page(PAGE_TITLESCREEN);

   // start game
   game_reset();

   while(1)
    {
     page_draw_page(PAGE_ENTERINGLEVEL);

// ---------------------------
#if defined(USE_OVERLAYS)
     load("GENR");
#endif
     genr_draw_room();
// ---------------------------
#if defined(USE_OVERLAYS)
     load("GUI");
#endif
     draw_gui();   
     draw_colors();

// ---------------------------
#if defined(USE_OVERLAYS)
#if defined(USE_FULL_OVERLAYS)
     load("OVL1");
#endif
#endif
     while(1)
      {
       get_input();
       REFRESH
       ovl1_draw_characters();  
       update_gui();       
   #if defined(WIN32)
       if(actors[HERO].dx&move_quit)
        return 1;
   #endif
       if(actors[HERO].dx&(move_dead|move_levelcompleted))
        break;

   #if !defined(WIN32)
       __asm{
        LDA JIFFYL
loop1:	CMP JIFFYL
		       BEQ loop1
       }
   #endif
      }

#if defined(USE_OVERLAYS)
     load("PAGE");
#endif
     if(actors[HERO].dx&move_dead)
      {
       page_draw_page(PAGE_GAMEOVER);
       break;
      }
     else
      level++;
    }
  }
 
 return 0;
}