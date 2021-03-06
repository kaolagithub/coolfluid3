// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef CF_LibPhysics_hpp
#define CF_LibPhysics_hpp

////////////////////////////////////////////////////////////////////////////////

#include "Common/CLibrary.hpp"

////////////////////////////////////////////////////////////////////////////////

/// Define the macro Physics_API
/// @note build system defines COOLFLUID_SOLVER_EXPORTS when compiling MeshTools files
#ifdef COOLFLUID_SOLVER_EXPORTS
#   define Physics_API      CF_EXPORT_API
#   define Physics_TEMPLATE
#else
#   define Physics_API      CF_IMPORT_API
#   define Physics_TEMPLATE CF_TEMPLATE_EXTERN
#endif

////////////////////////////////////////////////////////////////////////////////

namespace CF {

  /// Basic Classes for Physics applications used by CF
  namespace Physics {

////////////////////////////////////////////////////////////////////////////////

  /// Class defines the initialization and termination of the library Physics
  /// @author Tiago Quintino
  /// @author Martin Vymazal
  class Physics_API LibPhysics : public Common::CLibrary {

  public:

    typedef boost::shared_ptr<LibPhysics> Ptr;
    typedef boost::shared_ptr<LibPhysics const> ConstPtr;

    /// Constructor
    LibPhysics ( const std::string& name) : Common::CLibrary(name) {   }

  public: // functions

    /// @return string of the library namespace
    static std::string library_namespace() { return "CF.Physics"; }

    /// Static function that returns the library name.
    /// Must be implemented for CLibrary registration
    /// @return name of the library
    static std::string library_name() { return "Physics"; }

    /// Static function that returns the description of the library.
    /// Must be implemented for CLibrary registration
    /// @return description of the library

    static std::string library_description()
    {
      return "This library implements the Physics API.";
    }

    /// Gets the Class name
    static std::string type_name() { return "LibPhysics"; }

  protected:

    /// initiate library
    virtual void initiate_impl();

    /// terminate library
    virtual void terminate_impl();

  }; // end LibPhysics

////////////////////////////////////////////////////////////////////////////////

} // Physics
} // CF

////////////////////////////////////////////////////////////////////////////////

#endif // CF_LibPhysics_hpp
