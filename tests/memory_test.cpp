// #include <catch2/catch_test_macros.hpp>
//
// // #include "bfCurves.hpp"
// // #include "bfMatrix.hpp"
//
// #include <bfCurves2.h>
//
// #include <iostream>
// #include <memory>
// #include <vector>
//
// void print_vec(std::vector<int> vec, std::string mess)
// {
//    std::cout << mess << std::endl;
//    for (auto& const it : vec)
//    {
//       std::cout << it << "\n";
//    }
// }
//
// struct tC
// {
//    std::vector<int> array;
// };
//
// TEST_CASE("CURVES_H", "[single-file]")
// {
//    REQUIRE(BfID().get() == 0);
//    REQUIRE(BfID().get() == 1);
//    REQUIRE(BfID().get() == 2);
//
//    REQUIRE(BfID().get() == 3);
//    REQUIRE(BfID().get() == 4);
//    REQUIRE(BfID().get() == 5);
//
//    BfLayer layer1({10.0f, -10.0f, 0.0f},
//                   {30.0f, 10.0f, -5.0f},
//                   {-23.0f, 3.0f, 38.0f});
//    BfLayer layer2({165.0f, -119.0f, 184.0f}, -2840.0f);
//
//    REQUIRE(BfLayer::is_layers_equal(layer1, layer2));
//
//    auto rec1 = std::make_shared<BfRectangle>(
//        BfRectangle(BfVertex3({0, 0, 0}), BfVertex3({1, 1, 0})));
//    uint16_t rec_id = layer1.add_obj(rec1);
//
//    REQUIRE(layer1.get_obj(rec_id)->get_id() == rec_id);
//    REQUIRE(layer1.get_obj(rec_id) == rec1);
//    REQUIRE(rec1.use_count() == 2);
//    rec1.reset();
//
//    int rec_1_use_count = layer1.get_obj(rec_id).use_count();
//    REQUIRE(rec_1_use_count == 2);
//    REQUIRE(layer1.get_obj(rec_id)->get_id() == rec_id);
//
//    layer1.delete_obj(rec_id);
//    REQUIRE(layer1.get_obj(rec_id) == nullptr);
//
//    rec1 = std::make_shared<BfRectangle>(
//        BfRectangle(BfVertex3({0, 0, 0}, {1.0f, 1.0f, 1.0f}),
//                    BfVertex3({1, 1, 0}, {1.0f, 1.0f, 1.0f})));
//    uint16_t rec_id2 = layer1.add_obj(rec1);
//    uint16_t rec_id3 = layer1.add_obj(rec1);
//
//    layer1.get_obj(rec_id2)->calculate_geometry(
//        BF_DRAW_OBJECT_TRIANGLE_MODE_BIT);
//    auto pvec = layer1.get_obj(rec_id2)->get_pVertices();
//    // Write to buffers //
//    size_t                 total_vertex_len = layer1.get_total_vertex_size();
//    std::vector<BfVertex3> wtb_arr;
//    wtb_arr.reserve(total_vertex_len);
//
//    auto obj1 = layer1.get_obj(rec_id2);
//    auto obj2 = layer1.get_obj(rec_id3);
//    for (size_t i = 0; i < obj1->get_pVertices()->size(); i++)
//    {
//       wtb_arr.push_back(obj1->get_pVertices()->at(i));
//    }
//    for (size_t i = 0; i < obj2->get_pVertices()->size(); i++)
//    {
//       wtb_arr.push_back(obj2->get_pVertices()->at(i));
//    }
//    VmaAllocator allocator{};
//    layer1.bind_allocator(&allocator);
//
//    void*      ptr  = layer1.write_to_buffers();
//    BfVertex3* ptr3 = static_cast<BfVertex3*>(ptr);
//    for (size_t i = 0; i < layer1.get_total_vertex_size(); i++)
//    {
//       REQUIRE(ptr3[i].pos == wtb_arr[i].pos);
//       REQUIRE(ptr3[i].color == wtb_arr[i].color);
//       REQUIRE(ptr3[i].normals == wtb_arr[i].normals);
//    }
//
//    // using p_arr = std::shared_ptr<std::vector<int>>;
//
//    // auto p = std::make_shared<std::vector<int>>(std::vector<int>(10));
//
//    // p->operator[](0);
// }
