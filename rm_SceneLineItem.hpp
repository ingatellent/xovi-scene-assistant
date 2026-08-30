#pragma once

#include "rm_Line.hpp"
#include "rm_SceneItem.hpp"

struct SceneLineItem : public SceneItem {
    /* 0x04 - 0x08 */ unsigned char unk_x4;
    /* 0x08 - 0x0c */ int pageIndex; // starts at 0xE?
    /* 0x0c - 0x0e */ short unk_xc; // always zero?
    /* 0x0e - 0x10 */ short unk_xe; // always one?
    /* 0x10 - 0x14 */ int sourceLayerId; // always layer1 = 0xB, otherwise arbitrary?
    /* 0x14 - 0x18 */ short unk_x14; // always zero?
    /* 0x16 - 0x1a */ short unk_x16; // non-zero if copied from another layer?
    /* 0x18 - 0x1c */ int unk_x18;
    /* 0x1c - 0x20 */ void* unk_x1c;
    /* 0x20 - 0x28 */ unsigned char unk_x20; // 0 or 2
    /* 0x21 - 0x29 */ unsigned char unk_x21; // 0 or 2
    /* 0x24 - 0x30 */ void* unk_x24[3];
    /* 0x30 - 0x48 */ Line line;
    /* 0x78 - 0xa0 */ int unk_x78; // 1 on new items, 0x92B, 0xC6D, 0xF2B on very old items?
    /* 0x7c - 0xa4 */ int unk_x7c[3];

    static void* vtable_ptr;
    static void setupVtable(void* vtable);

    static SceneLineItem fromLine(Line &&line) {
        SceneLineItem item = {};
        item.vtable = vtable_ptr;
        item.unk_x4 = 3;
        item.pageIndex = 0xE;
        item.unk_xe = 1;
        item.sourceLayerId = 0xB;
        item.line = line;
        item.unk_x78 = 1;
        return item;
    }

    static SceneLineItem* tryCast(SceneItem* item);

    static void log(const SceneLineItem& item);
};
#ifdef __arm__
static_assert(offsetof(SceneLineItem, unk_x20) == 0x20);
static_assert(offsetof(SceneLineItem, line) == 0x30);
static_assert(offsetof(SceneLineItem, unk_x78) == 0x78);
static_assert(sizeof(SceneLineItem) == 0x88);
#elifdef __aarch64__
static_assert(offsetof(SceneLineItem, unk_x20) == 0x28);
static_assert(offsetof(SceneLineItem, line) == 0x48);
static_assert(offsetof(SceneLineItem, unk_x78) == 0xa0);
static_assert(sizeof(SceneLineItem) == 0xb0);
#else
#error "Unknown Arch"
#endif
