#include "bfLayerKiller.h"

//BfLayerKiller* BfLayerKiller::__p = nullptr;
//
//
//BfLayerKiller::BfLayerKiller() 
//{
//	__layers.reserve(10000);
//	BfLayerKiller::set_root(this);
//}
//
//
//void BfLayerKiller::add(std::shared_ptr<BfDrawLayer> layer) {
//	__layers.push_back(std::move(layer));
//}
//
//void BfLayerKiller::kill() {
//	__layers.clear();
//}
//
//BfLayerKiller* BfLayerKiller::get_root() {
//	return __p;
//}
//void BfLayerKiller::set_root(BfLayerKiller* k) {
//	__p = k;
//}