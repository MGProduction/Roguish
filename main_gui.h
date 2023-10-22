#if defined(HAS_GUI)
#if defined(USE_OVERLAYS)
#pragma code ( bcodegui )
#pragma data ( bdatagui )
#endif

void gui_update_sword(u8*ptr,u8 dx)
{
 u8 chA=ptr[0],chB=ptr[1];
 if(dx&move_left)
  {chA=0x80;chB=0xbc;}
 else
 if(dx&move_right)
  {chA=1;chB=0x3d;}
  
 ptr[0]=chA;ptr[1]=chB;
}

void gui_update_legs_(u8*ptr)
{
 u8 ch=*ptr;
 if(ch==32) ch=4; else ch=32;
 *ptr=ch;
}
void gui_update_legs(u8*ptr,u8 ch)
{
 if(ch==2)
  {
   if((timer&7)==1)
    gui_update_legs_(ptr);    
  }
 else
  *ptr=32|4;
}

void gui_set_num(u8 val)
{
 *video=GUI_NUMBER;
 while(val>=10)
 {
  val-=10;  
  *video=*video+1;
 } 
 video++;
 *video=(GUI_NUMBER+val); 
 video++;
//#endif
}
void gui_set_num16_(u16 cut)
{ 
 while(pos>=cut)
 {
  pos-=cut;  
  *video=*video+1;
 } 
 video++;
}
void gui_draw_score()
{ 
 video[0]=video[1]=video[2]=video[3]=GUI_NUMBER;
 pos=score;
 gui_set_num16_(1000);
 gui_set_num16_(100);
 gui_set_num16_(10);
 *video+=(u8)pos; 
 video++;
}
void enter_gui()
{
 video=ADDR(VIDEO_RAM+20*VIDEO_W);
 color=ADDR(COLOR_RAM+20*VIDEO_W);
}
void gui_update()
{
 enter_gui();
 video+=VIDEO_W+1;
 gui_set_num(actors[HERO].health);
 video++;
 gui_set_num(charlevels[HERO_KIND].val_defence);
 video++;
 gui_set_num(charlevels[HERO_KIND].val_attack);
 video+=2;
 gui_draw_score();
 
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