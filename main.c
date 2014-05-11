/**
**  \file
**  \brief     The main program file
**  \author    Sandor Zsuga (Jubatian)
**  \copyright 2013 - 2014, GNU GPLv3 (version 3 of the GNU General Public
**             License) extended as RRPGEv2 (version 2 of the RRPGE License):
**             see LICENSE.GPLv3 and LICENSE.RRPGEv2 in the project root.
**  \date      2014.05.10
*/



#include "host/types.h"
#include "host/screen.h"
#include "host/audio.h"
#include "host/filels.h"
#include "iface/render.h"

#include "librrpge/rrpge.h"

#include <SDL/SDL.h>
#include <errno.h>



/* Prototype for binary load kernel task */
static void main_loadbin(rrpge_object_t* hnd, rrpge_uint32 tsh, const void* par);

/* Temporary pages */
static uint8  main_tpg8 [8192U];
static uint16 main_tpg16[4096U];
static uint16 main_crom [4096U * 16U];

/* File handle for the application (must be open while emulating) */
static FILE*  main_app;
/* Start of binary pages in app. */
static auint  main_appbs;

/* Subroutines */
static const rrpge_cbd_sub_t main_cbsub[2] = {
 { RRPGE_CB_SETPAL,    &render_pal     },
 { RRPGE_CB_MODE,      &render_mode    }
};
/* Tasks */
static const rrpge_cbd_tsk_t main_cbtsk[1] = {
 { RRPGE_CB_LOADBIN,   &main_loadbin   }
};

/* Callback structure for the emulator. Only line rendering for now. */
static const rrpge_cbpack_t main_cbpack={
 &render_line,
 1,                           /* Task callbacks */
 &main_cbtsk[0],
 2,                           /* Subroutine callbacks */
 &main_cbsub[0],
 0,                           /* No function callbacks */
 NULL                         /* (No need to supply function callback data) */
};



/* Loads binary page, kernel task callback */
static void main_loadbin(rrpge_object_t* hnd, rrpge_uint32 tsh, const void* par)
{
 const rrpge_cbp_loadbin_t* p = (const rrpge_cbp_loadbin_t*)(par);
 /* No async task loading, neither check stuff, just run it. Prototype... */
 if (main_app != NULL){
  filels_readpage(main_app, main_appbs + (p->spg), &main_tpg8[0]);
 }
 rrpge_convpg_b2w(&main_tpg8[0], (p->buf));
 rrpge_taskend(hnd, tsh, 0x8000U);
}



/* Prints error message according to the passed RRPGE error code */
static void main_printrerr(auint e)
{
 printf("RRPGE error: ");

 if       (e == 0){
  printf("No error\n");
 }else if (e == 0x0001U){
  printf("RRPGE_ERR_UNK\n");
 }else if (e == 0x0002U){
  printf("RRPGE_ERR_LIC\n");
 }else if (e == 0x0003U){
  printf("RRPGE_ERR_PG\n");
 }else if (e == 0x0004U){
  printf("RRPGE_ERR_VER\n");
 }else if ((e & 0xF000U) == 0x1000U){
  printf("RRPGE_ERR_INV, location: 0x%03X\n", e & 0xFFFU);
 }else if ((e & 0xF000U) == 0x2000U){
  printf("RRPGE_ERR_UNS, location: 0x%03X\n", e & 0xFFFU);
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
 if (h & RRPGE_HLT_FAULT)   { printf(" FAULT"); }
 if (h == 0)                { printf(" No halt"); }

 printf("\n");
}



/* Prints exit error message */
static void main_errexit(auint h, rrpge_state_t const* sta)
{
 auint i;

 printf("Emulation halted because of error in source program!\n");
 main_printhalt(h);
 printf("Regs: %04X %04X %04X %04X %04X %04X %04X %04X\n",
        sta->ropd[0xD40U], sta->ropd[0xD41U], sta->ropd[0xD42U], sta->ropd[0xD43U],
        sta->ropd[0xD44U], sta->ropd[0xD45U], sta->ropd[0xD46U], sta->ropd[0xD47U]);
 printf("PC..: %04X\nXM..: %04X; XH: %04X\nBP..: %04X; SP: %04X\n",
        sta->ropd[0xD4AU], sta->ropd[0xD48U], sta->ropd[0xD49U],
        sta->ropd[0xD4CU], sta->ropd[0xD4BU]);
 i = sta->ropd[0xD4CU];
 printf("BP+0: %04X %04X %04X %04X %04X %04X %04X %04X\n",
        sta->sram[i + 0U], sta->sram[i + 1U], sta->sram[i + 2U], sta->sram[i + 3U],
        sta->sram[i + 4U], sta->sram[i + 5U], sta->sram[i + 6U], sta->sram[i + 7U]);
 i+= 8U;
 printf("BP+8: %04X %04X %04X %04X %04X %04X %04X %04X\n",
        sta->sram[i + 0U], sta->sram[i + 1U], sta->sram[i + 2U], sta->sram[i + 3U],
        sta->sram[i + 4U], sta->sram[i + 5U], sta->sram[i + 6U], sta->sram[i + 7U]);
}



int main(int argc, char** argv)
{
 auint  i;
 auint  j;
 auint  t;
 auint  cdi = 0U;
 auint  auc = 0U;
 uint8* lpt;
 uint8* rpt;
 SDL_Event event;      /* The event got from the queue */
 rrpge_object_t*      emu = NULL;
 rrpge_state_t const* sta = NULL;



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



 /* Try to load the application */
 j = filels_readpage(main_app, 0U, &(main_tpg8[0]));
 if (j < 8192U){
  printf("Failed to load application header (loaded size: %i)\n", j);
  goto loadfault;
 }
 rrpge_convpg_b2w(&(main_tpg8[0]), &(main_tpg16[0]));

 /* Load code pages */
 t = main_tpg16[0xBC2];             /* Extract number of code pages */
 t = (((t >> 8) - 1U) & 0xFU) + 1U; /* Code pages: 1-16 */
 for (i = 0; i < t; i++){
  j = filels_readpage(main_app, i + 1U, &(main_tpg8[0]));
  if (j < 8192U){
   printf("Failed to load code page (page: %i, loaded size: %i)\n", i, j);
   goto loadfault;
  }
  rrpge_convpg_b2w(&(main_tpg8[0]), &(main_crom[i << 12]));
 }

 /* Set binary data start */
 main_appbs = t + 1U;

 /* Allocate emulator state. This is temporary as is since the RRPGE library's
 ** rrpge_getdescription() is not implemented yet which would give the size of
 ** the emulator state. */
 emu = malloc(4U * 1024U * 1024U); /* Fits well in 4 megabytes. */
 if (emu == NULL){
  printf("Failed to allocate emulator state\n");
  goto loadfault;
 }

 /* Attempt to initialize the emulator over this stuff. */
 t = rrpge_init(&main_cbpack,         /* Callback set */
                &(main_tpg16[0]),     /* Application header */
                &(main_crom[0]),      /* Code memory */
                t,                    /* Number of code pages */
                emu);                 /* Emulator object to fill in */
 if (t != RRPGE_ERR_OK){
  printf("Failed to initialize emulator\n");
  main_printrerr(t);
  goto loadfault;
 }

 /* Initialize renderer */
 render_reset(emu);

 /* Initialize SDL */
 if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)!=0) return -1;

 /* Try to set up the screen */
 if (screen_set()!=0) return -1;

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
      cdi++;
      if (cdi >= 100){
       cdi = 0U;
       printf("Audio events: %08d\n", auc);
       printf("Cycles: %08d\n", j);
       sta = rrpge_exportstate(emu);
       printf("Regs: %04X %04X %04X %04X %04X %04X %04X %04X\n",
              sta->ropd[0xD40U], sta->ropd[0xD41U], sta->ropd[0xD42U], sta->ropd[0xD43U],
              sta->ropd[0xD44U], sta->ropd[0xD45U], sta->ropd[0xD46U], sta->ropd[0xD47U]);
       printf("PC: %04X, XM: %04X, XH: %04X, BP: %04X, SP: %04X\n",
              sta->ropd[0xD4AU], sta->ropd[0xD48U], sta->ropd[0xD49U],
              sta->ropd[0xD4CU], sta->ropd[0xD4BU]);
       printf("Data: %04X %04X %04X %04X %04X %04X %04X %04X\n",
              sta->dram[0x000U], sta->dram[0x001U], sta->dram[0x002U], sta->dram[0x003U],
              sta->dram[0x004U], sta->dram[0x005U], sta->dram[0x006U], sta->dram[0x007U]);
       printf("      %04X %04X %04X %04X %04X %04X %04X %04X\n",
              sta->dram[0x008U], sta->dram[0x009U], sta->dram[0x00AU], sta->dram[0x00BU],
              sta->dram[0x00CU], sta->dram[0x00DU], sta->dram[0x00EU], sta->dram[0x00FU]);
       printf("Stac: %04X %04X %04X %04X %04X %04X %04X %04X\n",
              sta->sram[0x000U], sta->sram[0x001U], sta->sram[0x002U], sta->sram[0x003U],
              sta->sram[0x004U], sta->sram[0x005U], sta->sram[0x006U], sta->sram[0x007U]);
       main_printhalt(t);
      }
      /* Need proper exit point... */
     }while ((t & (RRPGE_HLT_AUDIO |
                   RRPGE_HLT_EXIT |
                   RRPGE_HLT_STACK |
                   RRPGE_HLT_INVKCALL |
                   RRPGE_HLT_INVOP |
                   RRPGE_HLT_FAULT)) == 0U);

     if ((t & RRPGE_HLT_AUDIO) != 0U){ /* Audio data produced by emulator */

      audio_getbufptrs(&lpt, &rpt);
      rrpge_getaudio(emu, lpt, rpt);
      auc++;

     }else{                            /* Any other halt cause is an error */
      break;
     }

    }                            /* End of audio wait loop */

    if ((t & RRPGE_HLT_EXIT) != 0U){ break; } /* Exit program */
    if ((t & (RRPGE_HLT_STACK |
              RRPGE_HLT_INVKCALL |
              RRPGE_HLT_INVOP |
              RRPGE_HLT_FAULT)) != 0U){
     main_errexit(t, sta);
     break;                                   /* Also exit, with error */
    }

   }
  }else if (event.type==SDL_KEYDOWN){
   /* Some key was pressed. Which? */
   if (event.key.keysym.sym==SDLK_ESCAPE) break; /* Exit program */

  }else if (event.type==SDL_KEYUP){

  }else if (event.type==SDL_QUIT) break; /* Exit as well */

 }

 printf("Trying to exit\n");

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
