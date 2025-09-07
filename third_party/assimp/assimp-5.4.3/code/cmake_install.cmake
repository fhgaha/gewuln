# Install script for directory: D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/Assimp")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "libassimp5.4.3-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/lib/Debug/assimp-vc142-mtd.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/lib/Release/assimp-vc142-mt.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/lib/MinSizeRel/assimp-vc142-mt.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/lib/RelWithDebInfo/assimp-vc142-mt.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "libassimp5.4.3" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/bin/Debug/assimp-vc142-mtd.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/bin/Release/assimp-vc142-mt.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/bin/MinSizeRel/assimp-vc142-mt.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/bin/RelWithDebInfo/assimp-vc142-mt.dll")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp" TYPE FILE FILES
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/anim.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/aabb.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/ai_assert.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/camera.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/color4.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/color4.inl"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/config.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/ColladaMetaData.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/commonMetaData.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/defs.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/cfileio.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/light.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/material.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/material.inl"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/matrix3x3.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/matrix3x3.inl"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/matrix4x4.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/matrix4x4.inl"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/mesh.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/ObjMaterial.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/pbrmaterial.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/GltfMaterial.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/postprocess.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/quaternion.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/quaternion.inl"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/scene.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/metadata.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/texture.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/types.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/vector2.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/vector2.inl"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/vector3.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/vector3.inl"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/version.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/cimport.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/AssertHandler.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/importerdesc.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/Importer.hpp"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/DefaultLogger.hpp"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/ProgressHandler.hpp"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/IOStream.hpp"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/IOSystem.hpp"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/Logger.hpp"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/LogStream.hpp"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/NullLogger.hpp"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/cexport.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/Exporter.hpp"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/DefaultIOStream.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/DefaultIOSystem.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/ZipArchiveIOSystem.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/SceneCombiner.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/fast_atof.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/qnan.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/BaseImporter.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/Hash.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/MemoryIOWrapper.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/ParsingUtils.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/StreamReader.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/StreamWriter.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/StringComparison.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/StringUtils.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/SGSpatialSort.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/GenericProperty.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/SpatialSort.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/SkeletonMeshBuilder.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/SmallVector.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/SmoothingGroups.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/SmoothingGroups.inl"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/StandardShapes.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/RemoveComments.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/Subdivision.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/Vertex.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/LineSplitter.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/TinyFormatter.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/Profiler.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/LogAux.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/Bitmap.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/XMLTools.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/IOStreamBuffer.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/CreateAnimMesh.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/XmlParser.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/BlobIOSystem.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/MathFunctions.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/Exceptional.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/ByteSwapper.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/Base64.hpp"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp/Compiler" TYPE FILE FILES
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/Compiler/pushpack1.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/Compiler/poppack1.h"
    "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/code/../include/assimp/Compiler/pstdint.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/bin/Debug/assimp-vc142-mtd.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/bin/Release/assimp-vc142-mt.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/bin/MinSizeRel/assimp-vc142-mt.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "D:/MyProjects/cpp/third_party/assimp/assimp-5.4.3/bin/RelWithDebInfo/assimp-vc142-mt.pdb")
  endif()
endif()

