cmake_minimum_required(VERSION 3.10.2)

project(OS-Practice VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ============================================================
# Macro to add exercises from a folder
# Usage: add_exercise_folder(folder_name source_files...)
# ============================================================
macro(add_exercise_folder FOLDER)
    file(GLOB EXERCISE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${FOLDER}/*.cpp")
    foreach(SOURCE_FILE ${EXERCISE_SOURCES})
        get_filename_component(EXERCISE_NAME ${SOURCE_FILE} NAME_WE)
        set(TARGET_NAME "${FOLDER}_${EXERCISE_NAME}")
        add_executable(${TARGET_NAME} ${SOURCE_FILE})

        # Set output directory to keep executables organized
        set_target_properties(${TARGET_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${FOLDER}"
        )

        # Add common Windows libraries
        target_link_libraries(${TARGET_NAME} PRIVATE psapi)
    endforeach()
endmacro()

# ============================================================
# Add all exercise folders
# ============================================================
add_exercise_folder(01_process)
add_exercise_folder(02_thread)
add_exercise_folder(03_file_io)
add_exercise_folder(04_ipc)
add_exercise_folder(05_environment)
add_exercise_folder(06_system_info)
add_exercise_folder(07_console)
add_exercise_folder(case_studies)
