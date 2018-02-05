get_filename_component(CMOCKA_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

if (EXISTS "${CMOCKA_CMAKE_DIR}/CMakeCache.txt")
    # In build tree
    include(${CMOCKA_CMAKE_DIR}/cmocka-build-tree-settings.cmake)
else()
    set(CMOCKA_INCLUDE_DIR /home/pavan/APES/HWs/HW2/cmocka_unit_test/ecen5813/tutorials/unit_tests/3rd-party/build-Debug/include)
endif()

set(CMOCKA_LIBRARY /home/pavan/APES/HWs/HW2/cmocka_unit_test/ecen5813/tutorials/unit_tests/3rd-party/build-Debug/lib/libcmocka.so)
set(CMOCKA_LIBRARIES /home/pavan/APES/HWs/HW2/cmocka_unit_test/ecen5813/tutorials/unit_tests/3rd-party/build-Debug/lib/libcmocka.so)
