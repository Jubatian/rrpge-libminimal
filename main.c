/**
**  \file
**  \brief     The main program file
**  \author    Sandor Zsuga (Jubatian)
**  \license   2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEvt (temporary version of the RRPGE
**             License): see LICENSE.GPLv3 and LICENSE.RRPGEvt in the project
**             root.
**  \date      2015.08.04
*/



#include "host/types.h"
#include "host/screen.h"
#include "host/audio.h"
#include "host/filels.h"
#include "iface/render.h"

#include "librrpge/rrpge.h"

#include "version.h"

#include <SDL/SDL.h>
#include <errno.h>



/* Prototype for binary load kernel task */
static void main_loadbin(rrpge_object_t* hnd, rrpge_iuint tsh, const void* par);

/* Temporary areas */
static uint8  main_tdata[128U * 1024U];

/* File handle for the application (must be open while emulating) */
static FILE*  main_app;

/* Application name string */
static char const* main_appname = "RRPGE simple SDL emulator. Version: " EMULATOR_VERSION;
static char const* main_appicon = "RRPGE";

/* Other elements */
static char const* main_appauth = "By: Sandor Zsuga (Jubatian)\n";
static char const* main_copyrig = "License: 2013 - 2015, GNU GPLv3 (version 3 of the GNU General Public\nLicense) extended as RRPGEvt (temporary version of the RRPGE License):\nsee LICENSE.GPLv3 and LICENSE.RRPGEvt in the project root.\n";


/* Subroutines */
static const rrpge_cbd_sub_t main_cbsub[1] = {
 { RRPGE_CB_SETPAL,    &render_pal         }
};
/* Tasks */
static const rrpge_cbd_tsk_t main_cbtsk[1] = {
 { RRPGE_CB_LOADBIN,   &main_loadbin       }
};
/* Functions */
/* static const rrpge_cbd_fun_t main_cbfun[0] = { */
/* }; */

/* Callback structure for the emulator. Only line rendering for now. */
static const rrpge_cbpack_t main_cbpack={
 &render_line,
 1,                           /* Task callbacks */
 &main_cbtsk[0],
 1,                           /* Subroutine callbacks */
 &main_cbsub[0],
 0,                           /* Function callbacks */
 NULL
};



/* Loads binary page, kernel task callback */
static void main_loadbin(rrpge_object_t* hnd, rrpge_iuint tsh, const void* par)
{
 const rrpge_cbp_loadbin_t* p = (const rrpge_cbp_loadbin_t*)(par);
 /* No async task loading, neither check stuff, just run it. Prototype...
 ** Note that p->scw can be at most 65536. */
 if (main_app != NULL){
  filels_read(main_app, (p->sow) << 1, (p->scw) << 1,  &main_tdata[0]);
 }
 rrpge_conv_b2w(&main_tdata[0], (p->buf), (p->scw) << 1);
 rrpge_taskend(hnd, tsh, 0x8000U);
}



/* Wrapper for malloc to fix type */
static void* main_malloc(rrpge_iuint siz)
{
 return malloc(siz);
}



/* Prints error message according to the passed RRPGE error code */
static void main_printrerr(auint e)
{
 printf("RRPGE error: ");

 if       (e == 0){
  printf("No error\n");
 }else if (e == RRPGE_ERR_WAIT){
  printf("RRPGE_ERR_WAIT\n");
 }else if (e == RRPGE_ERR_UNK){
  printf("RRPGE_ERR_UNK\n");
 }else if (e == RRPGE_ERR_INI){
  printf("RRPGE_ERR_INI\n");
 }else if (e == RRPGE_ERR_VER){
  printf("RRPGE_ERR_VER\n");
 }else if ((e & 0xF000U) == RRPGE_ERR_STA){
  printf("RRPGE_ERR_STA, location: 0x%03X\n", e & 0xFFFU);
 }else if ((e & 0xF000U) == RRPGE_ERR_UNS){
  printf("RRPGE_ERR_UNS, location: 0x%03X\n", e & 0xFFFU);
 }else if ((e & 0xF000U) == RRPGE_ERR_DSC){
  printf("RRPGE_ERR_DSC, location: 0x%03X\n", e & 0xFFFU);
 }else{
  printf("Unknown, code: 0x%04X\n", e);
 }
}



/* Prints halt causes */
static void main_printhalt(auint h)
{
 printf("RRPGE halt:");

 if (h & RRPGE_HLT_AUDIO)   { printf(" AUDIO"); }
 if (h & RRPGE_HLT_EXIT)    { printf(" EXIT"); }
 if (h & RRPGE_HLT_FRAME)   { printf(" FRAME"); }
 if (h & RRPGE_HLT_CALLBACK){ printf(" CALLBACK"); }
 if (h & RRPGE_HLT_BREAK)   { printf(" BREAK"); }
 if (h & RRPGE_HLT_INVOP)   { printf(" INVOP"); }
 if (h & RRPGE_HLT_INVKCALL){ printf(" INVKCALL"); }
 if (h & RRPGE_HLT_STACK)   { printf(" STACK"); }
 if (h & RRPGE_HLT_DETACHED){ printf(" DETACHED"); }
 if (h & RRPGE_HLT_WAIT)    { printf(" WAIT"); }
 if (h & RRPGE_HLT_FAULT)   { printf(" FAULT"); }
 if (h == 0)                { printf(" No halt"); }

 printf("\n");
}



/* Prints some emulator state */
static void main_printstats(rrpge_object_t* obj)
{
 auint  bp;

 /* CPU Registers */

 printf("A...: %04X; B...: %04X; C...: %04X; D...: %04X\n",
        rrpge_get_state(obj, RRPGE_STA_VARS + 0x0U),
        rrpge_get_state(obj, RRPGE_STA_VARS + 0x1U),
        rrpge_get_state(obj, RRPGE_STA_VARS + 0x2U),
        rrpge_get_state(obj, RRPGE_STA_VARS + 0x3U));
 printf("X0..: %04X; X1..: %04X; X2..: %04X; X3..: %04X\n",
        rrpge_get_state(obj, RRPGE_STA_VARS + 0x4U),
        rrpge_get_state(obj, RRPGE_STA_VARS + 0x5U),
        rrpge_get_state(obj, RRPGE_STA_VARS + 0x6U),
        rrpge_get_state(obj, RRPGE_STA_VARS + 0x7U));
 printf("XM..: %04X; XH..: %04X\n",
        rrpge_get_state(obj, RRPGE_STA_VARS + 0x8U),
        rrpge_get_state(obj, RRPGE_STA_VARS + 0x9U));
 printf("BP..: %04X; SP..: %04X\n",
        rrpge_get_state(obj, RRPGE_STA_VARS + 0xCU),
        rrpge_get_state(obj, RRPGE_STA_VARS + 0xBU));
 printf("PC..: %04X\n",
        rrpge_get_state(obj, RRPGE_STA_VARS + 0xAU));

 /* Stack */

 bp = (auint)(rrpge_get_state(obj, RRPGE_STA_VARS + 0xCU));

 printf("BP+0: %04X; %04X; %04X; %04X; %04X; %04X; %04X; %04X\n",
        rrpge_get_stack(obj, bp + 0x0U), rrpge_get_stack(obj, bp + 0x1U),
        rrpge_get_stack(obj, bp + 0x2U), rrpge_get_stack(obj, bp + 0x3U),
        rrpge_get_stack(obj, bp + 0x4U), rrpge_get_stack(obj, bp + 0x5U),
        rrpge_get_stack(obj, bp + 0x6U), rrpge_get_stack(obj, bp + 0x7U));
 printf("BP+8: %04X; %04X; %04X; %04X; %04X; %04X; %04X; %04X\n",
        rrpge_get_stack(obj, bp + 0x8U), rrpge_get_stack(obj, bp + 0x9U),
        rrpge_get_stack(obj, bp + 0xAU), rrpge_get_stack(obj, bp + 0xBU),
        rrpge_get_stack(obj, bp + 0xCU), rrpge_get_stack(obj, bp + 0xDU),
        rrpge_get_stack(obj, bp + 0xEU), rrpge_get_stack(obj, bp + 0xFU));
}



/* Prints exit error message */
static void main_errexit(auint h, rrpge_object_t* obj)
{
 printf("Emulation halted because of error in source program!\n");
 main_printhalt(h);
 main_printstats(obj);
}



int main(int argc, char** argv)
{
 auint  j;
 auint  t;
 auint  cdi = 0U;
 auint  auc = 0U;
 uint8* lpt;
 uint8* rpt;
 uint16 edt;           /* Event data (just one) */
 auint  mid;           /* Mouse device id */
 SDL_Event event;      /* The event got from the queue */
 rrpge_object_t* emu = NULL;



 /* Init message */
 printf("\n");
 printf("%s", main_appname);
 printf("\n\n");
 printf("%s", main_appauth);
 printf("%s", main_copyrig);
 printf("\n");



 /* Check arguments: need an application */
 if (argc <= 1){
  printf("Error: need an application to run as parameter!\n");
  exit(1);
 }else{
  printf("Opening %s...\n", argv[1]);
  main_app = fopen(argv[1], "rb");
  if (main_app == NULL){
   perror("Failed to open file");
   exit(1);
  }
 }



 /* Initialize emulator library */
 rrpge_init_lib(&main_malloc, &free);



 /* Attempt to initialize the emulator. Note that the app. binary load
 ** callback is blocking, so no need to implement any waiting here using
 ** rrpge_init_run(). */
 emu = rrpge_new_emu(&main_cbpack);
 if (emu == NULL){
  printf("Failed to allocate emulator state\n");
  goto loadfault;
 }
 t = rrpge_init_run(emu, RRPGE_INI_RESET);
 if (t != RRPGE_ERR_OK){
  printf("Failed to initialize emulator\n");
  main_printrerr(t);
  goto loadfault;
 }
 mid = rrpge_dev_add(emu, RRPGE_DEV_POINT); /* Add mouse (pointing device) */

 /* Initialize renderer */
 render_reset(emu);

 /* Initialize SDL */
 if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)!=0) return -1;

 /* Try to set up the screen */
 if (screen_set()!=0) return -1;
 SDL_WM_SetCaption(main_appname, main_appicon);

 /* Set up audio */
 if (audio_set(2048U) != 0U) return -1;

 /* OK, let's go! */

 printf("Entering emulation\n");

 /* The main loop: It is more straightforward to exit from it with a suitable
 ** break at the proper place */
 while(1){

  /* Wait for some event... */
  if (SDL_WaitEvent(&event)==0) return -1; /*Error?*/

  /* An event is here: do something about it! */
  if (event.type==SDL_USEREVENT){
   if (event.user.code==AUDIO_EVENT){

    t = 0U;                      /* No halt */

    while (audio_needservice()){ /* Waiting for audio data from emulator */

     do{
      j = rrpge_run(emu, RRPGE_RUN_FREE);
      t = rrpge_gethaltcause(emu);
      if (t & RRPGE_HLT_AUDIO){ cdi++; }
      if (cdi >= 10){
       cdi = 0U;
       printf("Audio events: %08d\n", auc);
       printf("Cycles: %08d\n", j);
       main_printstats(emu);
       main_printhalt(t);
      }
      /* Need proper exit point... */
     }while ((t & (RRPGE_HLT_AUDIO |
                   RRPGE_HLT_EXIT |
                   RRPGE_HLT_STACK |
                   RRPGE_HLT_INVKCALL |
                   RRPGE_HLT_INVOP |
                   RRPGE_HLT_FAULT |
                   RRPGE_HLT_DETACHED |
                   RRPGE_HLT_WAIT)) == 0U);

     if ((t & RRPGE_HLT_AUDIO) != 0U){ /* Audio data produced by emulator */

      audio_getbufptrs(&lpt, &rpt);
      rrpge_getaudio(emu, lpt, rpt);
      auc++;

     }

     if ((t & (RRPGE_HLT_EXIT |
               RRPGE_HLT_STACK |
               RRPGE_HLT_INVKCALL |
               RRPGE_HLT_INVOP |
               RRPGE_HLT_FAULT |
               RRPGE_HLT_DETACHED |
               RRPGE_HLT_WAIT)) != 0U){ /* Errors & Exit */
      break;
     }

    }                                  /* End of audio wait loop */

    if ((t & RRPGE_HLT_EXIT) != 0U){ break; } /* Exit program */
    if ((t & (RRPGE_HLT_STACK |
              RRPGE_HLT_INVKCALL |
              RRPGE_HLT_INVOP |
              RRPGE_HLT_FAULT |
              RRPGE_HLT_DETACHED |
              RRPGE_HLT_WAIT)) != 0U){
     main_errexit(t, emu);
     break;                            /* Also exit, with error */
    }

   }
  }else if (event.type==SDL_KEYDOWN){
   /* Some key was pressed. Which? */
   if (event.key.keysym.sym==SDLK_ESCAPE) break; /* Exit program */

  }else if (event.type==SDL_KEYUP){

  }else if (event.type==SDL_QUIT){     /* Exit program */
   break;

  }else if (event.type==SDL_MOUSEBUTTONDOWN){

   edt = (uint16)(event.button.x);
   rrpge_dev_push(emu, mid, 2U, 1U, &edt);
   edt = (uint16)(event.button.y);
   rrpge_dev_push(emu, mid, 3U, 1U, &edt);
   if      (event.button.button == SDL_BUTTON_LEFT){   edt = 1U; rrpge_dev_push(emu, mid, 0U, 1U, &edt); }
   else if (event.button.button == SDL_BUTTON_RIGHT){  edt = 2U; rrpge_dev_push(emu, mid, 0U, 1U, &edt); }
   else if (event.button.button == SDL_BUTTON_MIDDLE){ edt = 3U; rrpge_dev_push(emu, mid, 0U, 1U, &edt); }
   else {}

  }else if (event.type==SDL_MOUSEBUTTONUP){

   edt = (uint16)(event.button.x);
   rrpge_dev_push(emu, mid, 2U, 1U, &edt);
   edt = (uint16)(event.button.y);
   rrpge_dev_push(emu, mid, 3U, 1U, &edt);
   if      (event.button.button == SDL_BUTTON_LEFT){   edt = 1U; rrpge_dev_push(emu, mid, 1U, 1U, &edt); }
   else if (event.button.button == SDL_BUTTON_RIGHT){  edt = 2U; rrpge_dev_push(emu, mid, 1U, 1U, &edt); }
   else if (event.button.button == SDL_BUTTON_MIDDLE){ edt = 3U; rrpge_dev_push(emu, mid, 1U, 1U, &edt); }
   else {}

  }else if (event.type==SDL_MOUSEMOTION){

   edt = (uint16)(event.motion.x);
   rrpge_dev_push(emu, mid, 2U, 1U, &edt);
   edt = (uint16)(event.motion.y);
   rrpge_dev_push(emu, mid, 3U, 1U, &edt);

  }

 }

 printf("Trying to exit\n");

 rrpge_delete(emu);
 audio_free();
 screen_free();

 SDL_Quit();

 fclose(main_app);

 exit(0);


loadfault:

 if (emu != NULL) free(emu);
 fclose(main_app);

 exit(1);
}
