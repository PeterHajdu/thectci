
set(SOURCE_FILES
  service_registry.cpp
    )

add_library(thectci STATIC ${SOURCE_FILES})
target_link_libraries(thectci ${LIBS})
install(TARGETS thectci DESTINATION lib)

