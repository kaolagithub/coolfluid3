// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/range/as_literal.hpp>
#include <boost/tokenizer.hpp>

#include "rapidxml/rapidxml.hpp"

#include "Common/URI.hpp"
#include "Common/Assertions.hpp"
#include "Common/BasicExceptions.hpp"
#include "Common/StringConversion.hpp"

#include "Common/XML/CastingFunctions.hpp"
#include "Common/XML/Protocol.hpp"
#include "Common/XML/Map.hpp"

// makes explicit instantiation for all template functions with a same type
#define TEMPLATE_EXPLICIT_INSTANTIATION(T) \
  Common_TEMPLATE template void Map::split_string<T>(const std::string&, const std::string&, std::vector<T>&, int);\
  Common_TEMPLATE template bool Map::value_has_ptr<T>(const XmlNode&);\
  Common_TEMPLATE template XmlNode Map::set_value<T>(const std::string&, const T&, const std::string&);\
  Common_TEMPLATE template XmlNode Map::set_array<T>(const std::string&, const std::vector<T>&, const std::string&, const std::string&);\
  Common_TEMPLATE template T Map::get_value<T>(const std::string&) const;\
  Common_TEMPLATE template std::vector<T> Map::get_array<T>(const std::string&) const;\
  Common_TEMPLATE template std::vector<T> Map::array_to_vector<T>(const XmlNode&, std::string*) const

/////////////////////////////////////////////////////////////////////////////////

namespace CF {
namespace Common {
namespace XML {

///////////////////////////////////////////////////////////////////////////////

template <typename TYPE>
void Map::split_string ( const std::string & str, const std::string & delimiter,
                         std::vector<TYPE> & result, int size )
{
  // make the code a bit more readable
  using namespace boost;            // first_finder() and is_iequal()
  using namespace boost::algorithm; // split_iterator and make_split_iterator()
  typedef split_iterator<std::string::iterator> StringSplitIterator;

  std::string value_str(str);
  StringSplitIterator it = make_split_iterator(value_str, first_finder(delimiter, is_iequal()));

  if(size >= 0)
  {
    result.clear();
    result.resize(size);
  }

  for (int item = 0 ; it != StringSplitIterator() && (size < 0 || item < size);  ++it, ++item)
  {
    try
    {
      if(size < 0)
        result.push_back( from_str<TYPE>( boost::copy_range<std::string>(*it)) );
      else
        result[item] = from_str<TYPE>( boost::copy_range<std::string>(*it) );
    }
    catch(boost::bad_lexical_cast e)
    {
      throw CastingFailed(FromHere(), "Unable to cast [" + boost::copy_range<std::string>(*it) + "] to " +
                          Protocol::Tags::type<TYPE>() + ".");
    }
  }
}

///////////////////////////////////////////////////////////////////////////////

Map::Map(XmlNode node) :
    content(node)
{

}

///////////////////////////////////////////////////////////////////////////////

template<typename TYPE>
XmlNode Map::set_value ( const std::string& value_key, const TYPE & value,
                         const std::string& descr )
{
  cf_assert ( content.is_valid() );

  if( value_key.empty() )
    throw BadValue(FromHere(), "The key is empty.");

  std::string type_name = Protocol::Tags::type<TYPE>();
  std::string value_str = to_str(value); // convert value to string
  XmlNode value_node = find_value( value_key );

  if( !value_node.is_valid() ) // if the value was not found
  {
    // create a "value" node and append it to the map
    value_node = content.add_node( Protocol::Tags::node_value() );

    // create the type node (with option value) and append it to the "value" node
    value_node.add_node(type_name, value_str);

    // add "key" attribute
    value_node.set_attribute( Protocol::Tags::attr_key(), value_key );
  }
  else if( std::strcmp(value_node.content->name(), Protocol::Tags::node_array()) == 0 )
    throw XmlError(FromHere(), "Value [" + value_key +"] is an array.");
  else
  {
    rapidxml::xml_node<> * type_node = value_node.content->first_node();

    if( type_node == nullptr )
      throw XmlError(FromHere(), "No type found for value [" + value_key + "].");

    if( type_name != type_node->name() )
      throw XmlError(FromHere(), std::string("Value is of type [") + type_node->name() +
                     "]. Cannot put a value of type [" + type_name + "].");

    type_node->value( type_node->document()->allocate_string(value_str.c_str()) );
  }

  if( !descr.empty() )
    value_node.set_attribute( Protocol::Tags::attr_descr(), descr );

  return value_node;
}

/////////////////////////////////////////////////////////////////////////////////

template<typename TYPE>
XmlNode Map::set_array ( const std::string& value_key,
                         const std::vector<TYPE> & value,
                         const std::string& delimiter,
                         const std::string& descr)
{
  cf_assert ( content.is_valid() );
//  cf_assert ( is_not_null(content.content->document()) );

  if( value_key.empty() )
    throw BadValue(FromHere(), "The key is empty.");

  if( delimiter.empty() )
    throw BadValue(FromHere(), "The delimiter is empty.");

  std::string type_name = Protocol::Tags::type<TYPE>();
  std::string value_str;
  XmlNode array_node = find_value( value_key );

  // build the value string
  typename std::vector<TYPE>::const_iterator it = value.begin();

  for( ; it != value.end() ; ++it )
  {
    // if it is not the first item, we add the delimiter
    if( !value_str.empty() )
      value_str += delimiter;

    value_str += to_str(*it);
  }

  if( !array_node.is_valid() ) // if the array was not found
  {
    // create an "array" node and append it to the map
    array_node = content.add_node( Protocol::Tags::node_array() );

    // add "key" and "type" attributes
    array_node.set_attribute( Protocol::Tags::attr_key(), value_key );
    array_node.set_attribute( Protocol::Tags::attr_array_type(), type_name );
  }
  else if( std::strcmp(array_node.content->name(), Protocol::Tags::node_value()) == 0 )
    throw XmlError(FromHere(), "Value [" + value_key +"] is not an array.");
  else
  {
    rapidxml::xml_attribute<> * type_attr = nullptr;
    // rapidxml::xml_node<> * child_node = nullptr;

    type_attr = array_node.content->first_attribute( Protocol::Tags::attr_array_type());

    if( type_attr == nullptr )
      throw XmlError(FromHere(), "No type found for array [" + value_key + "].");

    if( type_name != type_attr->value() )
      throw XmlError(FromHere(), std::string("Array is of type [") + type_attr->value() +
                     "]. Cannot put an array of type [" + type_name + "].");

  }

  // common modifications: set the array size and the delimiter

  // note: we explicitly cast the array size to CF::Uint for compatibility
  // reasons: MSVC's typedef of std::size_t is for unsigned __int64
  // (which is defined by Microsoft) instead of unsigned int. Since
  // to_str<unsigned __int64>() is not defined, it may cause a
  // lining error on Windows.
  array_node.set_attribute( Protocol::Tags::attr_array_size(), to_str( (CF::Uint) value.size() ));

  array_node.set_attribute( Protocol::Tags::attr_array_delimiter(), delimiter );

  array_node.content->value( array_node.content->document()->allocate_string(value_str.c_str()) );

  if( !descr.empty() )
    array_node.set_attribute( Protocol::Tags::attr_descr(), descr );

  return array_node;
}

/////////////////////////////////////////////////////////////////////////////////

bool Map::check_entry ( const std::string & entry_key ) const
{
  cf_assert( !entry_key.empty() );

  return find_value(entry_key).is_valid();
}

/////////////////////////////////////////////////////////////////////////////////

bool Map::is_single_value ( const XmlNode& node )
{
  return node.is_valid() && std::strcmp(node.content->name(), Protocol::Tags::node_value()) == 0;
}

/////////////////////////////////////////////////////////////////////////////////

bool Map::is_array_value ( const XmlNode& node )
{
  return node.is_valid() && std::strcmp(node.content->name(), Protocol::Tags::node_array()) == 0;
}

/////////////////////////////////////////////////////////////////////////////////

const char * Map::get_value_type ( const XmlNode& node )
{
  cf_assert( node.is_valid() );

  const char * type = nullptr;

  if( is_single_value(node) )
  {
    rapidxml::xml_node<>* type_node = node.content->first_node();

    type = is_null(type_node) ? nullptr : type_node->name();
  }
  else if( is_array_value(node) )
  {
    rapidxml::xml_attribute<>* type_attr = node.content->first_attribute( Protocol::Tags::attr_array_type() );

    type = is_null(type_attr) ? nullptr : type_attr->value();
  }
  else
    throw XmlError(FromHere(), "The node does not describe neither a "
                   "single not an array value.");

  if( is_null(type) )
    throw XmlError(FromHere(), "Could not find a type.");

  return type;
}

/////////////////////////////////////////////////////////////////////////////////

XmlNode Map::find_value ( const std::string & value_key, const char * value_type) const
{
  cf_assert ( content.is_valid() );

  XmlNode found_value;

  if( is_null(value_type) ||
      std::strcmp(value_type, Protocol::Tags::node_array()) == 0 ||
      std::strcmp(value_type, Protocol::Tags::node_value()) == 0 )
  {


    if( value_key.empty() )
      found_value.content = content.content->first_node(value_type);
    else
    {
      rapidxml::xml_node<> * curr_child = content.content->first_node(value_type);

      for( ; curr_child != nullptr ; curr_child = curr_child->next_sibling(value_type))
      {
        rapidxml::xml_attribute<> * key_attr = curr_child->first_attribute( Protocol::Tags::attr_key() );

        if( key_attr != nullptr && value_key == key_attr->value() )
        {
          found_value.content = curr_child;
          break;
        }
      } // END "for"

    } // END "if( !value_key.empty() )"
  }

  return found_value;
}

/////////////////////////////////////////////////////////////////////////////////

template<typename TYPE>
TYPE Map::get_value ( const std::string& value_key ) const
{
  cf_assert ( content.is_valid() );

  if(value_key.empty())
    throw BadValue(FromHere(), "The value key is empty.");

  XmlNode found_node;
  const char * type_name = Protocol::Tags::type<TYPE>();

  found_node = find_value( value_key, Protocol::Tags::node_value() );

  if( !found_node.is_valid() || !value_has_ptr<TYPE>(found_node) )
    throw  XmlError( FromHere(), "Could not find single value of type [" +
                     std::string(type_name) + "] with \'key\' attribute  [" +
                     value_key + "]." );

  // convert xml value to TYPE
  XmlNode tmpnode(found_node.content->first_node());
  return to_value<TYPE>( tmpnode );
}

/////////////////////////////////////////////////////////////////////////////////

template<typename TYPE>
std::vector<TYPE> Map::get_array ( const std::string& array_key ) const
{
  cf_assert ( content.is_valid() );

  const char * type_name = Protocol::Tags::type<TYPE>();
  // rapidxml::xml_attribute<> * delim_attr = nullptr;

  if(array_key.empty())
    throw BadValue(FromHere(), "The array key is empty.");

  // search for the node
  // note: seek_value_in() throws an exception if map is not valid
  XmlNode found_node = find_value( array_key, Protocol::Tags::node_array() );

  // check that node has been found and has the correct type
  if( !found_node.is_valid() || !value_has_ptr<TYPE>(found_node) )
    throw  XmlError( FromHere(),"Could not find an array value of type [" +
                     std::string(type_name) + "] with \'key\' attribute  [" +
                     array_key + "]." );


  return array_to_vector<TYPE>( found_node );
}

///////////////////////////////////////////////////////////////////////////////////

template<typename TYPE>
std::vector<TYPE> Map::array_to_vector ( const XmlNode & array_node, std::string * delim ) const
{
  cf_assert( array_node.is_valid() );
  std::vector<TYPE> result;
  rapidxml::xml_attribute<> * delim_attr = nullptr;

  delim_attr = array_node.content->first_attribute( Protocol::Tags::attr_array_delimiter() );

  // check that the delimiter is present and not empty
  if(delim_attr == nullptr || delim_attr->value()[0] == '\0')
    throw XmlError(FromHere(), "No delimiter found.");

  if( is_not_null(delim) )
    *delim = delim_attr->value();

  rapidxml::xml_attribute<>* size_attr = array_node.content->first_attribute( Protocol::Tags::attr_array_size() );
  if ( !size_attr )
    throw ParsingFailed (FromHere(), "Array does not have \'size\' attribute" );

  Uint expected_size = from_str<Uint>( size_attr->value() );

  // convert xml value to TYPE
  split_string(array_node.content->value(), delim_attr->value(), result, expected_size);

  if ( expected_size != result.size() )
    throw ParsingFailed (FromHere(), "Array \'size\' did not match number of entries "
                         "(expected " + to_str(expected_size) + " but found " +
                         to_str(result.size()) + " item(s)).");

  return result;
}


///////////////////////////////////////////////////////////////////////////////////

template<typename TYPE>
bool Map::value_has_ptr ( const XmlNode &node )
{
  bool type_ok = false;
  const char * type_name = Protocol::Tags::type<TYPE>();

  if( is_single_value(node) )
    type_ok = node.content->first_node( type_name ) != nullptr;
  else if( is_array_value(node) )
  {
    rapidxml::xml_attribute<>* type_attr = node.content->first_attribute( Protocol::Tags::attr_array_type() );

    type_ok = type_attr != nullptr && std::strcmp(type_name, type_attr->value()) == 0;
  }

  return type_ok;
}

///////////////////////////////////////////////////////////////////////////////////

/// explicit instantiation to avoid missing symbols on certain compilers
TEMPLATE_EXPLICIT_INSTANTIATION( bool );
TEMPLATE_EXPLICIT_INSTANTIATION( int );
TEMPLATE_EXPLICIT_INSTANTIATION( std::string );
TEMPLATE_EXPLICIT_INSTANTIATION( CF::Uint );
TEMPLATE_EXPLICIT_INSTANTIATION( CF::Real );
TEMPLATE_EXPLICIT_INSTANTIATION( CF::Common::URI );

/////////////////////////////////////////////////////////////////////////////////

} // XML
} // Common
} // CF

#undef TEMPLATE_EXPLICIT_INSTANTIATION
