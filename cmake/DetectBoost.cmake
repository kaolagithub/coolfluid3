# find boost package (essential)
# set( Boost_DEBUG 1 )  # to debug boost search
set( Boost_USE_STATIC_LIBS ${CF_ENABLE_STATIC} )
set( Boost_USE_MULTITHREAD ON  )
# find based on minimal version defined below
set( Boost_FIND_VERSION        ON   )
set( Boost_FIND_VERSION_MAJOR  "1"  )
set( Boost_FIND_VERSION_MINOR  "42" )
set( Boost_FIND_VERSION_PATCH  "0"  )
# older cmakes dont have these versions
set( Boost_ADDITIONAL_VERSIONS "1.44" "1.44.0" "1.43" "1.43.0" "1.42" "1.42.0" )
# components to search for
list( APPEND CF_Boost_COMPONENTS thread iostreams filesystem system regex unit_test_framework date_time program_options mpi serialization )

find_package( Boost COMPONENTS ${CF_Boost_COMPONENTS} )

coolfluid_log( "Boost include path [${Boost_INCLUDE_DIR}]" )
coolfluid_log( "Boost lib version  [${Boost_LIB_VERSION}]" )
coolfluid_log( "Boost libraries    [${Boost_LIBRARIES}]"   )

# if not found give more information
if( NOT Boost_FOUND )
  coolfluid_log( ${Boost_ERROR_REASON} )
  message( FATAL_ERROR "Boost is required to compile coolfluid Kernel" )
endif()

# add boost include path
INCLUDE_DIRECTORIES ( ${Boost_INCLUDE_DIR} )

#######################################################################################

coolfluid_log( "+++++  Checking for boost erfc function" )
set( CMAKE_REQUIRED_INCLUDES ${Boost_INCLUDE_DIR} )
CHECK_CXX_SOURCE_COMPILES (
"#include <boost/math/special_functions/erf.hpp>
int main(int argc, char* argv[])
{
  boost::math::erfc(0.);
}"
CF_HAVE_BOOST_ERFC )

