#if defined(USE_OVERLAYS)
#pragma code ( bcodepage )
#pragma data ( bdatapage )
#endif

#if defined(HAS_MUSIC)
#define NOTE_POS   0
#define NOTE_TIME  1
#define NOTE_HM    2
#define NOTE_START 3
void music_play()
{
 u8*ptr=ADDR(0x1e00+VIDEO_W*VIDEO_H+64); 
 if(ptr[NOTE_TIME]==0)
  {
   POKE(VOLUME,0xF);
   POKE(EFFECTS,ptr[NOTE_START+ptr[NOTE_POS]]);
   ptr[NOTE_TIME]=16;
   ptr[NOTE_POS]++;
   if(ptr[NOTE_POS]==ptr[NOTE_HM])
    ptr[NOTE_POS]=0;
  }
 else
  ptr[NOTE_TIME]--;
}
void music_stop()
{
 POKE(EFFECTS,0);
 //POKE(VOLUME,0x0);
}
#endif

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
 #if defined(HAS_MUSIC)
 music_stop();
 #endif 
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
            LDA #$00
            STA $900c
 }
#endif
}

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

void page_set_num(u8 val)
{
 video[0]=video[1]='0';
 while(val>=10)
 {
  val-=10;  
  video[0]=video[0]+1;
 } 
 video[1]+=val; 
 video+=2;
//#endif
}
void page_set_num16_(u16 cut)
{ 
 while(pos>=cut)
 {
  pos-=cut;  
  *video=*video+1;
 } 
 video++;
}
void page_draw_score()
{ 
 video[0]=video[1]=video[2]=video[3]='0';
 pos=score;
 page_set_num16_(1000);
 page_set_num16_(100);
 page_set_num16_(10);
 *video+=(u8)pos; 
 video++;
}
void page_draw_page(u8 r)
{
 #if defined(HAS_PAGES)
 while(1)
  {
  u8 pg=r;

  memset(ADDR(COLOR_RAM),0,VIDEO_W*VIDEO_H); 
  
 #if defined(WIN32)
  res[2]='0'+r;
  loaddata(res,ADDR(0)); 
 #else 
  loadres(r);
 #endif
  #endif

  #if defined(HAS_PAGES)
  #if defined(HAS_PETPAGES)
  POKE(VICCHGEN,(PEEK(VICCHGEN)&0xF0)|0x0);
  page_unpack(ADDR(0x1c38));
  #else
  POKE(VICCHGEN,(PEEK(VICCHGEN)&0xF0)|0xF);
  unpack(ADDR(0x1C00+24*8));
  #endif
  #endif
  
  if(pg==PAGE_ENTERINGLEVEL)
   {
    video=ADDR(VIDEO_RAM+18*VIDEO_W)+11;
    page_set_num(level);  
    break;
   }
  else
   {
    if(pg==PAGE_GAMEOVER)
     {
      video=ADDR(VIDEO_RAM+14*VIDEO_W)+5;
      page_set_num(level);
      video+=2;
      page_draw_score();
      if(topscore<score) topscore=score;
      //video=ADDR(VIDEO_RAM+17*VIDEO_W)+5+2+2;      
      score=topscore;
      video+=VIDEO_W*3-4;
      page_draw_score();
      
     }
    wait_fire();
    ch=PEEK(0xC5);
    if(ch==KEY_C)
     r=PAGE_CREDITS;
    else
    if(ch==KEY_H)
     r=PAGE_HELP;
    else
     break;
   }
  }
}

#if defined(USE_OVERLAYS)
#pragma code ( code )
#pragma data ( data )
#endif