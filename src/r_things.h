// SONIC ROBO BLAST 2
//-----------------------------------------------------------------------------
// Copyright (C) 1993-1996 by id Software, Inc.
// Copyright (C) 1998-2000 by DooM Legacy Team.
// Copyright (C) 1999-2019 by Sonic Team Junior.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  r_things.h
/// \brief Rendering of moving objects, sprites

#ifndef __R_THINGS__
#define __R_THINGS__

#include "sounds.h"
#include "r_plane.h"
#include "r_patch.h"
#include "r_portal.h"
#include "r_defs.h"

// "Left" and "Right" character symbols for additional rotation functionality
#define ROT_L ('L' - '0')
#define ROT_R ('R' - '0')

// number of sprite lumps for spritewidth,offset,topoffset lookup tables
// Fab: this is a hack : should allocate the lookup tables per sprite
#define MAXVISSPRITES 2048 // added 2-2-98 was 128

#define VISSPRITECHUNKBITS 6	// 2^6 = 64 sprites per chunk
#define VISSPRITESPERCHUNK (1 << VISSPRITECHUNKBITS)
#define VISSPRITEINDEXMASK (VISSPRITESPERCHUNK - 1)

#define FEETADJUST (4<<FRACBITS) // R_AddSingleSpriteDef

// Constant arrays used for psprite clipping
//  and initializing clipping.
extern INT16 negonearray[MAXVIDWIDTH];
extern INT16 screenheightarray[MAXVIDWIDTH];

// vars for R_DrawMaskedColumn
extern INT16 *mfloorclip;
extern INT16 *mceilingclip;
extern fixed_t spryscale;
extern fixed_t sprtopscreen;
extern fixed_t sprbotscreen;
extern fixed_t windowtop;
extern fixed_t windowbottom;

void R_DrawMaskedColumn(column_t *column);
void R_DrawFlippedMaskedColumn(column_t *column, INT32 texheight);

//faB: find sprites in wadfile, replace existing, add new ones
//     (only sprites from namelist are added or replaced)
void R_AddSpriteDefs(UINT16 wadnum);

//SoM: 6/5/2000: Light sprites correctly!
void R_AddSprites(sector_t *sec, INT32 lightlevel);
void R_InitSprites(void);
void R_ClearSprites(void);
void R_ClipSprites(drawseg_t* dsstart, portal_t* portal);

boolean R_ThingVisible (mobj_t *thing);

boolean R_ThingVisibleWithinDist (mobj_t *thing,
		fixed_t        draw_dist,
		fixed_t nights_draw_dist);

boolean R_PrecipThingVisible (precipmobj_t *precipthing,
		fixed_t precip_draw_dist);

/** Used to count the amount of masked elements
 * per portal to later group them in separate
 * drawnode lists.
 */
typedef struct
{
	size_t drawsegs[2];
	size_t vissprites[2];
	fixed_t viewx, viewy, viewz;			/**< View z stored at the time of the BSP traversal for the view/portal. Masked sorting/drawing needs it. */
	sector_t* viewsector;
} maskcount_t;

void R_DrawMasked(maskcount_t* masks, UINT8 nummasks);

// -----------
// SKINS STUFF
// -----------
#define SKINNAMESIZE 16
// should be all lowercase!! S_SKIN processing does a strlwr
#define DEFAULTSKIN "sonic"
#define DEFAULTSKIN2 "tails" // secondary player
#define DEFAULTNIGHTSSKIN 0

typedef struct
{
	char name[SKINNAMESIZE+1]; // INT16 descriptive name of the skin
	UINT16 wadnum;
	skinflags_t flags;

	char realname[SKINNAMESIZE+1]; // Display name for level completion.
	char hudname[SKINNAMESIZE+1]; // HUD name to display (officially exactly 5 characters long)

	UINT8 ability; // ability definition
	UINT8 ability2; // secondary ability definition
	INT32 thokitem;
	INT32 spinitem;
	INT32 revitem;
	INT32 followitem;
	fixed_t actionspd;
	fixed_t mindash;
	fixed_t maxdash;

	fixed_t normalspeed; // Normal ground
	fixed_t runspeed; // Speed that you break into your run animation

	UINT8 thrustfactor; // Thrust = thrustfactor * acceleration
	UINT8 accelstart; // Acceleration if speed = 0
	UINT8 acceleration; // Acceleration

	fixed_t jumpfactor; // multiple of standard jump height

	fixed_t radius; // Bounding box changes.
	fixed_t height;
	fixed_t spinheight;

	fixed_t shieldscale; // no change to bounding box, but helps set the shield's sprite size
	fixed_t camerascale;

	// Definable color translation table
	UINT8 starttranscolor;
	UINT8 prefcolor;
	UINT8 supercolor;
	UINT8 prefoppositecolor; // if 0 use tables instead

	fixed_t highresscale; // scale of highres, default is 0.5
	UINT8 contspeed; // continue screen animation speed
	UINT8 contangle; // initial angle on continue screen

	// specific sounds per skin
	sfxenum_t soundsid[NUMSKINSOUNDS]; // sound # in S_sfx table

	// contains super versions too
	spritedef_t sprites[NUMPLAYERSPRITES*2];
	spriteinfo_t sprinfo[NUMPLAYERSPRITES*2];

	UINT8 availability; // lock?
} skin_t;

// -----------
// NOT SKINS STUFF !
// -----------
typedef enum
{
	// actual cuts
	SC_NONE = 0,
	SC_TOP = 1,
	SC_BOTTOM = 1<<1,
	// other flags
	SC_PRECIP = 1<<2,
	SC_LINKDRAW = 1<<3,
	SC_FULLBRIGHT = 1<<4,
	SC_VFLIP = 1<<5,
	SC_ISSCALED = 1>>6,
	// masks
	SC_CUTMASK = SC_TOP|SC_BOTTOM,
	SC_FLAGMASK = ~SC_CUTMASK
} spritecut_e;

// A vissprite_t is a thing that will be drawn during a refresh,
// i.e. a sprite object that is partly visible.
typedef struct vissprite_s
{
	// Doubly linked list.
	struct vissprite_s *prev;
	struct vissprite_s *next;

	// Bonus linkdraw pointer.
	struct vissprite_s *linkdraw;

	mobj_t *mobj; // for easy access

	INT32 x1, x2;

	fixed_t gx, gy; // for line side calculation
	fixed_t gz, gzt; // global bottom/top for silhouette clipping
	fixed_t pz, pzt; // physical bottom/top for sorting with 3D floors

	fixed_t startfrac; // horizontal position of x1
	fixed_t scale, sortscale; // sortscale only differs from scale for paper sprites and MF2_LINKDRAW
	fixed_t scalestep; // only for paper sprites, 0 otherwise
	fixed_t paperoffset, paperdistance; // for paper sprites, offset/dist relative to the angle
	fixed_t xiscale; // negative if flipped

	angle_t centerangle; // for paper sprites

	fixed_t texturemid;
	patch_t *patch;

	lighttable_t *colormap; // for color translation and shadow draw
	                        // maxbright frames as well

	UINT8 *transmap; // for MF2_SHADOW sprites, which translucency table to use

	INT32 mobjflags;

	INT32 heightsec; // height sector for underwater/fake ceiling support

	extracolormap_t *extra_colormap; // global colormaps

	fixed_t xscale;

	// Precalculated top and bottom screen coords for the sprite.
	fixed_t thingheight; // The actual height of the thing (for 3D floors)
	sector_t *sector; // The sector containing the thing.
	INT16 sz, szt;

	spritecut_e cut;

	INT16 clipbot[MAXVIDWIDTH], cliptop[MAXVIDWIDTH];

	INT32 dispoffset; // copy of info->dispoffset, affects ordering but not drawing
} vissprite_t;

// A drawnode is something that points to a 3D floor, 3D side, or masked
// middle texture. This is used for sorting with sprites.
typedef struct drawnode_s
{
	visplane_t *plane;
	drawseg_t *seg;
	drawseg_t *thickseg;
	ffloor_t *ffloor;
	vissprite_t *sprite;

	struct drawnode_s *next;
	struct drawnode_s *prev;
} drawnode_t;

extern INT32 numskins;
extern skin_t skins[MAXSKINS];
extern UINT32 visspritecount;

void SetPlayerSkin(INT32 playernum,const char *skinname);
void SetPlayerSkinByNum(INT32 playernum,INT32 skinnum); // Tails 03-16-2002
boolean R_SkinUsable(INT32 playernum, INT32 skinnum);
UINT32 R_GetSkinAvailabilities(void);
INT32 R_SkinAvailable(const char *name);
void R_PatchSkins(UINT16 wadnum);
void R_AddSkins(UINT16 wadnum);

UINT8 P_GetSkinSprite2(skin_t *skin, UINT8 spr2, player_t *player);

void R_InitDrawNodes(void);

char *GetPlayerFacePic(INT32 skinnum);

// Functions to go from sprite character ID to frame number
// for 2.1 compatibility this still uses the old 'A' + frame code
// The use of symbols tends to be painful for wad editors though
// So the future version of this tries to avoid using symbols
// as much as possible while also defining all 64 slots in a sane manner
// 2.1:    [[ ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~   ]]
// Future: [[ ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyz!@ ]]
FUNCMATH FUNCINLINE static ATTRINLINE char R_Frame2Char(UINT8 frame)
{
#if 0 // 2.1 compat
	return 'A' + frame;
#else
	if (frame < 26) return 'A' + frame;
	if (frame < 36) return '0' + (frame - 26);
	if (frame < 62) return 'a' + (frame - 36);
	if (frame == 62) return '!';
	if (frame == 63) return '@';
	return '\xFF';
#endif
}

FUNCMATH FUNCINLINE static ATTRINLINE UINT8 R_Char2Frame(char cn)
{
#if 0 // 2.1 compat
	if (cn == '+') return '\\' - 'A'; // PK3 can't use backslash, so use + instead
	return cn - 'A';
#else
	if (cn >= 'A' && cn <= 'Z') return cn - 'A';
	if (cn >= '0' && cn <= '9') return (cn - '0') + 26;
	if (cn >= 'a' && cn <= 'z') return (cn - 'a') + 36;
	if (cn == '!') return 62;
	if (cn == '@') return 63;
	return 255;
#endif
}

FUNCMATH FUNCINLINE static ATTRINLINE boolean R_ValidSpriteAngle(UINT8 rotation)
{
	return ((rotation <= 8) || (rotation == ROT_L) || (rotation == ROT_R));
}

#endif //__R_THINGS__
