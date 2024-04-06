#ifndef BF_LAYER_KILLER_H
#define BF_LAYER_KILLER_H

#include "bfDrawObject.h"

class BfLayerKiller {
	std::vector<std::shared_ptr<BfDrawLayer>> __layers;

	static BfLayerKiller* __p;
public:
	BfLayerKiller();

	static BfLayerKiller* get_root();
	static void set_root(BfLayerKiller* k);

	void add(std::shared_ptr<BfDrawLayer> layer);
	void kill();

};




#endif // !BF_LAYER_KILLER_H
