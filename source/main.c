//
// main.c
// Load test tilemap sevenElevenExt.png
//

#include <string.h>
#include <tonc.h>
#include "sevenElevenExt.h"
#include "mainchar.h"

OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer= (OBJ_AFFINE*)obj_buffer;

void gameTest() {
	int x= 96, y= 32;
	u32 tid= 0, pb= 0;		// tile id, pal-bank

	OBJ_ATTR *metr= &obj_buffer[0];
	obj_set_attr(metr,
		ATTR0_SQUARE,				// Square, regular sprite
		ATTR1_SIZE_64,					// 64x64p,
		ATTR2_PALBANK(pb) | tid);		// palbank 0, tile 0

	// position sprite (redundant here; the _real_ position
	// is set further down
	obj_set_pos(metr, x, y);

	while(1)
	{
		vid_vsync();
		key_poll();

		// move left/right
		x += 2*key_tri_horz();

		// move up/down
		y += 2*key_tri_vert();

		// increment/decrement starting tile with R/L
		tid += bit_tribool(key_hit(-1), KI_R, KI_L);

		// flip
		if(key_hit(KEY_A))	// horizontally
			metr->attr1 ^= ATTR1_HFLIP;
		if(key_hit(KEY_B))	// vertically
			metr->attr1 ^= ATTR1_VFLIP;

		// make it glow (via palette swapping)
		pb= key_is_down(KEY_SELECT) ? 1 : 0;

		// toggle mapping mode
		if(key_hit(KEY_START))
			REG_DISPCNT ^= DCNT_OBJ_1D;

		// Hey look, it's one of them build macros!
		metr->attr2= ATTR2_BUILD(tid, pb, 0);
		obj_set_pos(metr, x, y);

		oam_copy(oam_mem, obj_buffer, 1);	// only need to update one
	}
}

int main() {

	/*Background setup*/

	// Load palette
	memcpy(pal_bg_mem, sevenElevenExtPal, sevenElevenExtPalLen);
	// Load tiles into CBB 0
	memcpy(&tile_mem[0][0], sevenElevenExtTiles, sevenElevenExtTilesLen);
	// Load map into SBB 30
	memcpy(&se_mem[28][0], sevenElevenExtMap, sevenElevenExtMapLen);

	// set up BG0 for a 8bpp 64x64t map, using
	//   using charblock 0 and screenblock 31
	REG_BG0CNT= BG_CBB(0) | BG_SBB(28) | BG_8BPP | BG_REG_64x64;
	REG_DISPCNT= DCNT_MODE0 | DCNT_BG0;

	/*Sprite setup*/

	memcpy(&tile_mem[4][0], maincharTiles, maincharTilesLen);
	memcpy(pal_obj_mem, maincharPal, maincharPalLen);

	oam_init(obj_buffer, 128);
	REG_DISPCNT= DCNT_OBJ | DCNT_OBJ_1D;

	// Scroll around some
	int x= 0, y= 0;
	while(1) {
		vid_vsync();
		key_poll();

		x += key_tri_horz();
		y += key_tri_vert();

		REG_BG0HOFS= x;
		REG_BG0VOFS= y;
	}

	return 0;
}
