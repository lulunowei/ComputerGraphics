cmake_minimum_required(VERSION 3.10)
project(vulkanTriangle)

# 使用 C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找 Vulkan SDK（确保你已经安装）
find_package(Vulkan REQUIRED)

# 包含目录
include_directories(${Vulkan_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# 源文件
set(SOURCES
    main.cpp
    FileUtils.cpp
    Matrix.cpp
    Triangle.cpp
    VertexData.cpp
)

set(HEADERS
    const.h
    FileUtils.h
    Matrix.h
    Triangle.h
    VertexData.h
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# 链接 Vulkan 库
target_link_libraries(${PROJECT_NAME} ${Vulkan_LIBRARIES})

# 设置资源路径变量（用于你将来需要复制 shader、texture 等资源）
set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
set(TEXTURE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/textures)
set(MODEL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/models)

# 可选：把 shader 编译集成进构建流程（如果不需要，可以注释掉）
file(GLOB SHADER_SOURCES "${SHADER_DIR}/*.vert" "${SHADER_DIR}/*.frag")
foreach(SHADER ${SHADER_SOURCES})
    get_filename_component(FILE_NAME ${SHADER} NAME)
    set(SPIRV "${CMAKE_CURRENT_BINARY_DIR}/shaders/${FILE_NAME}.spv")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/shaders
        COMMAND glslangValidator -V ${SHADER} -o ${SPIRV}
        DEPENDS ${SHADER}
        COMMENT "Compiling shader ${FILE_NAME}"
    )
    list(APPEND SPIRV_SHADERS ${SPIRV})
endforeach()

add_custom_target(CompileShaders ALL DEPENDS ${SPIRV_SHADERS})
add_dependencies(${PROJECT_NAME} CompileShaders)
