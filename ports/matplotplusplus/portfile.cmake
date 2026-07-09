message(STATUS " ${PORT}'s gnuplot backend currently requires Gnuplot 5.2.6+.
    Windows users may get a pre-built binary installer from http://www.gnuplot.info/download.html.
    Linux and MacOS users may install it from the system package manager.
    Please visit https://alandefreitas.github.io/matplotplusplus/ for more information."
)

vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO bs3d-dev/matplotplusplus
    REF 600e4b982a76bfaf186258f6c93268f8e6c53fcc
    SHA512 9243b9c9822c5fb82e87466b899a3e1b4f2b6f887b7973da0a535350edc500d4a6c2a0a680a43dae777300a0dae20bc2f0ffa90a2154e72420b16aab051b535b
    HEAD_REF master
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
		-DMATPLOTPP_BUILD_EXPERIMENTAL_OPENGL_BACKEND=OFF
        -DMATPLOTPP_BUILD_EXAMPLES=OFF
        -DMATPLOTPP_BUILD_TESTS=OFF
        -DMATPLOTPP_BUILD_INSTALLER=ON
        -DMATPLOTPP_BUILD_PACKAGE=OFF
        -DMATPLOTPP_BUILD_WITH_PEDANTIC_WARNINGS=OFF
        -DMATPLOTPP_WITH_SYSTEM_CIMG=OFF
		-DMATPLOTPP_WITH_SYSTEM_NODESOUP=OFF
        -DMATPLOTPP_BUILD_HIGH_RESOLUTION_WORLD_MAP=OFF
        -DMATPLOTPP_BUILD_WITH_SANITIZERS=OFF
		-DWITH_JPEG=OFF
		-DWITH_TIFF=OFF
		-DWITH_ZLIB=OFF
		-DWITH_LAPACK=OFF
		-DWITH_BLAS=OFF
		-DWITH_FFTW3=OFF
		-DWITH_OPENCV=OFF		
)

vcpkg_cmake_install()

vcpkg_copy_pdbs()

vcpkg_cmake_config_fixup(PACKAGE_NAME matplot++ CONFIG_PATH lib/cmake/Matplot++)

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include" "${CURRENT_PACKAGES_DIR}/debug/share")

file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
file(INSTALL "${CURRENT_PORT_DIR}/usage" DESTINATION "${CURRENT_PACKAGES_DIR}/share/matplotplusplus/")
