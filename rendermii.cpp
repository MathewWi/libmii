/*===========================================
    RenderMii Example
    Code     : mdbrim

    rendermii.cpp
============================================*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiiuse/wpad.h>
#include <ogcsys.h>
#include <gccore.h>
#include <math.h>
#include <fat.h>
#include <asndlib.h>
#include <mp3player.h>
#include <stdarg.h>

#include <mii.h>
#include <grrlib.h>
#include <FreeTypeGX.h>

#include "pointer_png.h"
#include "mii_heads_png.h"
#include "mii_hairs1_png.h"
#include "mii_hairs2_png.h"
#include "mii_eyebrows_png.h"
#include "mii_eyes1_png.h"
#include "mii_eyes2_png.h"
#include "mii_eyes3_png.h"
#include "mii_noses_png.h"
#include "mii_features_png.h"
#include "mii_lips_png.h"
#include "mii_mole_png.h"
#include "mii_beards_png.h"
#include "mii_mustache_png.h"
#include "mii_glasses_png.h"
#include "bankgothic_med_ttf.h"

Mii* miis;

static GRRLIB_texImg* img_pointer;
static GRRLIB_texImg* img_heads;
static GRRLIB_texImg* img_hairs1;
static GRRLIB_texImg* img_hairs2;
static GRRLIB_texImg* img_eyebrows;
static GRRLIB_texImg* img_eyes1;
static GRRLIB_texImg* img_eyes2;
static GRRLIB_texImg* img_eyes3;
static GRRLIB_texImg* img_noses;
static GRRLIB_texImg* img_features;
static GRRLIB_texImg* img_lips;
static GRRLIB_texImg* img_mole;
static GRRLIB_texImg* img_beards;
static GRRLIB_texImg* img_mustache;
static GRRLIB_texImg* img_glasses;
static GRRLIB_texImg* img_mii;              //This is the placeholder that we will store our built mii into
static GRRLIB_texImg* img_staticmii[100];   //This is where we will store up to 100 drawn static miis

static FreeTypeGX* bankgothic16;

u32 wpaddown,wpadheld;
ir_t ir1,ir2,ir3;

// ************************ DEFINE OUR ARRAYS ********************
int hairbg[72] = {56,56,56,56,56,56,56,56,56,56,56,56,16,56,56,56,56,56,17,18,56,19,20,56,56,56,21,56,56,56,56,56,56,56,56,56,22,23,56,56,24,25,56,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,56,56,48,49,50,51,52,53,56};
int hairfg[72] = {59,42,65,49,40,44,52,47,45,63,51,54,36,37,48,70,61,56,64,43,53,58,50,27,69,41,39,46,66,71,33,11,12,0,35,57,30,14,25,4,1,31,26,24,3,6,62,13,15,7,19,2,17,67,29,20,9,34,18,8,22,60,23,55,21,32,16,28,10,38,5,68};
int haircol[8] = {0x111111FF, 0x332222FF, 0x441111FF, 0xBB6644FF, 0x8888AAFF, 0x443322FF, 0x996644FF, 0xDDBB99FF};
int skincol[6] = {0xECCFBDFF, 0xF7BC7dFF, 0xD78A48FF, 0xF5B189FF, 0x995122FF, 0x563010FF};
int eyecol[6] = {0x000000FF, 0x778887FF, 0x7E6355FF, 0x888940FF, 0x6A84D0FF, 0x409B5AFF};
int lipcol[3] = {0xC76C46FF, 0xE44E3AFF, 0xD88789FF};
int glassescol[6] = {0x626D6CFF, 0x85703AFF, 0xAB4E37FF, 0x426996FF, 0xB97F27FF, 0xBDBFB9FF};
int eyebrows[24] = {1,3,14,15,11,10,0,6,8,4,13,12,2,19,16,18,22,9,21,5,17,7,20,23};
int eyes[48] = {2,6,0,42,1,24,29,36,3,16,45,13,17,26,46,9,8,5,33,14,11,20,44,18,30,21,7,10,34,41,31,32,15,12,19,23,27,28,38,4,22,25,39,43,37,40,35,47};
int noses[12] = {5,0,2,3,7,6,4,10,8,9,1,11};
int lips[24] = {6,1,14,16,17,5,10,12,7,13,8,19,23,11,22,18,9,15,21,2,20,3,4,0};
int feature=0;
// ***************************************************************


// ************** DEFINE SOME VARIABLES FOR PROG USE *************
int selectedmii=0;
int blink=0;
int blinkrand=0;
int blinkcount=0;
int startblink=0;
// ***************************************************************

void printf_ttf(FreeTypeGX* font, int xpos, int ypos, int shadow, GXColor fgc, GXColor bgc, u16 style, const char *s, ...) { //Just a function i wrote to draw TTF text with 3d effect.
	va_list ap;
	char line[200];
	wchar_t line_wchar[200];

	va_start(ap, s);
	vsnprintf(line, 200, s, ap);
	va_end(ap);

	char *src = line;
	wchar_t *dst = line_wchar;
	while((*dst++ = *src++));
	
	font->drawText(xpos+shadow,ypos+shadow,line_wchar, bgc, style);
	font->drawText(xpos, ypos, line_wchar, fgc, style);
}
 
void GRRLIB_MiisInit() { // INITIALISE THE MIIS!!!
	
	img_heads = GRRLIB_LoadTexture(mii_heads_png);
	img_hairs1 = GRRLIB_LoadTexture(mii_hairs1_png);
	img_hairs2 = GRRLIB_LoadTexture(mii_hairs2_png);
	img_eyebrows = GRRLIB_LoadTexture(mii_eyebrows_png);
	img_eyes1 = GRRLIB_LoadTexture(mii_eyes1_png);
	img_eyes2 = GRRLIB_LoadTexture(mii_eyes2_png);
	img_eyes3 = GRRLIB_LoadTexture(mii_eyes3_png);
	img_noses = GRRLIB_LoadTexture(mii_noses_png);
	img_features = GRRLIB_LoadTexture(mii_features_png);
	img_lips = GRRLIB_LoadTexture(mii_lips_png);
	img_mole = GRRLIB_LoadTexture(mii_mole_png);
	img_beards = GRRLIB_LoadTexture(mii_beards_png);
	img_mustache = GRRLIB_LoadTexture(mii_mustache_png);
	img_glasses = GRRLIB_LoadTexture(mii_glasses_png);
	
	GRRLIB_InitTileSet(img_heads, 120, 120, 0);
	GRRLIB_InitTileSet(img_hairs1, 120, 120, 0);
	GRRLIB_InitTileSet(img_hairs2, 120, 120, 0);
	GRRLIB_InitTileSet(img_eyebrows, 36, 54, 0);
	GRRLIB_InitTileSet(img_eyes1, 54, 54, 0);
	GRRLIB_InitTileSet(img_eyes2, 54, 54, 0);
	GRRLIB_InitTileSet(img_eyes3, 54, 54, 0);
	GRRLIB_InitTileSet(img_noses, 50, 50, 0);
	GRRLIB_InitTileSet(img_features, 120, 120, 0);
	GRRLIB_InitTileSet(img_lips, 60, 60, 0);
	GRRLIB_InitTileSet(img_beards, 120, 140, 0);
	GRRLIB_InitTileSet(img_mustache, 60, 60, 0);
	GRRLIB_InitTileSet(img_glasses, 180, 72, 0);
	
	miis = loadMiis_Wii();
}	


void GRRLIB_BuildMii(Mii mii, const int miiblinks, int miieyes, int miimouth, GRRLIB_texImg* tex) {  // THE HEART AND SOLE... BUILD YOUR MII
	
	GRRLIB_CompoStart();

	GRRLIB_SetHandle(img_hairs1, 60,0);
	GRRLIB_SetHandle(img_hairs2, 60,0);
	GRRLIB_DrawTile(230, 210, img_hairs2, 0, -1*(2*mii.hairPart-1), 1, haircol[mii.hairColor], hairbg[hairfg[mii.hairType]]);
	GRRLIB_DrawTile(230, 200, img_heads, 0, 1, 1, skincol[mii.skinColor], mii.faceShape);
	
	
	if(mii.facialFeature==0) feature=39;
	else if(mii.facialFeature==1) feature=mii.faceShape;
	else if(mii.facialFeature==2) feature=mii.faceShape;
	else if(mii.facialFeature==3) feature=33;
	else if(mii.facialFeature==4) feature=34;
	else if(mii.facialFeature==5) feature=35;
	else if(mii.facialFeature==6) feature=36;
	else if(mii.facialFeature==7) feature=40+mii.faceShape;
	else if(mii.facialFeature==8) feature=38;
	else if(mii.facialFeature==9) feature=8+mii.faceShape;
	else if(mii.facialFeature==10) feature=16+mii.faceShape;
	else if(mii.facialFeature==11) feature=24+mii.faceShape;
	
	if(mii.facialFeature==2) {
		GRRLIB_DrawTile(230, 200, img_features, 0, 1, 1, skincol[mii.skinColor], 32);
		GRRLIB_DrawTile(230, 200, img_features, 0, 1, 1, skincol[mii.skinColor], feature);
	}
	else GRRLIB_DrawTile(230, 200, img_features, 0, 1, 1, skincol[mii.skinColor], feature);
	
	if(mii.beardType>0) GRRLIB_DrawTile(230, 200, img_beards, 0, 1, 1, haircol[mii.facialHairColor], ((mii.beardType-1)*8)+mii.faceShape);
	
	GRRLIB_SetHandle(img_mole, 6, 6);
	if(mii.mole==1) GRRLIB_DrawImg(252+(4*mii.moleHorizPos), 212+(2.8*mii.moleVertPos), img_mole, 0, .2+(mii.moleSize*0.1), .2+(mii.moleSize*0.1), 0xFFFFFFFF);

	// ********** RANDOM BLINKER ************
	blink++;										// each pass increment blink by 1
	if(blink>300+100*blinkrand) startblink=1;		// when blink gets to 300 plus a random blink value from 1 to 8 start blink
	if(startblink==1) blinkcount++;				// if startblink is 1, start counting blinkcount
	if(blinkcount>11) {								// when blink count gets to 11
		srand(time(0));
		blinkrand=rand() % 8;						// create a new random blink value to add to 300 (ensure random blink pattern)
		blink=0;									// reset the blink counter
		startblink=0;								// reset the startblink switch
		blinkcount=0;								// reset the blinkduration counter.
	}
	// **************************************
	
	if(blinkcount<11 && blinkcount>1 && miiblinks==1) { //if blinking (and time is right for a blink) display closed eye
		GRRLIB_SetHandle(img_eyes1, 18,36);
		GRRLIB_DrawTile(272+(2.6*mii.eyeHorizSpacing), 184+(2.8*(mii.eyeVertPos)), img_eyes1, 11.25 * (7-mii.eyeRotation), .3+(mii.eyeSize*0.1), .3+(mii.eyeSize*0.1), 0xFFFFFFFF, 46); 
		GRRLIB_SetHandle(img_eyes2, 18,36);
		GRRLIB_DrawTile(272+(2.6*mii.eyeHorizSpacing), 184+(2.8*(mii.eyeVertPos)), img_eyes2, 11.25 * (7-mii.eyeRotation), .3+(mii.eyeSize*0.1), .3+(mii.eyeSize*0.1), eyecol[mii.eyeColor], 46); 
		GRRLIB_SetHandle(img_eyes3, 18,36);
		GRRLIB_DrawTile(272+(2.6*mii.eyeHorizSpacing), 184+(2.8*(mii.eyeVertPos)), img_eyes3, 11.25 * (7-mii.eyeRotation), .3+(mii.eyeSize*0.1), .3+(mii.eyeSize*0.1), 0xFFFFFFFF, 46); 
		GRRLIB_SetHandle(img_eyes1, 36,36);
		GRRLIB_DrawTile(254-(2.6*mii.eyeHorizSpacing), 184+(2.8*(mii.eyeVertPos)), img_eyes1, -11.25 * (7-mii.eyeRotation), .3+(mii.eyeSize*0.1), .3+(mii.eyeSize*0.1), 0xFFFFFFFF, 95-46); 
		GRRLIB_SetHandle(img_eyes2, 36,36);
		GRRLIB_DrawTile(254-(2.6*mii.eyeHorizSpacing), 184+(2.8*(mii.eyeVertPos)), img_eyes2, -11.25 * (7-mii.eyeRotation), .3+(mii.eyeSize*0.1), .3+(mii.eyeSize*0.1), eyecol[mii.eyeColor], 95-46); 
		GRRLIB_SetHandle(img_eyes3, 36,36);
		GRRLIB_DrawTile(254-(2.6*mii.eyeHorizSpacing), 184+(2.8*(mii.eyeVertPos)), img_eyes3, -11.25 * (7-mii.eyeRotation), .3+(mii.eyeSize*0.1), .3+(mii.eyeSize*0.1), 0xFFFFFFFF, 95-46);
	}
	else { // display open eye
		if(miieyes==0) { // if miieyes is 0 draw default open eye.
			GRRLIB_SetHandle(img_eyes1, 18,36);
			GRRLIB_DrawTile(272+(2.6*mii.eyeHorizSpacing), 184+(2.8*(mii.eyeVertPos)), img_eyes1, 11.25 * (7-mii.eyeRotation), .3+(mii.eyeSize*0.1), .3+(mii.eyeSize*0.1), 0xFFFFFFFF, eyes[mii.eyeType]); 
			GRRLIB_SetHandle(img_eyes2, 18,36);
			GRRLIB_DrawTile(272+(2.6*mii.eyeHorizSpacing), 184+(2.8*(mii.eyeVertPos)), img_eyes2, 11.25 * (7-mii.eyeRotation), .3+(mii.eyeSize*0.1), .3+(mii.eyeSize*0.1), eyecol[mii.eyeColor], eyes[mii.eyeType]); 
			GRRLIB_SetHandle(img_eyes3, 18,36);
			GRRLIB_DrawTile(272+(2.6*mii.eyeHorizSpacing), 184+(2.8*(mii.eyeVertPos)), img_eyes3, 11.25 * (7-mii.eyeRotation), .3+(mii.eyeSize*0.1), .3+(mii.eyeSize*0.1), 0xFFFFFFFF, eyes[mii.eyeType]); 
			GRRLIB_SetHandle(img_eyes1, 36,36);
			GRRLIB_DrawTile(254-(2.6*mii.eyeHorizSpacing), 184+(2.8*(mii.eyeVertPos)), img_eyes1, -11.25 * (7-mii.eyeRotation), .3+(mii.eyeSize*0.1), .3+(mii.eyeSize*0.1), 0xFFFFFFFF, 95-eyes[mii.eyeType]); 
			GRRLIB_SetHandle(img_eyes2, 36,36);
			GRRLIB_DrawTile(254-(2.6*mii.eyeHorizSpacing), 184+(2.8*(mii.eyeVertPos)), img_eyes2, -11.25 * (7-mii.eyeRotation), .3+(mii.eyeSize*0.1), .3+(mii.eyeSize*0.1), eyecol[mii.eyeColor], 95-eyes[mii.eyeType]); 
			GRRLIB_SetHandle(img_eyes3, 36,36);
			GRRLIB_DrawTile(254-(2.6*mii.eyeHorizSpacing), 184+(2.8*(mii.eyeVertPos)), img_eyes3, -11.25 * (7-mii.eyeRotation), .3+(mii.eyeSize*0.1), .3+(mii.eyeSize*0.1), 0xFFFFFFFF, 95-eyes[mii.eyeType]);
		}
		else { // draw custom open eye.
			GRRLIB_SetHandle(img_eyes1, 18,36);
			GRRLIB_DrawTile(272+(2.6*mii.eyeHorizSpacing), 184+(2.8*(mii.eyeVertPos)), img_eyes1, 11.25 * (7-mii.eyeRotation), .3+(mii.eyeSize*0.1), .3+(mii.eyeSize*0.1), 0xFFFFFFFF, miieyes-1); 
			GRRLIB_SetHandle(img_eyes2, 18,36);
			GRRLIB_DrawTile(272+(2.6*mii.eyeHorizSpacing), 184+(2.8*(mii.eyeVertPos)), img_eyes2, 11.25 * (7-mii.eyeRotation), .3+(mii.eyeSize*0.1), .3+(mii.eyeSize*0.1), eyecol[mii.eyeColor], miieyes-1); 
			GRRLIB_SetHandle(img_eyes3, 18,36);
			GRRLIB_DrawTile(272+(2.6*mii.eyeHorizSpacing), 184+(2.8*(mii.eyeVertPos)), img_eyes3, 11.25 * (7-mii.eyeRotation), .3+(mii.eyeSize*0.1), .3+(mii.eyeSize*0.1), 0xFFFFFFFF, miieyes-1); 
			GRRLIB_SetHandle(img_eyes1, 36,36);
			GRRLIB_DrawTile(254-(2.6*mii.eyeHorizSpacing), 184+(2.8*(mii.eyeVertPos)), img_eyes1, -11.25 * (7-mii.eyeRotation), .3+(mii.eyeSize*0.1), .3+(mii.eyeSize*0.1), 0xFFFFFFFF, 95-(miieyes-1)); 
			GRRLIB_SetHandle(img_eyes2, 36,36);
			GRRLIB_DrawTile(254-(2.6*mii.eyeHorizSpacing), 184+(2.8*(mii.eyeVertPos)), img_eyes2, -11.25 * (7-mii.eyeRotation), .3+(mii.eyeSize*0.1), .3+(mii.eyeSize*0.1), eyecol[mii.eyeColor], 95-(miieyes-1)); 
			GRRLIB_SetHandle(img_eyes3, 36,36);
			GRRLIB_DrawTile(254-(2.6*mii.eyeHorizSpacing), 184+(2.8*(mii.eyeVertPos)), img_eyes3, -11.25 * (7-mii.eyeRotation), .3+(mii.eyeSize*0.1), .3+(mii.eyeSize*0.1), 0xFFFFFFFF, 95-(miieyes-1));
		}
	}
	
	GRRLIB_SetHandle(img_eyebrows, 0, 54);
	GRRLIB_DrawTile(288+(2.6*mii.eyebrowHorizSpacing), 166+(2.8*(mii.eyebrowVertPos-3)), img_eyebrows, 11.25 * (11-mii.eyebrowRotation), .3+(mii.eyebrowSize*0.1), .3+(mii.eyebrowSize*0.1), haircol[mii.eyebrowColor], eyebrows[mii.eyebrowType]);
	GRRLIB_SetHandle(img_eyebrows, 36, 54);
	GRRLIB_DrawTile(256-(2.6*mii.eyebrowHorizSpacing), 166+(2.8*(mii.eyebrowVertPos-3)), img_eyebrows, -11.25 * (11-mii.eyebrowRotation), .3+(mii.eyebrowSize*0.1), .3+(mii.eyebrowSize*0.1), haircol[mii.eyebrowColor], 53-eyebrows[mii.eyebrowType]);
	
	GRRLIB_SetHandle(img_lips, 30, 30);
	if(miimouth==0) { //if mii mouth = 0 draw default mouth.
		if(lips[mii.lipType]==1 || lips[mii.lipType]==6 || lips[mii.lipType]==11 || lips[mii.lipType]==17 || lips[mii.lipType]==19) {
			GRRLIB_DrawTile(260, 220+(2.6*mii.lipVertPos), img_lips, 0, .2+(mii.lipSize*0.1), .2+(mii.lipSize*0.1), lipcol[mii.lipColor], lips[mii.lipType]);
		}
		else {
			GRRLIB_DrawTile(260, 220+(2.6*mii.lipVertPos), img_lips, 0, .2+(mii.lipSize*0.1), .2+(mii.lipSize*0.1), 0XFFFFFFFF, lips[mii.lipType]);
		}
	}
	else { // draw requested mouth (miimouth)
		if(miimouth-1==1 || miimouth-1==6 || miimouth-1==11 || miimouth-1==17 || miimouth-1==19) {
			GRRLIB_DrawTile(260, 220+(2.6*mii.lipVertPos), img_lips, 0, .2+(mii.lipSize*0.1), .2+(mii.lipSize*0.1), 0xC76C46FF, miimouth-1);
		}
		else {
			GRRLIB_DrawTile(260, 220+(2.6*mii.lipVertPos), img_lips, 0, .2+(mii.lipSize*0.1), .2+(mii.lipSize*0.1), 0XFFFFFFFF, miimouth-1);
		}
	}
	
	GRRLIB_SetHandle(img_mustache, 30, 10);
	if(mii.mustacheType>0) GRRLIB_DrawTile(260, 244+(2.9*mii.mustacheVertPos), img_mustache, 0, .2+(mii.mustacheSize*0.1), .2+(mii.mustacheSize*0.1), haircol[mii.facialHairColor], mii.mustacheType-1);
	
	GRRLIB_SetHandle(img_noses, 25, 30);
	GRRLIB_DrawTile(265, 220+(2.6*mii.noseVertPos), img_noses, 0, .2+(mii.noseSize*0.1), .2+(mii.noseSize*0.1), skincol[mii.skinColor], noses[mii.noseType]);
		
	if(hairfg[mii.hairType]<56) {
		GRRLIB_DrawTile(230, 190, img_hairs1, 0, -1*(2*mii.hairPart-1), 1, haircol[mii.hairColor], hairfg[mii.hairType]);
	}
	else {
		GRRLIB_DrawTile(230, 190, img_hairs2, 0, -1*(2*mii.hairPart-1), 1, haircol[mii.hairColor], hairfg[mii.hairType]-56);
	}

	if(mii.glassesType > 0) {
		GRRLIB_SetHandle(img_glasses, 90, 32);
		if(mii.glassesType < 6) {
			GRRLIB_DrawTile(200, 195+(2.6*(mii.glassesVertPos)), img_glasses, 0, .1+(mii.glassesSize*0.1), .1+(mii.glassesSize*0.1), glassescol[mii.glassesColor], mii.glassesType-1);
		}
		else {
			GRRLIB_DrawTile(200, 195+(2.6*(mii.glassesVertPos)), img_glasses, 0, .1+(mii.glassesSize*0.1), .1+(mii.glassesSize*0.1), glassescol[mii.glassesColor], mii.glassesType+2);
			GRRLIB_DrawTile(200, 195+(2.6*(mii.glassesVertPos)), img_glasses, 0, .1+(mii.glassesSize*0.1), .1+(mii.glassesSize*0.1), 0xFFFFFFFF, mii.glassesType-1);
		}
	}
	GRRLIB_CompoEnd(200, 160, tex);  // finish the composition and store it as whatever texture was passed to GRRLIB_Build Mii
}

void GRRLIB_CreateStaticMiis() {  // Create up to 100 empty textures and store each static mii into them.
	u8 i;
	for(i=0; i<NoOfMiis; i++) {
		img_staticmii[i] = GRRLIB_CreateEmptyTexture(180, 200);
		GRRLIB_BuildMii(miis[i], 0, 0, 0, img_staticmii[i]);
	}
}

void drawthemiis() {  // Draws the menu that displays all the static mii's for you to pick one.
	int themiis=1;
	int x=-150;
	int y=-150;
	int i=0;
	int j=0;
	int n=0;
	while(themiis==1) {
		WPAD_SetVRes(0, 640+100, 480+100);
		WPAD_ScanPads();
		wpaddown = WPAD_ButtonsDown(0);
		WPAD_IR(WPAD_CHAN_0, &ir1);
	
		for(i=0;i<15;i++) {																									// draw 16 per page
			n=j*15+i;																											// mii to draw is page number * 15 + i position.
			GRRLIB_Rectangle(90+(i%5*100), 70+((i/5-(i%5)/5)*105), 90, 95, 0x4982B299, true); 								// draw a blue background rect for each mii
			GRRLIB_Rectangle(x,y,90,95,0x00FF00FF,false);    																	// draw a green box around the selected mii
			GRRLIB_Rectangle(x+1,y+1,88,93,0x00FF00FF,false);	  																// draw another green box around first green box to make it thicker
			if(n<NoOfMiis) GRRLIB_DrawImg(80+(i%5*100), 60+((i/5-(i%5)/5)*105), img_staticmii[n], 0, .6, .6, 0xFFFFFFFF);  	// draw the mii
		}
		
		GRRLIB_DrawImg(ir1.x-40, ir1.y-40, img_pointer, ir1.angle, 1, 1, 0xFFFFFFFF);  	// draw the pointer
		GRRLIB_Render();																	// render it
		
		for(i=0;i<15;i++) {			// now draw all the selection bounds that determines what mii you are on when you click
			if(GRRLIB_PtInRect(90+(i%5*100), 70+((i/5-(i%5)/5)*105), 90, 95, ir1.x-30, ir1.y-40)) {
				x=90+(i%5*100);
				y=70+((i/5-(i%5)/5)*105);
				if(i+15*j<NoOfMiis) {
					if(wpaddown & WPAD_BUTTON_A) {		// when you click A
						selectedmii = i+15*j;			// set selectedmii to the correct mii number (page number*15+i)
						themiis=0;						// leave the menu
					}
				}
			}
		}
		
		if(wpaddown & WPAD_BUTTON_B) {
			themiis=0;									// leave the menu
		}
		
		if(((wpaddown & WPAD_BUTTON_RIGHT) || (wpaddown & WPAD_BUTTON_PLUS)) && (j<6)) {
			j++;										// next page
		}
		if(((wpaddown & WPAD_BUTTON_LEFT) || (wpaddown & WPAD_BUTTON_MINUS)) && (j>0)) {
			j--;										// previous page
		}	
		if(wpaddown & WPAD_BUTTON_HOME) {
			exit(0);									// exit the program
		}
	}	
}

void initme() {

	GRRLIB_Init();

	GRRLIB_MiisInit();
	
	GRRLIB_CreateStaticMiis();
	
	img_pointer = GRRLIB_LoadTexture(pointer_png);
	img_mii = GRRLIB_CreateEmptyTexture(180, 200);
	
	bankgothic16 = new FreeTypeGX();
	bankgothic16->loadFont(bankgothic_med_ttf, bankgothic_med_ttf_size, 16);
	
	WPAD_Init();
	WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
	
}

int main() {
	initme();
	
	int miieyes=0;
	int miimouth=0;
    while(1) {
		WPAD_SetVRes(0, 640+100, 480+100);
		WPAD_ScanPads();
		wpaddown = WPAD_ButtonsDown(0);
		WPAD_IR(WPAD_CHAN_0, &ir1);
		
		GRRLIB_BuildMii(miis[selectedmii], 1, miieyes, miimouth, img_mii);  		// update our img_mii file with the correct mii and expression by sending it to the BuildMii function with approp variables
		
		GRRLIB_FillScreen(0x0000FFFF);												// blue screen
		
		GRRLIB_DrawImg(220, 100, img_mii, 0, 1, 1, 0xFFFFFFFF);					// draw updated img_mii
		
		printf_ttf(bankgothic16, 80, 300, 2, (GXColor){255,255,255,255}, (GXColor){0,0,0,255}, FTGX_JUSTIFY_LEFT, "RANDOM HAPPY");
		printf_ttf(bankgothic16, 280, 300, 2, (GXColor){255,255,255,255}, (GXColor){0,0,0,255}, FTGX_JUSTIFY_LEFT, "DEFAULT");
		printf_ttf(bankgothic16, 480, 300, 2, (GXColor){255,255,255,255}, (GXColor){0,0,0,255}, FTGX_JUSTIFY_LEFT, "RANDOM SAD");
		
		printf_ttf(bankgothic16, 280, 400, 2, (GXColor){255,255,255,255}, (GXColor){0,0,0,255}, FTGX_JUSTIFY_LEFT, "MII MENU");
		
		GRRLIB_DrawImg(ir1.x-40, ir1.y-40, img_pointer, ir1.angle, 1, 1, 0xFFFFFFFF);
		
		GRRLIB_Render();

		if(GRRLIB_PtInRect(80, 280, 150, 20, ir1.x-30, ir1.y-40)) {  //if click on Random Happy
			if((wpaddown & WPAD_BUTTON_A)) {
				srand(time(0));										// seed the random number
				int rhap = rand() % 7 + 1;							// set a random number
				if(rhap==1) miimouth=3;							// pick one of the happy mouth tiles
				else if(rhap==2) miimouth=4;
				else if(rhap==3) miimouth=5;
				else if(rhap==4) miimouth=16;
				else if(rhap==5) miimouth=19;
				else miimouth=23;
			}
		}
		
		if(GRRLIB_PtInRect(280, 280, 90, 20, ir1.x-30, ir1.y-40)) {  //if click on default
			if((wpaddown & WPAD_BUTTON_A)) {
				miieyes=0;							// set to draw default eyes
				miimouth=0;							// set to draw default mouth
			}
		}
		
		if(GRRLIB_PtInRect(480, 280, 120, 20, ir1.x-30, ir1.y-40)) {  //if click on random sad
			if((wpaddown & WPAD_BUTTON_A)) {
				srand(time(0));										// seed the random number
				int rsad = rand() % 4 + 1;							// set a random number
				if(rsad==1) miimouth=9;							// pick one of the sad mouth tiles
				else if(rsad==2) miimouth=22;
				else miimouth=24;
			}
		}
		
		if(GRRLIB_PtInRect(280, 380, 90, 20, ir1.x-30, ir1.y-40)) {  	//if click on menu
			if((wpaddown & WPAD_BUTTON_A)) {
				drawthemiis();											// draw menu
			}
		}

		if((wpaddown & WPAD_BUTTON_HOME)) {
			exit(0);													// exit
		}
		
    }
	
    return 0;
}
