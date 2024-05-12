# We simply define the name of the executable, called calc,
# then list the source files to compile and the library to
# link against:
add_executable (calc
  Calc.cpp CodeGen.cpp Lexer.cpp Parser.cpp Sema.cpp)
target_link_libraries(calc PRIVATE ${llvm_libs})