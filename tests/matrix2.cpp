#include "catch2/catch_message.hpp"
#include "catch2/catch_test_macros.hpp"
#include <iostream>
#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

#include "bfMath.hpp"

TEST_CASE("Matrix2") { 
    std::vector<float> s_v {
            1, 2, 3,
            4, 5, 6,
            7, 8, 9
        };
        const size_t n = 3;
        const size_t m = 3;
        
        BfMatrix2<float> mtx(s_v, n, m);
        
    SECTION("Constructors") { 
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++) { 
                REQUIRE(mtx(i, j) == s_v[j + i * 3]);
            }
    }
    SECTION("begin_col & end_col") { 
        size_t i = 0;
        for (auto it = mtx.begin_row(0); it != mtx.end_row(0); it++) { 
            REQUIRE(*it == s_v[i]);
            i++;
        }
        i = 0; 
        for (auto it = mtx.begin_row(1); it != mtx.end_row(1); it++) { 
            REQUIRE(*it == s_v[3 + i]);
            i++;
        }
        i = 0;
        for (auto it = mtx.begin_row(2); it != mtx.end_row(2); it++) { 
            REQUIRE(*it == s_v[6 + i]);
            i++;
        }
    }
    SECTION("begin_col & end_col") { 
        size_t i = 0;
        std::vector<float> r1{1, 4, 7}; 
        for (auto it = mtx.begin_col(0); it != mtx.end_col(0); it++) { 
            REQUIRE(*it == r1[i]);
            i++;
        }
        i = 0;
        std::vector<float> r2{2, 5, 8}; 
        for (auto it = mtx.begin_col(1); it != mtx.end_col(1); it++) { 
            REQUIRE(*it == r2[i]);
            i++;
        }
        i = 0;
        std::vector<float> r3{3, 6, 9}; 
        for (auto it = mtx.begin_col(2); it != mtx.end_col(2); it++) { 
            REQUIRE(*it == r3[i]);
            i++;
        }
    } 
    SECTION("rbegin_col & rend_col") { 
        std::vector<float> r1 {3, 2, 1}; 
        size_t i = 0;
        for (auto it = mtx.rbegin_row(0); it != mtx.rend_row(0); it++) { 
            REQUIRE(*it == r1[i]);
            i++;
        }
        i = 0; 
        std::vector<float> r2 {6,5,4};
        for (auto it = mtx.rbegin_row(1); it != mtx.rend_row(1); it++) { 
            REQUIRE(*it == r2[i]);
            i++;
        }
        i = 0;
        std::vector<float> r3 {9,8,7};
        for (auto it = mtx.rbegin_row(2); it != mtx.rend_row(2); it++) { 
            REQUIRE(*it == r3[i]);
            i++;
        }
    }
    SECTION("rbegin_col & rend_col") { 
        size_t i = 0;
        std::vector<float> r1{7, 4, 1}; 
        for (auto it = mtx.rbegin_col(0); it != mtx.rend_col(0); it++) { 
            REQUIRE(*it == r1[i]);
            i++;
        }
        i = 0;
        std::vector<float> r2{8,5,2}; 
        for (auto it = mtx.rbegin_col(1); it != mtx.rend_col(1); it++) { 
            REQUIRE(*it == r2[i]);
            i++;
        }
        i = 0;
        std::vector<float> r3{9,6,3}; 
        for (auto it = mtx.rbegin_col(2); it != mtx.rend_col(2); it++) { 
            REQUIRE(*it == r3[i]);
            i++;
        }

    }
    


    SECTION("BfMatrix2 multiplication") { 
        BfMatrix2<int> A(2, 3, 1); // Создаем матрицу 2x3, заполненную единицами
        BfMatrix2<int> B(3, 2, 2); // Создаем матрицу 3x2, заполненную двойками

        BfMatrix2<int> expected(2, 2, 6); // Ожидаемая матрица при умножении A на B: все элементы равны 3*2=6

        BfMatrix2<int> result = A * B; // Умножаем матрицы A и B

        // Проверяем, что полученная матрица совпадает с ожидаемой
        for (size_t i = 0; i < expected.rows(); ++i) {
            for (size_t j = 0; j < expected.cols(); ++j) {
                REQUIRE(result(i, j) == expected(i, j));
            }
        }
        SECTION("2nd") { 
            // Создаем матрицы A и B
            std::vector<float> dataA = {1.0f, 2.0f, 3.0f, 4.0f}; 
            BfMatrix2<float> A(dataA, 2, 2); 

            std::vector<float> dataB = {5.0f, 6.0f, 7.0f, 8.0f}; 
            BfMatrix2<float> B(dataB, 2, 2); 

            std::vector<float> expectedData = {19.0f, 22.0f, 43.0f, 50.0f}; 
            BfMatrix2<float> expected(expectedData, 2, 2); 

            BfMatrix2<float> result = A * B;

            for (size_t i = 0; i < expected.rows(); ++i) {
                for (size_t j = 0; j < expected.cols(); ++j) {
                    REQUIRE(result(i, j) == expected(i, j));
                }
            }
        }
        SECTION("3nd") { 
            std::vector<float> dataA = {1.0f, 2.0f, 3.0f, 4.0f}; 
            BfMatrix2<float> A(dataA, 2, 2); 

            std::vector<float> dataB = {5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f}; 
            BfMatrix2<float> B(dataB, 2, 3); 


            std::vector<float> expectedData = {21, 24, 27, 47, 54, 61}; 
            BfMatrix2<float> expectedC(expectedData, 2, 3); 

            REQUIRE((A * B) == expectedC);

        }
    }
}

TEST_CASE("begin_dim_left") { 
        std::vector<float> s_v {
            1, 2, 3,
            4, 5, 6,
            7, 8, 9
        };
        const size_t n = 3;
        const size_t m = 3;
        
        BfMatrix2<float> mtx(s_v, n, m);
    
        {
            std::vector<float> r {1, 5, 9};
            int i = 0;
            // std::cout << *mtx.begin_dim_left(0) << ", " << *mtx.end_dim_left(0) << "\n";
            for (auto it = mtx.begin_dim_left(0); it != mtx.end_dim_left(0); it++) { 
                UNSCOPED_INFO("Outout: iterator_value="<< *it << ", real=" << r[i]);
                INFO("FDSFSDFSDFDS");
                // REQUIRE(*it == r[i]);
                i++;
            }
        }
        // {
        //     std::vector<float> r {2, 6};
        //     int i = 0;
        //     for (auto it = mtx.begin_dim_left(1); it != mtx.end_dim_left(1); it++) { 
        //         REQUIRE(*it == r[i]);
        //         i++;
        //     }
        // }
        // {
        //     std::vector<float> r {3};
        //     int i = 0;
        //     for (auto it = mtx.begin_dim_left(2); it != mtx.end_dim_left(2); it++) { 
        //         REQUIRE(*it == r[i]);
        //         i++;
        //     }
        // }
}


TEST_CASE("SWAPS") { 
    SECTION("SWAP_ROWS") { 
        std::vector<int> __A {1, 2, 3,
                          4, 5, 6,
                          7, 8, 9};

        BfMatrix2<int> A(__A, 3, 3); 


        A.swap_rows(1, 2);
        {        
            std::vector<int> __B {7, 8, 9};
            for (int i = 0; i < 3; i++) { 
                REQUIRE(__B[i] == A(1, i));
            }
        }
        {
            std::vector<int> __B {4, 5, 6};
            for (int i = 0; i < 3; i++)  {
                REQUIRE(__B[i] == A(2, i));
            }
        }
    }

    SECTION("SWAP_COLS") { 
        std::vector<int> __A {1, 2, 3,
                              4, 5, 6,
                              7, 8, 9};

        BfMatrix2<int> A(__A, 3, 3); 


        A.swap_cols(1, 2);
        {        
            std::vector<int> __B {3, 6, 9};
            for (int i = 0; i < 3; i++) { 
                REQUIRE(__B[i] == A(i, 1));
            }
        }
        {
            std::vector<int> __B {2, 5, 8};
            for (int i = 0; i < 3; i++)  {
                REQUIRE(__B[i] == A(i, 2));
            }
        }
    }

    SECTION("SWAP_ROW_COL") { 
        SECTION("0-0") { 
            std::vector<int> __A {1, 2, 3,
                                  4, 5, 6,
                                  7, 8, 9};

            BfMatrix2<int> A(__A, 3, 3); 

            std::vector<int> __B {1, 4, 7, 
                                  2, 5, 6,
                                  3, 8, 9};        

            BfMatrix2<int> B(__B, 3, 3);

            A.swap_row_col(0, 0);
            for (int i = 0; i < 9; i++) { 
                REQUIRE(B[i] == A[i]);
                
            }
        }
        SECTION("2-0") { 
            std::vector<int> __A {1, 2, 3,
                                  4, 5, 6,
                                  7, 8, 9};

            BfMatrix2<int> A(__A, 3, 3); 

            std::vector<int> __B {7, 2, 3, 
                                  8, 5, 6,
                                  9, 4, 1};        

            BfMatrix2<int> B(__B, 3, 3);

            A.swap_row_col(2, 0);
            for (int i = 0; i < 9; i++) { 
                REQUIRE(B[i] == A[i]);
                
            }
        }
    }

} 


// TEST_CASE("UPPER-TRIANGULAR FORM") { 
//     // SECTION("SIMPLE") { 
//     //     BfMatrix2<float> matrix(3, 3, 0); 
//     //     matrix(0, 0) = 1.0f; 
//     //     matrix(0, 1) = 2.0f;
//     //     matrix(0, 2) = 3.0f;
//     //     matrix(1, 1) = 4.0f;
//     //     matrix(1, 2) = 5.0f;
//     //     matrix(2, 2) = 6.0f;
//     //    
//     //     BfMatrix2<float> mtx(matrix.upper_triangular());
//     //
//     //     REQUIRE(mtx(1, 0) == 0.0f);
//     //     REQUIRE(mtx(2, 0) == 0.0f);
//     //     REQUIRE(mtx(2, 1) == 0.0f);
//     // }  
//     SECTION("NOT SIMPLE") { 
//         std::vector<float> __A { 
//             1, 2, 3,
//             4, 5, 6,
//             7, 8, 9 };
//         
//         BfMatrix2<float> A(__A, 3, 3);
//         
//         std::cout << "Start: " << A << "\n";
//         BfMatrix2<float> UP_A {A.upper_triangular()};
//         std::cout << "End: " << UP_A << "\n";
//         std::vector<float> ANS_A { 
//             1, 2, 3,
//             0, -3, -6,
//             0, 0, 0
//         };
//
//         for (int i = 0; i < 9; i++) {
//             REQUIRE(UP_A[i] == ANS_A[i]);
//         }
//
//     }
// }
// TEST_CASE("LU decomposition with permutation") {
//     SECTION("Test 3x3 matrix") {
//         std::vector<float> data = { 2, 3, 1, 4, 6, 3, 6, 10, 9 };
//         BfMatrix2<float> A(data, 3, 3);
//
//         auto [L, U] = luDecomposition(A);
//
//         REQUIRE(L.rows() == 3);
//         REQUIRE(L.cols() == 3);
//         REQUIRE(U.rows() == 3);
//         REQUIRE(U.cols() == 3);
//
//         // Verify LU decomposition
//         BfMatrix2<float> LU = L * U;
//
//         for (size_t i = 0; i < 3; ++i) {
//             for (size_t j = 0; j < 3; ++j) {
//                 // REQUIRE(Catch::Approx(LU(i, j)).margin(1e-6) == A(i, j));
//                 std::cout << LU(i,j) << ", " << A(i, j);
//             }
//         }
//
//         // Verify the permutation vector P
//         std::vector<size_t> P_expected = { 0, 2, 1 }; // Expected permutation after LU decomposition
//         std::vector<size_t> P_actual = { 0, 0, 0 }; // Initialize actual permutation vector
//
//         for (size_t i = 0; i < 3; ++i) {
//             for (size_t j = 0; j < 3; ++j) {
//                 if (L(i, j) == 1) {
//                     P_actual[i] = j;
//                     break;
//                 }
//             }
//         }
//
//         // REQUIRE(P_actual == P_expected);
//     }
// }
