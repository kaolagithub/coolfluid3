// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef CF_RDM_LibScalar_hpp
#define CF_RDM_LibScalar_hpp

////////////////////////////////////////////////////////////////////////////////

#include "Common/CLibrary.hpp"

////////////////////////////////////////////////////////////////////////////////

/// Define the macro RDM_SCALAR_API
/// @note build system defines COOLFLUID_RDM_EXPORTS when compiling
/// RDM files
#ifdef COOLFLUID_RDM_SCALAR_EXPORTS
#   define RDM_SCALAR_API      CF_EXPORT_API
#   define RDM_TEMPLATE
#else
#   define RDM_SCALAR_API      CF_IMPORT_API
#   define RDM_SCALAR_TEMPLATE CF_TEMPLATE_EXTERN
#endif

////////////////////////////////////////////////////////////////////////////////

namespace CF {
namespace RDM {

////////////////////////////////////////////////////////////////////////////////

/// Class defines the RDM finite elment method library
/// @author Tiago Quintino
class RDM_SCALAR_API LibScalar : public Common::CLibrary {

public:

  typedef boost::shared_ptr<LibScalar> Ptr;
  typedef boost::shared_ptr<LibScalar const> ConstPtr;

  /// Constructor
  LibScalar ( const std::string& name) : Common::CLibrary(name) {   }

public: // functions

  /// @return string of the library namespace
  static std::string library_namespace() { return "CF.RDM.Scalar"; }

  /// Static function that returns the library name.
  /// Must be implemented for CLibrary registration
  /// @return name of the library
  static std::string library_name() { return "Scalar"; }

  /// Static function that returns the description of the library.
  /// Must be implemented for CLibrary registration
  /// @return description of the library

  static std::string library_description()
  {
    return "This library implements the RDM scalar.";
  }

  /// Gets the Class name
  static std::string type_name() { return "LibScalar"; }

protected:

  /// initiate library
  virtual void initiate_impl();

  /// terminate library
  virtual void terminate_impl();

}; // end LibScalar

///////////////////////////////////////////////////////////////////////////////////////

} // RDM
} // CF

////////////////////////////////////////////////////////////////////////////////

#endif // CF_RDM_LibScalar_hpp
