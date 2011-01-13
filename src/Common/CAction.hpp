// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef CF_Common_CAction_hpp
#define CF_Common_CAction_hpp

#include "Common/Component.hpp"

/////////////////////////////////////////////////////////////////////////////////////

namespace CF {
namespace Common {

///////////////////////////////////////////////////////////////////////////////////////

class Common_API CAction : public Component
{
public: // typedefs

  /// pointers
  typedef boost::shared_ptr<CAction> Ptr;
  typedef boost::shared_ptr<CAction const> ConstPtr;

public: // functions
  /// Contructor
  /// @param name of the component
  CAction ( const std::string& name );

  /// Virtual destructor
  virtual ~CAction() {};

  /// Get the class name
  static std::string type_name () { return "CAction"; }

  /// execute the action
  virtual void execute () = 0;

  /// Templated version for high efficiency
  template < typename T >
  void executeT ()
  {
    execute();
  }

};

/////////////////////////////////////////////////////////////////////////////////////

} // Common
} // CF

/////////////////////////////////////////////////////////////////////////////////////

#endif // CF_Common_CAction_hpp