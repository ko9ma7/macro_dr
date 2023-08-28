cmake_minimum_required(VERSION 3.5)

project(macro_dr LANGUAGES CXX)
find_package(OpenMP REQUIRED)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_FLAGS "-ftemplate-backtrace-limit=0 -Werror=return-type -fconcepts-diagnostics-depth=20" )
#find_package(Catch2 REQUIRED)
#target_link_libraries(tests Catch2::Catch2)

find_package(BLAS)
find_package(LAPACK)
if(LAPACK_FOUND AND BLAS_FOUND)
   set(lapackblas_libraries ${BLAS_LIBRARIES} ${LAPACK_LIBRARIES})
endif()

add_executable(macro_dr main.cpp
   # CLI_base.h
   maybe_error.h
   CLI_regular_types.h
   qmodel.h
   run_script.txt
   CLI_macro_dr.h
   CLI_grammar.h
   grammar_untyped.h
    grammar_Identifier.h
    lexer_untyped.h
    test.txt
    matrix.h
    lapack_headers.h
#    tst_untyped_grammar.cpp
)
target_link_libraries(macro_dr ${lapackblas_libraries}  OpenMP::OpenMP_CXX)

include(GNUInstallDirs)
install(TARGETS macro_dr
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)