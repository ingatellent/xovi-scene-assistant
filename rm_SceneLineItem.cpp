#include "rm_SceneLineItem.hpp"

#include <cstdio>

void* SceneLineItem::vtable_ptr = nullptr;

void SceneLineItem::setupVtable(void* vtable) {
	printf("Setting SceneLineItem vtable to %p\n", vtable);
	SceneLineItem::vtable_ptr = vtable;
}

void SceneLineItem::log(const SceneLineItem& item) {
	printf("  Id: %d, Source Layer: %d\n", item.pageIndex, item.sourceLayerId);

	bool unusual = false;
	unusual |= item.vtable != vtable_ptr;
	unusual |= item.unk_x4 != 3;
	unusual |= item.pageIndex < 0xE;
	unusual |= item.unk_xc != 0;
	unusual |= item.unk_xe != 1;
	unusual |= item.sourceLayerId < 0xB;
	unusual |= item.unk_x14 != 0;
	if (item.sourceLayerId > 0xB) {
		unusual |= item.unk_x16 != 1;
	}
	unusual |= item.unk_x18 != 0;
	unusual |= item.unk_x1c != nullptr;
	unusual |= !(item.unk_x20 == 0x0 || (item.unk_x20 == 0x2 && item.unk_x21 == 0x2));
	for (auto v : item.unk_x24) {
		unusual |= v != nullptr;
	}
	unusual |= item.unk_x78 != 1;
	for (auto v : item.unk_x7c) {
		unusual |= v != 0;
	}

	if (!unusual) {
		printf("Normal clipboard item\n");
		return;
	}
	printf("Unusual clipboard item\n");

	const unsigned int* raw = reinterpret_cast<const unsigned int*>(&item.vtable);
	for (uint8_t i = 0; i < (sizeof(SceneLineItem) / 4); ++i) {
		if (i < 12 || i >= 30) {
			printf("\t%02X: %08X\n", i * 4, raw[i]);
		}
	}
}

SceneLineItem* SceneLineItem::tryCast(SceneItem* item) {
    if (!item) return nullptr;

    auto* lineItem = reinterpret_cast<SceneLineItem*>(item);

    if (lineItem->unk_xc != 0) return nullptr;
    if (lineItem->unk_xe != 0) return nullptr;
    if (lineItem->unk_x78 != 1) return nullptr;


    if (!(lineItem->unk_x20 == 0x0 || (lineItem->unk_x20 == 0x2 && lineItem->unk_x21 == 0x2))) return nullptr;
    

    // We could also check for correct vtable, but it would break until vtable is set
    // Consider doing this check if vtable_ptr != nullptr
    // if(lineItem->vtable != SceneLineItem::vtable_ptr) return nullptr;

    return lineItem;
}

