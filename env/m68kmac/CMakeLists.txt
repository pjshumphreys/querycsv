# cmake . -DCMAKE_TOOLCHAIN_FILE=[path to Retro68-build]/toolchain/m68k-apple-macos/cmake/retro68.toolchain.cmake

cmake_minimum_required(VERSION 2.8)

# See: https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html
SET(CMAKE_CXX_FLAGS "-Ofast")


enable_language(ASM)
enable_language(C)
add_application(QueryCSV
  hash2.c
  hash3.c
  hash4a.c
  hash4b.c
  hash4c.c
  lexer.c
  sql.c
  querycsv.c
  mac.c
  TEGlue.s
  size.r
  mac.r

  CREATOR "qcsv"
)

add_compile_definitions(MPW_C)
add_compile_definitions(RETRO68)
set_target_properties(QueryCSV PROPERTIES COMPILE_OPTIONS "-ffunction-sections")
set_target_properties(QueryCSV PROPERTIES LINK_FLAGS " -Wl,-gc-sections -W")
