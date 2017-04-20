# Turn back the CMAKE_LIBRARY_PATH string into a list
STRING(REPLACE "," ";" CMAKE_LIBRARY_PATH "${CMAKE_LIBRARY_PATH}")

# On windows, we need to add the paths to all the dll dependencies of the tulip Python modules
# in the PATH environment variable, otherwise the import of the tulip modules will fail
IF(WIN32)
  SET(DEPENDENCIES_PATHS "${BIN_DIR}/../../library/tulip-core/src"
                         "${BIN_DIR}/../../library/tulip-gui/src"
                         "${BIN_DIR}/../../library/tulip-ogl/src"
                         "${BIN_DIR}/../../library/tulip-ogdf/src/"
                         "${BIN_DIR}/../../library/tulip-python/src/"
                         "${BIN_DIR}/../../thirdparty/gzstream"
                         "${BIN_DIR}/../../thirdparty/quazip"
                         "${BIN_DIR}/../../thirdparty/ftgl"
                         "${BIN_DIR}/../../thirdparty/OGDF"
                         "${BIN_DIR}/../../thirdparty/yajl/src"
                         "${BIN_DIR}/../../thirdparty/libtess2"
                         "${CMAKE_LIBRARY_PATH}"
                         "${QT_BIN_DIR}")

  SET(ENV{PATH} "${DEPENDENCIES_PATHS};$ENV{PATH}")
  SET(ENV{QT_QPA_PLATFORM_PLUGIN_PATH} "${QT_BIN_DIR}/../plugins/platforms")

ENDIF(WIN32)

# Add the paths of the tulip Python modules in the PYTHONPATH environement variable
# in order for the interpreter to import them
SET(PYTHON_PATHS "${BIN_DIR}/../../library/tulip-python/bindings/tulip-core/tulip_module"
                 "${BIN_DIR}/../../library/tulip-python/bindings/tulip-gui/tulipgui_module"
                 "${SRC_DIR}/../../library/tulip-python/modules")

# Add the path of the sip Python module if we compile it from thirdparty as
# the tulip modules depend on it
IF(NOT SYSTEM_SIP)
  SET(PYTHON_PATHS "${BIN_DIR}/../../thirdparty/sip-${SIP_VERSION}/siplib"
                   "${PYTHON_PATHS}")

ENDIF(NOT SYSTEM_SIP)

# On non windows systems, the character separating the paths in an environement variable
# is a ':' not a ';' (generated by CMake when turning a list into string)
IF(NOT WIN32)
  STRING(REPLACE ";" ":" PYTHON_PATHS "${PYTHON_PATHS}")
ENDIF(NOT WIN32)

SET(ENV{PYTHONPATH} "${PYTHON_PATHS}")

# Fix an encoding issue on Mac OS
IF(APPLE)
  SET(ENV{LC_ALL} "en_EN.UTF-8")
ENDIF(APPLE)

# Everything is now set up, we can generate the documentation
EXECUTE_PROCESS(COMMAND ${SPHINX_EXECUTABLE} -c ${BIN_DIR} -b html -E -d ${BIN_DIR}/doctrees ${SRC_DIR} ${BIN_DIR}/html)
