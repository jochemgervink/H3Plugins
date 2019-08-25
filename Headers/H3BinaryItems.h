//////////////////////////////////////////////////////////////////////
//                                                                  //
//                  Created by RoseKavalier:                        //
//                  rosekavalierhc@gmail.com                        //
//                                                                  //
//          ***You may use or distribute these files freely         //
//             so long as this notice remains present.***           //
//                                                                  //
//          In preparing these files, several sources were          //
//            consulted, they are listed in no particular           //
//                          order below.                            //
//                                                                  //
//  Source: https://github.com/potmdehex/homm3tools                 //
//  Author: potmdehex and contributors                              //
//                                                                  //
//  Source: https://github.com/redxu/HoMM3_FA                       //
//  Author: redxu                                                   //
//                                                                  //
//  Source: https://github.com/openhomm/openhomm                    //
//  Author: Ershov Stanislav, Fomin Roman, Akulich Alexander        //
//                                                                  //
//  Source: https://github.com/GrayFace/wog                         //
//  Author: GrayFace and WoG team                                   //
//                                                                  //
//  Source: https://github.com/ethernidee/era                       //
//  Author: Berserker                                               //
//                                                                  //
//  Source: https://github.com/ethernidee/era-editor                //
//  Author: Grayface and Berserker                                  //
//                                                                  //
//  Source: http://wforum.heroes35.net/showthread.php?tid=3900      //
//  Author: Slava and GrayFace                                      //
//                                                                  //
//  Source: http://wforum.heroes35.net/showthread.php?tid=4583      //
//  Author: gamecreator                                             //
//                                                                  //
//  Thanks: patcher_x86 by baratorch and code therein               //
//                                                                  //
//////////////////////////////////////////////////////////////////////

#ifndef _H3BINARYITEMS_H_
#define _H3BINARYITEMS_H_

#pragma once

#include "H3Base.h"

typedef UINT16 RGB555;
typedef UINT16 RGB565;

// BINARY TREE ITEMS ~ named _BaseFileItem_ in database
struct H3BinaryItem;
struct H3BinaryTreeNode;
struct H3BinTreeList;
struct H3WavFile;
struct H3ColumnTextFile;
struct H3TextFile;
struct H3RGB888;
struct H3RGB555;
struct H3RGB565;
struct H3Palette565;
struct H3Font;
struct H3Palette888;
struct H3LoadedPCX;
struct H3LoadedPCX16;
struct H3DefFrame;
struct H3DefGroup;
struct H3LoadedDEF;

#pragma pack(push, 1)


// from WoG binTree is 0x69E560, _BinTreeList_ BinTreeHead in database
//#define SOD_BinTree_FindItem(TreeRoot,Name) (((int (__fastcall *)(Dword,int,char *))0x55EE00)(TreeRoot,0,Name))
//#define SOD_BinTree_RemoveItem(TreeRoot,Item_po,buf_po) (((int (__fastcall *)(Dword,int,Dword,Dword *))0x55DF20)(TreeRoot,0,Item_po,buf_po))
struct H3BinTreeList
{
	h3unk _f0[4];
	// * +4
	PVOID postLastItem;
	h3unk _f8[4];
	INT32 count;

	UINT FindItem(LPCSTR name) { return THISCALL_2(UINT, 0x55EE00, this, name); }
};

struct H3BinaryTreeNode
{
	// * +0
	H3BinaryTreeNode *leftNode;
	// * +4
	H3BinaryTreeNode *parent;
	// * +8
	H3BinaryTreeNode *rightNode;
	// * +C
	CHAR name[12];
	h3unk _f_18[4];
	// * +1C
	H3BinaryItem *item;
	BOOL isBlackNode; // https://en.wikipedia.org/wiki/Red�black_tree
};

struct H3BinaryItem // constructor 0x558970, size 1C
{
protected:
	// * +0
	struct BinaryVTable
	{
		h3func destroy;		// 0x5589C0, called by dereference
		h3func dereference; // 0x55D300
		h3func getSize;
	}*vTable;
	// * +4
	CHAR name[12];
	h3unk _f_10[4];
	// * +14
	INT32 type; // arg3 from constructor
	// * +18
	INT32 ref; // the number of times it is being used

	VOID Dereference() { THISCALL_1(VOID, vTable->dereference, this); }
};

///////////////////////////////////////////////////
// from breakpoint at constructor, the following items are all H3BinaryTreeNode subclasses

struct H3WavFile : public H3BinaryItem // size 0x34 from 0x55C67E
{
	h3unk _f_1C[12];
	// * +28 fields of _RTL_CRITICAL_SECTION
	HANDLE lockSemaphore;
	// * +2C
	ULONG_PTR spinCount;
	// * +30
	PRTL_CRITICAL_SECTION_DEBUG debugInfo;
};

// * text file with a single column of text
struct H3ColumnTextFile : public H3BinaryItem
{
protected:
	// * +1C list of rows holding text, single column
	H3Vector<LPCSTR> text;
public:
	inline LPCSTR GetText(INT32 row) { return text.first[row - 1]; } // using the index from TxtEdit
};

// * text file with a several columns of text
struct H3TextFile : public H3BinaryItem
{
protected:
	// * +1C list of rows holding lists of column text
	H3Vector<H3Vector<LPCSTR>*> text;
public:
	H3Vector<H3Vector<LPCSTR>*> & GetText() { return text; }
	INT32 CountRows() { return text.Count(); }
	static H3TextFile* Load(LPCSTR name) { return THISCALL_1(H3TextFile*, 0x55C2B0, name); }
	VOID UnLoad() { THISCALL_1(VOID, vTable->dereference, this); }
};

struct H3RGB888
{
	UINT8 r;
	UINT8 g;
	UINT8 b;
};

struct H3ARGB888
{
	UINT8 a; // alpha
	UINT8 r;
	UINT8 g;
	UINT8 b;
};

struct H3RGB555 // https://docs.microsoft.com/en-us/windows/desktop/DirectShow/working-with-16-bit-rgb
{
protected:
	WORD bits;
public:
	UINT8 GetRed() { return (bits & 0x7C00) >> 10; }
	UINT8 GetGreen() { return (bits & 0x3E0) >> 5; }
	UINT8 GetBlue() { return bits & 0x1F; }
	VOID PackRGB(UINT8 r, UINT8 g, UINT8 b) { bits = (r & 0x1F) << 10 | (g & 0x1F) << 5 | (b & 0x1F); }
	DWORD UnPack() { return GetRed() << 16 | GetGreen() << 8 | GetBlue(); }
};

struct H3RGB565 // https://docs.microsoft.com/en-us/windows/desktop/DirectShow/working-with-16-bit-rgb
{
protected:
	WORD bits;
public:
	UINT8 GetRed() { return (bits & 0xF800) >> 11; }
	UINT8 GetRed8() { return (GetRed() * 255 + 15) / 31; }
	UINT8 GetGreen() { return (bits & 0x7E0) >> 5; }
	UINT8 GetGreen8() { return (GetGreen() * 255 + 31) / 63; }
	UINT8 GetBlue() { return bits & 0x1F; }
	UINT8 GetBlue8() { return (GetBlue() * 255 + 15) / 31; }
	WORD PackRGB565(UINT8 r, UINT8 g, UINT8 b) { return bits = (r & 0x1F) << 11 | (g & 0x3F) << 5 | (b & 0x1F); }
	DWORD GetRGB888() { return (GetRed8() << 16 | GetGreen8() << 8 | GetBlue8()); }
};

#define Color565To888(rgb565) (((((rgb565 & 0xF800) >> 11) * 255 + 15) / 31) << 16 + ((((rgb565 & 0x7E0) >> 5) * 255 + 31) / 63) << 8 + (((rgb565 & 0x1F) * 255 + 15) / 31))

struct H3Palette565 : public H3BinaryItem
{
	// * +1C
	H3RGB565 color[256];

	VOID ColorToPlayer(INT id) { FASTCALL_2(VOID, 0x6003E0, color, id); }
};

struct H3Font : public H3BinaryItem
{
	h3unk _f_1C[32];
	// * +3C
	UINT8 width[3072]; // used to calculate line width
	// * +C3C
	UINT32 bufferOffsets[256]; // referenced at 0x4B4F1C
	// * +103C
	H3Palette565 palette;
	// * 1258
	PUINT8 bitmapBuffer; // how the characters look
	// * +125C
	INT32 bufferSize;

	INT32 GetLinesCountInText(LPCSTR str, INT32 width) { return THISCALL_3(INT32, 0x4B5580, this, str, width); }
	INT32 GetMaxLineWidth(LPCSTR str) { return THISCALL_2(INT32, 0x4B56F0, this, str); }
	INT32 GetMaxWordWidth(LPCSTR str) { return THISCALL_2(INT32, 0x4B5770, this, str); }
};

struct H3Palette888 : public H3BinaryItem
{
	// * +1C
	H3RGB888 color[256];

	VOID ColorToPlayer(INT id) { FASTCALL_2(VOID, 0x600400, this, id); }
};

struct H3LoadedPCX : public H3BinaryItem // size 0x56C // vt 63BA14
{
	// * +1C
	INT32 bufSize;
	// * +20
	INT32 bufSizeUnk;
	// * +24
	INT32 width;
	// * +28
	INT32 height;
	// * +2C
	INT32 scanlineSize;
	// * +30
	// * 256-color indexed buffer
	PUINT8 buffer;
	// * +34
	H3Palette565 palette565;
	// * +250
	H3Palette888 palette888;

	VOID DrawToPcx16(int srcX, int srcY, int dx, int dy, H3LoadedPCX16 *drawMan, int x, int y, int transparent)
	{
		THISCALL_9(VOID, 0x44FA80, this, srcX, srcY, dx, dy, drawMan, x, y, transparent);
	}

	static H3LoadedPCX* Load(LPCSTR name) { return THISCALL_1(H3LoadedPCX*, 0x55AA10, name); }

	// * returns row start in buffer
	PUINT8 GetRow(int row) { return buffer + row * scanlineSize; }
	// * returns the color index of pixel (x, y) starting from top left
	PUINT8 GetPixelIndex(int x, int y) {	return GetRow(y) + x; }
};

struct H3LoadedPCX16 : public H3BinaryItem // size 0x38 // vt 63B9C8
{
	// * +1C
	INT32 buffSize;
	// * +20
	INT32 buffSizeUnk;
	// * +24
	INT32 width;
	// * +28
	INT32 height;
	// * +2C
	INT32 scanlineSize;
	// * +30
	// * H3RGB565 buffer unless bitmode = 4
	// * in which case this is a H3ARGB888 buffer
	PUINT8 buffer;
	h3unk _f_34; // set to 0 by constructor
	h3unk _f_35[3];

	H3LoadedPCX16* Construct(LPCSTR name, INT width, INT height)
	{
		return THISCALL_4(H3LoadedPCX16*, 0x44DD20, this, name, width, height);
	}
	VOID DrawToPcx16(INT x, INT y, BOOL transparent, H3LoadedPCX16 *dest, INT srcX = 0, INT srcY = 0)
	{
		return THISCALL_12(VOID, 0x44DF80, this, srcX, srcY, width, height, dest->buffer, x,y, dest->width, dest->height, dest->scanlineSize, transparent);
	}
	static H3LoadedPCX16* Load(LPCSTR name) { return THISCALL_1(H3LoadedPCX16*, 0x55B1E0, name); }

	// * returns row start in buffer
	PUINT8 GetRow(int row) { return buffer + row * scanlineSize; }
	// * returns rgb565 pixel at coordinates (x,y) in buffer
	// * !WARNING! This is only valid when h3_BitMode == 2 aka default
	H3RGB565* GetPixel565(int x, int y) { return (H3RGB565*)GetRow(y) + x; };
	// * returns rgb565 pixel at coordinates (x,y) in buffer
	// * !WARNING! This is only valid when h3_BitMode == 4 aka TRUE mode with HDmod
	H3ARGB888* GetPixel888(int x, int y) { return (H3ARGB888*)GetRow(y) + x; };
};

struct H3LoadedPCX24 : public H3BinaryItem // size 0x30 // vt 63B9F4
{
	// * +1C
	INT32 buffSize;
	// * +20
	INT32 buffSizeUnk;
	// * +24
	INT32 width;
	// * +28
	INT32 height;
	// * +2C
	// * RGB888 buffer
	PUINT8 buffer;

	H3LoadedPCX24* Construct(LPCSTR name, INT width, INT height, H3Palette565 *palette, UINT32 bufferSize)
	{
		return THISCALL_6(H3LoadedPCX24*, 0x44EA20, this, name, width, height, palette, bufferSize);
	}
	// * converts RGB888 to RGB565
	VOID DrawToPcx16(INT x, INT y, H3LoadedPCX16 *dest, INT srcX = 0, INT srcY = 0)
	{
		return THISCALL_11(VOID, 0x44ECE0, this, srcX, srcY, width, height, buffer, x, y, dest->width, dest->height, dest->scanlineSize);
	}

	// * returns row start in buffer
	PUINT8 GetRow(int row) { return buffer + row * sizeof(H3RGB888) * width; }
	// * returns rgb888 pixel at coordinates (x, y) in buffer
	H3RGB888* GetPixel(int x, int y) { return (H3RGB888*)GetRow(y) + x; }
};

struct H3DefFrame : public H3BinaryItem
{
	// * +1C
	// * all row offsets and actual pixel data
	INT32 rawDataSize;
	// * +20
	// * frame width * frame height
	INT32 dataSize;
	// * +24
	INT32 compressionType;
	// * +28
	INT32 width;
	// * +2C
	INT32 height;
	// * +30
	INT32 frameWidth;
	// * +34
	INT32 frameHeight;
	// * +38
	// * marginLeft + frameWidth = width
	INT32 marginLeft;
	// * +3C
	INT32 marginTop;
	// * +40
	INT32 width2;
	// * +44
	// * pixel index and such.
	PUINT8 rawData;
};

// * Not a binary item but relevant to H3LoadedDEF and H3DefFrame
struct H3DefGroup
{
	// * +0
	int count;
	// * +4
	int spritesSize;
	// * +8
	H3DefFrame **frames;
};

struct H3LoadedDEF : public H3BinaryItem
{
	// * +1C
	H3DefGroup **groups;
	// * +20
	H3Palette565 *palette565;
	// * +24
	H3Palette888 *palette888;
	// * +28
	INT32 groupsCount;
	// * +2C
	INT32 *activeGroups;
	// * +30
	INT32 widthDEF;
	// * +34
	INT32 heightDEF;

	// * for creature animation groups
	enum CreatureGroups
	{
		CG_MOVING = 0,
		CG_MOUSE_OVER = 1,
		CG_STANDING = 2,
		CG_GETTING_HIT = 3,
		CG_DEFEND = 4,
		CG_DEATH = 5,
		CG_UNUSED_DEATH = 6,
		CG_TURN_LEFT = 7,
		CG_TURN_RIGHT = 8,
		CG_TURN_LEFT2 = 9,
		CG_TURN_RIGHT2 = 10,
		CG_ATTACK_UP = 11,
		CG_ATTACK_STRAIGHT = 12,
		CG_ATTACK_DOWN = 13,
		CG_SHOOT_UP = 14,
		CG_SHOOT_STRAIGHT = 15,
		CG_SHOOT_DOWN = 16,
		CG_SPELL_ATTACK_UP = 17,
		CG_SPELL_ATTACK_STRAIGHT = 18,
		CG_SPELL_ATTACK_DOWN = 19,
		CG_START_MOVING = 20,
		CG_STOP_MOVING = 21
	};

	static H3LoadedDEF* Load(LPCSTR name) { return THISCALL_1(H3LoadedDEF*, 0x55C9C0, name); }
	VOID AddFrameFromDef(LPCSTR source, INT32 index);
	VOID ColorToPlayer(INT32 id);

	VOID DrawTransparent(INT32 frame, H3LoadedPCX16 *pcx, INT32 x, INT32 y, BOOL transparent = TRUE, BOOL mirror = FALSE, INT32 group = 0, INT32 xFromRight = 0, INT32 yFromBottom = 0)
	{
		THISCALL_15(VOID, 0x47BA90, this, group, frame, xFromRight, yFromBottom, widthDEF, heightDEF, pcx->buffer, x, y, pcx->width, pcx->height, pcx->scanlineSize, mirror, transparent);
	}
};

#pragma pack(pop)


inline VOID H3LoadedDEF::AddFrameFromDef(LPCSTR source, INT32 index)
{
	H3LoadedDEF *src = H3LoadedDEF::Load(source);
	if (!src)
		return;
	H3DefFrame *frm = src->groups[0]->frames[0];
	H3DefGroup *grp = groups[0];

	H3DefFrame *frame = (H3DefFrame *)F_malloc(sizeof(H3DefFrame));
	if (frame)
	{
		F_memcpy(frame, frm, sizeof(H3DefFrame)); // copy frame data
		frame->rawData = (PUINT8)F_malloc(frame->rawDataSize); // new data buffer
		F_memcpy(frame->rawData, frm->rawData, frame->rawDataSize); // copy data buffer

		int nFrames = grp->count;

		if (nFrames < index + 1) // add room for frame
		{
			H3DefFrame ** newList = (H3DefFrame**)F_realloc(grp->frames, (index + 1) * 4);
			if (newList)
			{
				newList[index] = frame;
				grp->count = index + 1;
				grp->spritesSize = index + 1;
				grp->frames = newList;
			}
		}
		else
			grp->frames[index] = frame;
	}
	src->Dereference(); // get rid of source
}

inline VOID H3LoadedDEF::ColorToPlayer(INT32 id)
{
	if (palette565)
		palette565->ColorToPlayer(id);
	palette888->ColorToPlayer(id);
}

#endif /* #define _H3BINARYITEMS_H_ */