cmake_minimum_required(VERSION 3.16)
project(neurolabi LANGUAGES C CXX)

# ===================================================
# Template generation (Perl)
# ===================================================
find_program(PERL_EXECUTABLE NAMES perl REQUIRED)
set(PARSETMPL "${CMAKE_CURRENT_SOURCE_DIR}/../shell/parsetmpl.pl")

# Accumulators for generated sources/headers (from .t)
set(GENERATED_C_SOURCES "")
set(GENERATED_C_HEADERS "")

function(gen_from_template TEMPLATE REL_OUTPUT)
  set(args ${ARGN})
  set(out "${CMAKE_CURRENT_BINARY_DIR}/${REL_OUTPUT}")  # write into build dir
  get_filename_component(out_dir "${out}" DIRECTORY)
  file(MAKE_DIRECTORY "${out_dir}")

  add_custom_command(
    OUTPUT  "${out}"
    COMMAND "${PERL_EXECUTABLE}" "${PARSETMPL}"
            "${CMAKE_CURRENT_SOURCE_DIR}/${TEMPLATE}"
            "${out}" ${args}
    DEPENDS "${PARSETMPL}" "${CMAKE_CURRENT_SOURCE_DIR}/${TEMPLATE}"
    COMMENT "Generating ${REL_OUTPUT} from ${TEMPLATE}"
    VERBATIM
  )

  if(REL_OUTPUT MATCHES "\\.(h|hpp)$")
    list(APPEND GENERATED_C_HEADERS "${out}")
    set(GENERATED_C_HEADERS "${GENERATED_C_HEADERS}" PARENT_SCOPE)
  else()
    list(APPEND GENERATED_C_SOURCES "${out}")
    set(GENERATED_C_SOURCES "${GENERATED_C_SOURCES}" PARENT_SCOPE)
  endif()
endfunction()

# ---- Rules from legacy Makefile.ctc (explicit, no mixing) ----
# Arrays (signed + unsigned used by private/*.c)
gen_from_template(tz_array.h.t tz_darray.h           double   DARRAY  fftw)
gen_from_template(tz_array.c.t tz_darray.c           double   DARRAY  fftw)
gen_from_template(tz_array.h.t tz_farray.h           float    FARRAY  fftwf)
gen_from_template(tz_array.c.t tz_farray.c           float    FARRAY  fftwf)
gen_from_template(tz_array.h.t tz_iarray.h           int      IARRAY)
gen_from_template(tz_array.c.t tz_iarray.c           int      IARRAY)
gen_from_template(tz_array.h.t tz_i64array.h         int64_t  I64ARRAY)
gen_from_template(tz_array.c.t tz_i64array.c         int64_t  I64ARRAY)
gen_from_template(tz_array.h.t tz_u16array.h         tz_uint16 U16ARRAY)
gen_from_template(tz_array.c.t tz_u16array.c         tz_uint16 U16ARRAY)
gen_from_template(tz_array.h.t tz_u8array.h          tz_uint8  U8ARRAY)
gen_from_template(tz_array.c.t tz_u8array.c          tz_uint8  U8ARRAY)


# Matrices (signed + unsigned used by tz_arraytrans.h)
gen_from_template(tz_matrix.c.t tz_dmatrix.c         double   DMatrix darray)
gen_from_template(tz_matrix.h.t tz_dmatrix.h         double   DMatrix darray)
gen_from_template(tz_matrix.c.t tz_fmatrix.c         float    FMatrix farray)
gen_from_template(tz_matrix.h.t tz_fmatrix.h         float    FMatrix farray)
gen_from_template(tz_matrix.c.t tz_imatrix.c         int      IMatrix iarray)
gen_from_template(tz_matrix.h.t tz_imatrix.h         int      IMatrix iarray)
gen_from_template(tz_matrix.c.t tz_u16matrix.c       uint16_t U16Matrix u16array)
gen_from_template(tz_matrix.h.t tz_u16matrix.h       uint16_t U16Matrix u16array)
gen_from_template(tz_matrix.c.t tz_u8matrix.c        uint8_t  U8Matrix u8array)
gen_from_template(tz_matrix.h.t tz_u8matrix.h        uint8_t  U8Matrix u8array)

# Image libs
gen_from_template(tz_timage_lib.h.t tz_dimage_lib.h  DIMAGE   DMatrix DOUBLE D fftw  Double darray)
gen_from_template(tz_timage_lib.c.t tz_dimage_lib.c  DIMAGE   DMatrix DOUBLE D fftw  Double darray)
gen_from_template(tz_timage_lib.h.t tz_fimage_lib.h  FIMAGE   FMatrix FLOAT  F fftwf Float  farray)
gen_from_template(tz_timage_lib.c.t tz_fimage_lib.c  FIMAGE   FMatrix FLOAT  F fftwf Float  farray)
gen_from_template(tz_timage_lib.h.t tz_iimage_lib.h  IIMAGE   IMatrix INT    I fftwf Int    iarray)
gen_from_template(tz_timage_lib.c.t tz_iimage_lib.c  IIMAGE   IMatrix INT    I fftwf Int    iarray)

# FFTW families (headers include tz_fftw_header.h mirrored below)
gen_from_template(tz_fftw.h.t tz_fftw.h   fftw   double)
gen_from_template(tz_fftw.c.t tz_fftw.c   fftw   double)
gen_from_template(tz_fftw.h.t tz_fftwf.h  fftwf  float)
gen_from_template(tz_fftw.c.t tz_fftwf.c  fftwf  float)

# Array/list utilities
gen_from_template(tz_arraylist.h.t tz_int_arraylist.h            Int    int      basic)
gen_from_template(tz_arraylist.c.t tz_int_arraylist.c            Int    int      basic)
gen_from_template(tz_arraylist.h.t tz_int64_arraylist.h          Int64  int64_t  basic)
gen_from_template(tz_arraylist.c.t tz_int64_arraylist.c          Int64  int64_t  basic)
gen_from_template(tz_arraylist.h.t tz_unipointer_arraylist.h     Unipointer unipointer_t compact_pointer)
gen_from_template(tz_arraylist.c.t tz_unipointer_arraylist.c     Unipointer unipointer_t compact_pointer)
gen_from_template(tz_arraylist.h.t tz_stack_tile_arraylist.h     Stack_Tile   Stack_Tile   object)
gen_from_template(tz_arraylist.c.t tz_stack_tile_arraylist.c     Stack_Tile   Stack_Tile   object)
gen_from_template(tz_arraylist.h.t tz_stack_tile_i_arraylist.h   Stack_Tile_I Stack_Tile_I object)
gen_from_template(tz_arraylist.c.t tz_stack_tile_i_arraylist.c   Stack_Tile_I Stack_Tile_I object)
gen_from_template(tz_arraylist.h.t tz_neuron_component_arraylist.h Neuron_Component Neuron_Component object)
gen_from_template(tz_arraylist.c.t tz_neuron_component_arraylist.c Neuron_Component Neuron_Component object)
gen_from_template(tz_arraylist.h.t tz_swc_arraylist.h            Swc  Swc_Node  struct)
gen_from_template(tz_arraylist.c.t tz_swc_arraylist.c            Swc  Swc_Node  struct)

# Linked lists + doubly linked lists
gen_from_template(tz_linked_list.h.t     tz_int_linked_list.h           Int    int    basic)
gen_from_template(tz_linked_list.c.t     tz_int_linked_list.c           Int    int    basic)
gen_from_template(tz_linked_list.h.t     tz_double_linked_list.h        Double double basic)
gen_from_template(tz_linked_list.c.t     tz_double_linked_list.c        Double double basic)
gen_from_template(tz_linked_list.h.t     tz_object_3d_linked_list.h     Object_3d Object_3d_P object)
gen_from_template(tz_linked_list.c.t     tz_object_3d_linked_list.c     Object_3d Object_3d_P object)
gen_from_template(tz_linked_list.h.t     tz_voxel_linked_list.h         Voxel  Voxel_P compact_pointer)
gen_from_template(tz_linked_list.h.t     tz_unipointer_linked_list.h    Unipointer unipointer_t compact_pointer)
gen_from_template(tz_linked_list.c.t     tz_unipointer_linked_list.c    Unipointer unipointer_t compact_pointer)

gen_from_template(tz_doubly_linked_list.h.t tz_int_doubly_linked_list.h                    Int int basic)
gen_from_template(tz_doubly_linked_list.c.t tz_int_doubly_linked_list.c                    Int int basic)
gen_from_template(tz_doubly_linked_list.h.t tz_local_neuroseg_plane_doubly_linked_list.h   Local_Neuroseg_Plane  Local_Neuroseg_Plane_P  compact_pointer)
gen_from_template(tz_doubly_linked_list.c.t tz_local_neuroseg_plane_doubly_linked_list.c   Local_Neuroseg_Plane  Local_Neuroseg_Plane_P  compact_pointer)
gen_from_template(tz_doubly_linked_list.h.t tz_local_neuroseg_ellipse_doubly_linked_list.h Local_Neuroseg_Ellipse Local_Neuroseg_Ellipse_P compact_pointer)
gen_from_template(tz_doubly_linked_list.c.t tz_local_neuroseg_ellipse_doubly_linked_list.c Local_Neuroseg_Ellipse Local_Neuroseg_Ellipse_P compact_pointer)
gen_from_template(tz_doubly_linked_list.h.t tz_local_neuroseg_doubly_linked_list.h         Local_Neuroseg Local_Neuroseg_P compact_pointer)
gen_from_template(tz_doubly_linked_list.c.t tz_local_neuroseg_doubly_linked_list.c         Local_Neuroseg Local_Neuroseg_P compact_pointer)
gen_from_template(tz_doubly_linked_list.h.t tz_locne_node_doubly_linked_list.h             Locne_Node Locne_Node_P object)
gen_from_template(tz_doubly_linked_list.c.t tz_locne_node_doubly_linked_list.c             Locne_Node Locne_Node_P object)

# Trace nodes, chains
gen_from_template(tz_trace_node.h.t         tz_locseg_node.h                       Locseg         Local_Neuroseg)
gen_from_template(tz_trace_node.c.t         tz_locseg_node.c                       Locseg         Local_Neuroseg)
gen_from_template(tz_trace_node.h.t         tz_locrect_node.h                      Locrect        Local_R2_Rect)
gen_from_template(tz_trace_node.c.t         tz_locrect_node.c                      Locrect        Local_R2_Rect)
gen_from_template(tz_trace_node.h.t         tz_locnp_node.h                        Locnp          Local_Neuroseg_Plane)
gen_from_template(tz_trace_node.c.t         tz_locnp_node.c                        Locnp          Local_Neuroseg_Plane)

gen_from_template(tz_doubly_linked_list.h.t tz_locseg_node_doubly_linked_list.h    Locseg_Node    Locseg_Node_P  object)
gen_from_template(tz_doubly_linked_list.c.t tz_locseg_node_doubly_linked_list.c    Locseg_Node    Locseg_Node_P  object)
gen_from_template(tz_doubly_linked_list.h.t tz_locrect_node_doubly_linked_list.h   Locrect_Node   Locrect_Node_P object)
gen_from_template(tz_doubly_linked_list.c.t tz_locrect_node_doubly_linked_list.c   Locrect_Node   Locrect_Node_P object)
gen_from_template(tz_doubly_linked_list.h.t tz_locnp_node_doubly_linked_list.h     Locnp_Node     Locnp_Node_P   object)
gen_from_template(tz_doubly_linked_list.c.t tz_locnp_node_doubly_linked_list.c     Locnp_Node     Locnp_Node_P   object)

gen_from_template(tz_trace_chain_com.h.t    tz_locseg_chain_com.h                  Locseg         Local_Neuroseg)
gen_from_template(tz_trace_chain_com.c.t    tz_locseg_chain_com.c                  Locseg         Local_Neuroseg)
gen_from_template(tz_trace_chain_com.h.t    tz_locrect_chain_com.h                 Locrect        Local_R2_Rect)
gen_from_template(tz_trace_chain_com.c.t    tz_locrect_chain_com.c                 Locrect        Local_R2_Rect)
gen_from_template(tz_trace_chain_com.h.t    tz_locnp_chain_com.h                   Locnp          Local_Neuroseg_Plane)
gen_from_template(tz_trace_chain_com.c.t    tz_locnp_chain_com.c                   Locnp          Local_Neuroseg_Plane)
gen_from_template(tz_trace_chain_com.h.t    tz_locne_chain_com.h                   Locne          Local_Neuroseg_Ellipse)
gen_from_template(tz_trace_chain_com.c.t    tz_locne_chain_com.c                   Locne          Local_Neuroseg_Ellipse)
gen_from_template(tz_trace_node.h.t         tz_locne_node.h                        Locne          Local_Neuroseg_Ellipse)

# Generated headers must be visible during this build
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# ===================================================
# Mirror non-templated headers/fragments from src/c → build dir root
# BUT: exclude any headers we generate from templates to avoid duplicate rules.
# ===================================================
# Build a set of generated-header basenames
set(_GENERATED_HEADER_BASENAMES "")
foreach(_gh IN LISTS GENERATED_C_HEADERS)
  get_filename_component(_bn "${_gh}" NAME)
  list(APPEND _GENERATED_HEADER_BASENAMES "${_bn}")
endforeach()

file(GLOB SRC_C_TOPLEVEL_HEADERS   "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB SRC_C_TOPLEVEL_FRAGMENTS "${CMAKE_CURRENT_SOURCE_DIR}/*.d" "${CMAKE_CURRENT_SOURCE_DIR}/*.inc")

set(COPIED_TOPLEVEL_FILES "")
foreach(src_file IN LISTS SRC_C_TOPLEVEL_HEADERS SRC_C_TOPLEVEL_FRAGMENTS)
  get_filename_component(base_name "${src_file}" NAME)
  # Skip if this header is generated (we already produce it with a rule)
  list(FIND _GENERATED_HEADER_BASENAMES "${base_name}" _is_gen)
  if(_is_gen GREATER -1)
    continue()
  endif()

  set(dst_file "${CMAKE_CURRENT_BINARY_DIR}/${base_name}")
  add_custom_command(
    OUTPUT "${dst_file}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${src_file}" "${dst_file}"
    DEPENDS "${src_file}"
    COMMENT "Copy ${base_name} → build dir"
    VERBATIM
  )
  list(APPEND COPIED_TOPLEVEL_FILES "${dst_file}")
endforeach()
add_custom_target(copy_src_c_non_templated ALL DEPENDS ${COPIED_TOPLEVEL_FILES})

# ===================================================
# Copy the entire src/c/private tree → _build/core_c/private
# ===================================================
file(GLOB_RECURSE SRC_C_PRIVATE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/private/*.*")
set(COPIED_PRIVATE_FILES "")
foreach(src_file IN LISTS SRC_C_PRIVATE_FILES)
  file(RELATIVE_PATH rel "${CMAKE_CURRENT_SOURCE_DIR}" "${src_file}")
  set(dst "${CMAKE_CURRENT_BINARY_DIR}/${rel}")
  get_filename_component(dst_dir "${dst}" DIRECTORY)
  add_custom_command(
    OUTPUT "${dst}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${dst_dir}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${src_file}" "${dst}"
    DEPENDS "${src_file}"
    COMMENT "Copy private/${rel} → build dir"
    VERBATIM
  )
  list(APPEND COPIED_PRIVATE_FILES "${dst}")
endforeach()
add_custom_target(copy_src_c_private ALL DEPENDS ${COPIED_PRIVATE_FILES})

# ===================================================
# Also mirror generated headers into the *source* tree (src/c)
# (satisfies legacy relative includes in private/*.c, e.g. "../tz_u8array.h")
# ===================================================
add_custom_target(mirror_generated_headers_to_src ALL DEPENDS ${GENERATED_C_HEADERS})
foreach(hdr IN LISTS GENERATED_C_HEADERS)
  get_filename_component(_base "${hdr}" NAME)
  add_custom_command(
    TARGET mirror_generated_headers_to_src
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${hdr}" "${CMAKE_CURRENT_SOURCE_DIR}/${_base}"
    COMMENT "Mirror generated header ${_base} → src/c"
    VERBATIM
  )
endforeach()

# ===================================================
# Myers *.p -> *.c generation (awk)
# ===================================================
set(MYERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../lib/genelib/src" CACHE PATH "Myers/genelib src directory")
find_program(AWK_EXECUTABLE NAMES gawk awk REQUIRED)
set(MYERS_MANAGER_AWK "${MYERS_DIR}/manager.awk")

set(MYERS_P_BASENAMES
  cdf
  fct_min
  contour_lib
  fct_root
  image_lib
  utilities
  draw_lib
  level_set
  hash
  water_shed
  tiff_io
  tiff_image
)

set(MYERS_SOURCES "")
foreach(name IN LISTS MYERS_P_BASENAMES)
  set(p_src "${MYERS_DIR}/${name}.p")
  if(NOT EXISTS "${p_src}")
    message(FATAL_ERROR "Required Myers source not found: ${p_src}")
  endif()

  set(out_c "${CMAKE_CURRENT_BINARY_DIR}/myers/${name}.c")
  get_filename_component(out_dir "${out_c}" DIRECTORY)
  file(MAKE_DIRECTORY "${out_dir}")

  add_custom_command(
    OUTPUT  "${out_c}"
    COMMAND "${AWK_EXECUTABLE}" -f "${MYERS_MANAGER_AWK}" "${p_src}" > "${out_c}"
    DEPENDS "${MYERS_MANAGER_AWK}" "${p_src}"
    COMMENT "Generating ${name}.c from ${name}.p (manager.awk)"
    VERBATIM
  )

  list(APPEND MYERS_SOURCES "${out_c}")
endforeach()

set(MYERS_EMBEDDED_PATHS
  "${CMAKE_CURRENT_BINARY_DIR}/myers/tiff_io.c"
  "${CMAKE_CURRENT_BINARY_DIR}/myers/tiff_image.c"
)

# Remove them from the compiled list, but still make sure they get generated
list(REMOVE_ITEM MYERS_SOURCES ${MYERS_EMBEDDED_PATHS})
add_custom_target(generate_myers_embedded DEPENDS ${MYERS_EMBEDDED_PATHS})

# ===================================================
# Build options / policy
# ===================================================
option(USE_LIBFFTW    "Link double-precision FFTW3"         ON)
option(USE_LIBFFTWF   "Link single-precision FFTW3 (fftwf)" OFF)
option(USE_LIBZ       "Link zlib (zlib-ng compat is fine)"  ON)
option(USE_LIBXML2    "Link libxml2"                        ON)
option(USE_LIBJANSSON "Link jansson"                        ON)
option(ENABLE_AVX2    "Enable AVX2/FMA where supported"     OFF)

set(DEPS_PREFIX "" CACHE PATH "Prefix where static dependencies are installed")

set(BUILD_SHARED_LIBS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS ON)

# NOTE: Do NOT set optimization flags here; they come from the Python driver.
if(ENABLE_AVX2)
  add_compile_options(-mavx2 -mfma)
endif()

# ===================================================
# Discovery hints (Windows + Linux)
# ===================================================
set(_HINT_PREFIXES "")
if(DEPS_PREFIX)
  list(APPEND _HINT_PREFIXES "${DEPS_PREFIX}")
endif()
if(WIN32)
  list(APPEND _HINT_PREFIXES "C:/msys64/clang64")
else()
  list(APPEND _HINT_PREFIXES "/usr" "/usr/local")
endif()

set(_HINT_INCLUDE_DIRS "")
set(_HINT_LIBRARY_DIRS "")
foreach(_p IN LISTS _HINT_PREFIXES)
  list(APPEND _HINT_INCLUDE_DIRS "${_p}/include" "${_p}/include/libxml2")
  list(APPEND _HINT_LIBRARY_DIRS "${_p}/lib" "${_p}/lib64" "${_p}/lib/x86_64-linux-gnu")
endforeach()

# Prefer static libs on Windows to avoid import libs
if(WIN32)
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".a;.dll.a;.lib" CACHE STRING "" FORCE)
endif()

# ===================================================
# Static deps discovery (prefer DEPS_PREFIX)
# ===================================================
set(EXT_LIBS "")

# zlib (zlib-ng compat provides libz.a)
if(USE_LIBZ)
  if(WIN32)
    find_library(ZLIB_LIBRARY NAMES libz.a z libz zlib HINTS ${_HINT_LIBRARY_DIRS})
  else()
    find_library(ZLIB_LIBRARY NAMES z libz zlib HINTS ${_HINT_LIBRARY_DIRS})
  endif()
  find_path(ZLIB_INCLUDE_DIR NAMES zlib.h HINTS ${_HINT_INCLUDE_DIRS})
  if(NOT ZLIB_LIBRARY OR NOT ZLIB_INCLUDE_DIR)
    message(FATAL_ERROR "Could not find static zlib (zlib-ng compat). Searched: ${_HINT_LIBRARY_DIRS}")
  endif()
  list(APPEND EXT_LIBS "${ZLIB_LIBRARY}")
  include_directories("${ZLIB_INCLUDE_DIR}")
  message(STATUS "Found zlib: ${ZLIB_LIBRARY}")
endif()

# libxml2
if(USE_LIBXML2)
  if(WIN32)
    find_library(LIBXML2_LIBRARY NAMES libxml2.a xml2 libxml2 HINTS ${_HINT_LIBRARY_DIRS})
  else()
    find_library(LIBXML2_LIBRARY NAMES xml2 libxml2 HINTS ${_HINT_LIBRARY_DIRS})
  endif()
  find_path(LIBXML2_INCLUDE_DIR NAMES libxml/parser.h HINTS ${_HINT_INCLUDE_DIRS})
  if(NOT LIBXML2_LIBRARY OR NOT LIBXML2_INCLUDE_DIR)
    message(FATAL_ERROR "Could not find static libxml2. Searched: ${_HINT_LIBRARY_DIRS}")
  endif()
  list(APPEND EXT_LIBS "${LIBXML2_LIBRARY}")
  include_directories("${LIBXML2_INCLUDE_DIR}")
  message(STATUS "Found libxml2: ${LIBXML2_LIBRARY}")
endif()

# jansson
if(USE_LIBJANSSON)
  if(WIN32)
    find_library(JANSSON_LIBRARY NAMES libjansson.a jansson HINTS ${_HINT_LIBRARY_DIRS})
  else()
    find_library(JANSSON_LIBRARY NAMES jansson HINTS ${_HINT_LIBRARY_DIRS})
  endif()
  find_path(JANSSON_INCLUDE_DIR NAMES jansson.h HINTS ${_HINT_INCLUDE_DIRS})
  if(NOT JANSSON_LIBRARY OR NOT JANSSON_INCLUDE_DIR)
    message(FATAL_ERROR "Could not find static jansson. Searched: ${_HINT_LIBRARY_DIRS}")
  endif()
  list(APPEND EXT_LIBS "${JANSSON_LIBRARY}")
  include_directories("${JANSSON_INCLUDE_DIR}")
  message(STATUS "Found jansson: ${JANSSON_LIBRARY}")
endif()

# fftw3 (double)
if(USE_LIBFFTW)
  if(WIN32)
    find_library(FFTW3_LIBRARY NAMES libfftw3.a fftw3 HINTS ${_HINT_LIBRARY_DIRS})
  else()
    find_library(FFTW3_LIBRARY NAMES fftw3 HINTS ${_HINT_LIBRARY_DIRS})
  endif()
  find_path(FFTW3_INCLUDE_DIR NAMES fftw3.h HINTS ${_HINT_INCLUDE_DIRS})
  if(NOT FFTW3_LIBRARY OR NOT FFTW3_INCLUDE_DIR)
    message(FATAL_ERROR "Could not find fftw3. Searched: ${_HINT_LIBRARY_DIRS}")
  endif()
  list(APPEND EXT_LIBS "${FFTW3_LIBRARY}")
  include_directories("${FFTW3_INCLUDE_DIR}")
  message(STATUS "Found fftw3: ${FFTW3_LIBRARY}")
endif()

# fftwf (single)
if(USE_LIBFFTWF)
  if(WIN32)
    find_library(FFTW3F_LIBRARY NAMES libfftw3f.a fftw3f HINTS ${_HINT_LIBRARY_DIRS})
  else()
    find_library(FFTW3F_LIBRARY NAMES fftw3f HINTS ${_HINT_LIBRARY_DIRS})
  endif()
  find_path(FFTW3F_INCLUDE_DIR NAMES fftw3.h HINTS ${_HINT_INCLUDE_DIRS})
  if(NOT FFTW3F_LIBRARY OR NOT FFTW3F_INCLUDE_DIR)
    message(FATAL_ERROR "Could not find fftw3f. Searched: ${_HINT_INCLUDE_DIRS}")
  endif()
  list(APPEND EXT_LIBS "${FFTW3F_LIBRARY}")
  include_directories("${FFTW3F_INCLUDE_DIR}")
  message(STATUS "Found fftw3f: ${FFTW3F_LIBRARY}")
endif()

# PCRE2 + POSIX wrapper (needed by tz_utilities.c)
find_path(PCRE2_INCLUDE_DIR NAMES pcre2posix.h HINTS ${_HINT_INCLUDE_DIRS})
if(NOT PCRE2_INCLUDE_DIR)
  message(FATAL_ERROR "PCRE2 POSIX headers (pcre2posix.h) not found in: ${_HINT_INCLUDE_DIRS}")
endif()
if(WIN32)
  find_library(PCRE2POSIX_LIBRARY NAMES libpcre2-posix.a pcre2-posix HINTS ${_HINT_LIBRARY_DIRS})
  find_library(PCRE2_8_LIBRARY    NAMES libpcre2-8.a    pcre2-8      HINTS ${_HINT_LIBRARY_DIRS})
else()
  find_library(PCRE2POSIX_LIBRARY NAMES pcre2-posix HINTS ${_HINT_LIBRARY_DIRS})
  find_library(PCRE2_8_LIBRARY    NAMES pcre2-8     HINTS ${_HINT_LIBRARY_DIRS})
endif()
if(NOT PCRE2POSIX_LIBRARY OR NOT PCRE2_8_LIBRARY)
  message(FATAL_ERROR "PCRE2 libraries (pcre2-posix, pcre2-8) not found in: ${_HINT_LIBRARY_DIRS}")
endif()
include_directories("${PCRE2_INCLUDE_DIR}")
list(APPEND EXT_LIBS "${PCRE2POSIX_LIBRARY}" "${PCRE2_8_LIBRARY}")
message(STATUS "Found PCRE2: posix=${PCRE2POSIX_LIBRARY} core=${PCRE2_8_LIBRARY}")

# ======================
# Hand-written tz_* srcs
# ======================
set(NEULIB_SOURCES
  tz_image_lib.c tz_math_lib.c tz_complex.c tz_utilities.c tz_matlabio.c
  tz_array.c tz_error.c tz_complex_f.c tz_mxutils.c tz_string.c tz_pipe.c tz_arraytrans.c
  tz_image_code.c tz_3dgeom.c tz_object_tmpl.c tz_stack_bwmorph.c
  tz_neuroseg.c tz_neuropos.c tz_object_3d.c tz_objdetect.c tz_pixel_array.c
  tz_stack_draw.c tz_stack_math.c tz_image_io.c tz_object_3d_linked_list_ext.c
  tz_stack_threshold.c tz_stack_attribute.c tz_stack_relation.c tz_object_3d_operation.c
  tz_geometry.c tz_stack_lib.c tz_stack_stat.c tz_voxel_graphics.c
  tz_voxel.c tz_voxeltrans.c tz_geo3d_vector.c tz_tvoxel.c tz_stack_sampling.c tz_stack_utils.c
  tz_local_neuroseg.c tz_neurofield.c tz_geo3d_utils.c tz_bitmask.c tz_geoangle_utils.c
  tz_stack_neighborhood.c tz_stack_objlabel.c tz_gsl_linalg.c tz_optimize_utils.c tz_arrayview.c
  tz_image_attribute.c tz_image_relation.c tz_image_array.c tz_int_histogram.c tz_stack_io.c tz_interface.c
  tz_coordinate_3d.c tz_geo3d_point_array.c tz_stack_objmask.c tz_vrml_io.c tz_geo3d_circle.c
  tz_xz_orientation.c tz_vrml_material.c tz_perceptor.c tz_cont_fun.c
  tz_geo3d_scalar_field.c tz_neuroseg_plane.c tz_local_neuroseg_plane.c tz_bifold_neuroseg.c
  tz_local_bifold_neuroseg.c tz_vrml_utils.c tz_locnp_chain.c tz_neuroseg_ellipse.c
  tz_local_neuroseg_ellipse.c tz_unipointer.c tz_locne_chain.c tz_graph.c tz_graph_workspace.c
  tz_arrayqueue.c tz_graph_utils.c tz_voxel_linked_list_ext.c tz_locseg_chain.c tz_locseg_chain_exp.c
  tz_trace_utils.c tz_stack.c tz_stack_graph.c tz_neuron_component.c tz_neuron_structure.c tz_swc_cell.c
  tz_color.c tz_neurocomp_conn.c tz_random.c tz_math.c tz_mc_stack.c tz_image_trans.c tz_string_workspace.c
  tz_locseg_chain_network.c tz_rastergeom.c tz_tiff_image.c tz_geo3d_ball.c tz_intpair_map.c tz_workspace.c
  tz_xml_utils.c tz_stack_document.c tz_swc.c tz_swc_tree.c tz_locseg_chain_knot.c tz_geo3d_ellipse.c
  tz_stack_tile.c tz_sp_grow.c tz_stack_watershed.c tz_geo3d_cuboid.c tz_cuboid_i.c tz_stack_tile_i.c
  tz_testdata.c tz_local_rpi_neuroseg.c tz_rpi_neuroseg.c tz_receptor_transform.c tz_r2_rect.c tz_r2_ellipse.c
  tz_apo.c tz_png_io.c tz_file_list.c tz_json.c
)

# ======================
# Orchestration: make sure generation/copy happens before compilation
# ======================
set(ALL_GENERATED_FILES ${GENERATED_C_SOURCES} ${GENERATED_C_HEADERS} ${MYERS_SOURCES})
add_custom_target(generate_all DEPENDS ${ALL_GENERATED_FILES})
add_dependencies(generate_all copy_src_c_non_templated copy_src_c_private mirror_generated_headers_to_src)

# ======================
# Final static library
# ======================
add_library(neurolabi STATIC
  ${NEULIB_SOURCES}
  ${GENERATED_C_SOURCES}
  ${GENERATED_C_HEADERS}  # list headers so Ninja knows they are produced
  ${MYERS_SOURCES}
)

if(USE_LIBXML2)
  target_compile_definitions(neurolabi PRIVATE HAVE_LIBXML2)
  if(USE_LIBJANSSON)
    target_compile_definitions(neurolabi PRIVATE HAVE_JANSSON HAVE_LIBJANSSON)
  endif()
  if(WIN32)
    target_compile_definitions(neurolabi PRIVATE LIBXML_STATIC)
  endif()
endif()

add_dependencies(neurolabi generate_all generate_myers_embedded)

# These two compile as C++ (headers have extern "C" guards)
set_source_files_properties(
  tz_complex.c
  tz_complex_f.c
  PROPERTIES LANGUAGE CXX
)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include paths:
target_include_directories(neurolabi
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/neurolabi>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}                 # src/c for legacy relative includes
    ${CMAKE_CURRENT_SOURCE_DIR}/private         # private headers if any
    ${CMAKE_CURRENT_BINARY_DIR}                 # generated headers
    ${CMAKE_CURRENT_BINARY_DIR}/private         # copied private tree
    ${CMAKE_CURRENT_BINARY_DIR}/myers           # generated Myers .c included by code
    ${MYERS_DIR}                                # original .p manager scripts sometimes include headers
)


target_link_libraries(neurolabi PRIVATE ${EXT_LIBS})

# Install (mirror legacy layout)
install(TARGETS neurolabi ARCHIVE DESTINATION lib)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION include/neurolabi
        FILES_MATCHING PATTERN "*.h")
