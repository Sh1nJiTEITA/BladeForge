# This is a stub override to prevent xtensor-blas from trying to find LAPACK manually
set(LAPACK_FOUND TRUE)
set(LAPACK_LIBRARIES OpenBLAS::OpenBLAS)