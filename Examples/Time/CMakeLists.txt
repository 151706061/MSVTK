project(msvTimeExamples)
set(KIT TimeExamples)

set(include_dirs
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${MSVTK_INCLUDE_DIRS}
  ${VTK_INCLUDE_DIRS}
  ${CTK_INCLUDE_DIRS}
  ${msvVTKParallel_INCLUDE_DIRS}
  ${msvVTKWidgets_INCLUDE_DIRS}
  ${msvQtWidgets_INCLUDE_DIRS}
  )

include_directories(${include_dirs})

add_definitions( -DDATA_TESTING_PATH="${PROJECT_SOURCE_DIR}/../Data/" )

# msvQTimePlayerWidget
add_executable(TimePlayerWidgetExample msvQTimePlayerWidgetExample.cxx)
target_link_libraries(TimePlayerWidgetExample msvQtWidgets)

# msvVTKFileSeriesReader
add_executable(FileSeriesReaderExample msvVTKFileSeriesReaderExample.cxx)
target_link_libraries(FileSeriesReaderExample msvVTKParallel)
