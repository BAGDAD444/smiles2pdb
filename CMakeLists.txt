cmake_minimum_required(VERSION 3.10)
project(smiles2pdb)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Force static libraries only
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

# Find Boost static libraries
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS serialization iostreams)

# Set RDKit paths
set(RDKIT_INCLUDE_DIR /opt/rdkit/include/rdkit)
set(RDKIT_LIB_DIR /opt/rdkit/lib)

# Add executable
add_executable(smiles2pdb smiles2pdb.cpp)

# Include directories
target_include_directories(smiles2pdb PRIVATE ${RDKIT_INCLUDE_DIR})

# Link required static libraries
target_link_libraries(smiles2pdb
    -Wl,--start-group
    # Start/End the group to avoid circular dependencies
    ${RDKIT_LIB_DIR}/libRDKitSmilesParse_static.a
    ${RDKIT_LIB_DIR}/libRDKitFileParsers_static.a
    ${RDKIT_LIB_DIR}/libRDKitDistGeomHelpers_static.a
    ${RDKIT_LIB_DIR}/libRDKitForceFieldHelpers_static.a
    ${RDKIT_LIB_DIR}/libRDKitDescriptors_static.a
    ${RDKIT_LIB_DIR}/libRDKitPartialCharges_static.a
    ${RDKIT_LIB_DIR}/libRDKitDepictor_static.a
    ${RDKIT_LIB_DIR}/libRDKitSubstructMatch_static.a
    ${RDKIT_LIB_DIR}/libRDKitMolAlign_static.a
    ${RDKIT_LIB_DIR}/libRDKitMolTransforms_static.a
    ${RDKIT_LIB_DIR}/libRDKitAlignment_static.a
    ${RDKIT_LIB_DIR}/libRDKitDistGeometry_static.a
    ${RDKIT_LIB_DIR}/libRDKitForceField_static.a
    ${RDKIT_LIB_DIR}/libRDKitOptimizer_static.a
    ${RDKIT_LIB_DIR}/libRDKitGraphMol_static.a
    ${RDKIT_LIB_DIR}/libRDKitGenericGroups_static.a
    ${RDKIT_LIB_DIR}/libRDKitChemTransforms_static.a
    ${RDKIT_LIB_DIR}/libRDKitCIPLabeler_static.a
    ${RDKIT_LIB_DIR}/libRDKitEigenSolvers_static.a
    ${RDKIT_LIB_DIR}/libRDKitSubgraphs_static.a
    ${RDKIT_LIB_DIR}/libRDKitRDGeometryLib_static.a
    ${RDKIT_LIB_DIR}/libRDKitRingDecomposerLib_static.a
    ${RDKIT_LIB_DIR}/libRDKitDataStructs_static.a
    ${RDKIT_LIB_DIR}/libRDKitRDGeneral_static.a
    ${RDKIT_LIB_DIR}/libRDKitMolStandardize_static.a
    ${RDKIT_LIB_DIR}/libRDKitFingerprints_static.a
    ${RDKIT_LIB_DIR}/libRDKitFilterCatalog_static.a
    ${RDKIT_LIB_DIR}/libRDKitCatalogs_static.a
    -Wl,--end-group

    # Boost libraries
    Boost::serialization
    Boost::iostreams

    # System libraries
    -lz
    -lbz2
    -llzma
    -lzstd
    -lpthread
    -ldl
)
