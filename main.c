#include "vic20_defines.h"

//#define HAS_MESSAGES
#define HAS_PAGES
#define HAS_PETPAGES
#define HAS_GUI
#define HAS_ENEMYAI

#if !defined(WIN32)
#define USE_OVERLAYS
#endif
//#define USE_FULL_OVERLAYS
#define HAS_MUSIC

#define HAS_KEYBOARD
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
#pragma overlay( game, 1 )
#pragma section( bcodegame, 0 )
#pragma section( bdatagame, 0 )
#pragma region(bank1, BASEMEMEND+1, 0x1cff, ,1, { bcodegame, bdatagame } )

#pragma overlay( genr, 2 )
#pragma section( bcodegenr, 0 )
#pragma section( bdatagenr, 0 )
#pragma region(bank2,  BASEMEMEND+1, 0x1cff, ,2, { bcodegenr, bdatagenr } )


#pragma overlay( page, 3 )
#pragma section( bcodepage, 0 )
#pragma section( bdatapage, 0 )
#pragma region(bank3,  BASEMEMEND+1, 0x1cff, ,3, { bcodepage, bdatapage } )


#pragma overlay( gui, 4 )
#pragma section( bcodegui, 0 )
#pragma section( bdatagui, 0 )
#pragma region(bank4,  BASEMEMEND+1, 0x1cff, ,4, { bcodegui, bdatagui } )

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

//#define RAND_2

#if defined(RAND_2)

u8 rnd_a=62;
void rand()
{
 #if defined(WIN32)
#else
 __asm{
  lda rnd_a
  beq doEor
  asl
  beq noEor //;if the input was $80, skip the EOR
  bcc noEor
doEor:    
  eor #$1d
noEor:  
  sta rnd_a
 }
#endif
}
void srand()
{
 #if defined(WIN32)
 rnd_a=62;
 #else
 __asm{
try_again:
        lda $a2          //       ;jiffy as random seed
        beq try_again    //      ;can't be zero
        sta rnd_a
 }
 #endif
}
#elif defined(RAND_1)
u8 rnd_x=7, rnd_y=3, rnd_z=31, rnd_a=62;
void rand(void)
{
 u8 t = rnd_x ^ (rnd_x << 4);
	rnd_x=rnd_y;
	rnd_y=rnd_z;
	rnd_z=rnd_a;
	rnd_a = rnd_z ^ t ^ ( rnd_z >> 1) ^ (t << 1);
}
void srand()
{
 #if defined(WIN32)
 rnd_a=LOBYTE(31232);
 #else
 __asm{
try_again:
        lda $a2          //       ;jiffy as random seed
        beq try_again    //      ;can't be zero
        sta rnd_a
 }
 #endif
}
#else

u8  rnd_a;
u16 seed = 31232;
void rand(void)
{
 seed ^= seed << 7;
 seed ^= seed >> 9;
 seed ^= seed << 8;
 rnd_a=seed&0xFF;
	//return seed;
}
void srand()
{ 
 #if defined(WIN32)
 rnd_a=0;
 #else
 __asm{
try_again:
        lda $a2          //       ;jiffy as random seed
        beq try_again    //      ;can't be zero
        sta rnd_a
 }
 #endif 
 seed=(seed&0xff00)|rnd_a;
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
#define PAGE_CREDITS       5
#define PAGE_HELP          6

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
#define HERO_SWORD     (0x1C00+(HERO_STAND*8+2))
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

#define efx_hit      8
#define efx_attack  16
#define efx_hunt   128

#define MAP_WALL1  1
#define MAP_WALL2  2

// -------------------------------------------------------------

void ovl1_performattack(u8 attacker,u8 attacked);
void ovl1_update(u8 h);

void game_draw_characters();
void game_move_hero();
void game_move_enemy(u8 r);

#if defined(HAS_GUI)
#if defined(USE_OVERLAYS)
#pragma code ( bcodegui )
#pragma data ( bdatagui )
#endif

void gui_update_legs_(u8*ptr);
void gui_update_legs(u8*ptr,u8 ch);
void gui_update_sword(u8*ptr,u8 ch);

#if defined(USE_OVERLAYS)
#pragma code ( code )
#pragma data ( data )
#endif
#endif

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
 u8  dx,ldx,tm;
 u8  health;
 u8  bkch,bkcol;
}_actor;

// -------------------------------------------------------------

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
u16       score,topscore=125;
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

void loadres(u8 which)
{
 u8*res=ADDR(0x02a1);
 res[0]='R';res[1]='0';res[2]='0'+which;
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
#if defined(HAS_KEYBOARD)
 if(movement==0)
  {
   a=PEEK(0xC5);
   if(a==0x9) // W
    movement|=move_up;
   else
    if(a==0x29) // S
     movement|=move_down;
    else
     if(a==0x11) // A
      movement|=move_left;
     else
      if(a==0x12) // D
       movement|=move_right;
   if(PEEK(0x028D)&1) // SHIFT
    movement|=move_fire;
  }
#endif
#endif  
 actors[HERO].dx=movement;
 
}

#include "main_game.h"

void readd_enemy(u8 h)
{ 
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

#include "main_genr.h"

#include "main_gui.h"

void game_updatelevels()
{
 charlevels[ENEMY1_KIND].val_attack=5+level;
 charlevels[ENEMY1_KIND].val_defence=2+level; 
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
 srand();

 POKE(VICCOLOR,SCREENBORDER_VALUE);

 // load small custom charset
#if defined(WIN32)
 res[1]='0';res[2]='2';
 loaddata(res,ADDR(0)); 
#else 
 loadres(2);
#endif

#if defined(WIN32)
 res[1]='0';res[2]='7';
 loaddata(res,ADDR(0)); 
#else 
 loadres(7);
#endif 

#if defined(WIN32)
 res[1]='0';res[2]='8';
 loaddata(res,ADDR(0)); 
#else 
 loadres(8);
#endif 
 
 POKE(VICCOLNC,VIDEO_W | 0x80);
#if defined(WIN32)
 POKE(VICSCRHO,PEEK(VICSCRHO)+(BASE_VIDEO_W-VIDEO_W)*8);
#else
 POKE(VICSCRHO,PEEK(VICSCRHO)+(BASE_VIDEO_W-VIDEO_W));
#endif
 
}

#include "main_page.h"

#if defined(HAS_MESSAGES) 
void draw_message(u8 r)
{ 
#if defined(WIN32)
 res[2]='0'+r;
 loaddata(res,ADDR(0)); 
#else 
 loadres(r);
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
     load("GAME");
#endif
#endif
     while(1)
      {
       get_input();
       REFRESH
       game_draw_characters();  
       gui_update();       
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

#if defined(HAS_MUSIC)     
     POKE(EFFECTS,0);
#endif

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