// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef CF_Common_TypeInfo_hpp
#define CF_Common_TypeInfo_hpp

/// @note This header should be included by including CF.hpp instead.

////////////////////////////////////////////////////////////////////////////////

namespace CF {

  namespace Common {

////////////////////////////////////////////////////////////////////////////////

/// Function to demangle the return of typeid()
Common_API std::string demangle (const char* type);

#define DEMANGLED_TYPEID(a) CF::Common::demangle(typeid(a).name())

////////////////////////////////////////////////////////////////////////////////

  } // namespace Common


  /// @brief Handles type information
  /// This struct allows to associate a type to a string. It is a singleton.
  /// @author Tiago Quintino
  /// @author Quentin Gasper
  struct Common_API TypeInfo : public boost::noncopyable
  {

    /// @brief Constructor.
    /// Registers all supported otion types.
    TypeInfo();

    /// @return Returns the instance
    static TypeInfo& instance();

    /// @brief Registers a type to a string
    /// @param tname The type name
    template <typename TYPE>
     void  regist ( const std::string& tname )
    {
        portable_types[typeid(TYPE).name()] = tname ;
    }

    /// @brief Map for types and their associated string
    /// The key is the type, the value is the associated string. The type
    /// is converted to a string using @c typeid
    std::map< std::string, std::string> portable_types;

  };

  template < typename TYPE >
      std::string class_name ()
  {
    TypeInfo& ti = TypeInfo::instance();
    std::map<std::string, std::string>::const_iterator it =
        ti.portable_types.find(typeid(TYPE).name());

    cf_assert( it != ti.portable_types.end() );

    return it->second;
  }

  std::string class_name_from_typeinfo (const std::type_info & info);

} // namespace CF

////////////////////////////////////////////////////////////////////////////////

#endif // CF_Common_TypeInfo_hpp