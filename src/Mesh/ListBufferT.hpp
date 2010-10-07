// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef CF_Mesh_ListBufferT_hpp
#define CF_Mesh_ListBufferT_hpp

////////////////////////////////////////////////////////////////////////////////

#include <deque>

#include <boost/foreach.hpp>

#include "Common/BoostArray.hpp"
#include "Common/BasicExceptions.hpp"

#include "Mesh/LibMesh.hpp"

////////////////////////////////////////////////////////////////////////////////

namespace CF {
namespace Mesh {

////////////////////////////////////////////////////////////////////////////////

/// @brief A Buffer that is constructed passing a boost::multi_array<T,2> table.
/// This class allows to interface this table by using a buffer.
///
/// The idea is to add and remove rows from the table through this buffer.
/// The table is resized when the buffer is full, and values are copied from
/// the buffer into the table.
///
/// @note Before using the matching table or array one has to be sure that
/// the buffer is flushed.
/// @author Willem Deconinck
template < typename T >
class ListBufferT {

public: // typedef
  
  typedef boost::multi_array<T,1> Array_t;
  typedef T value_type;

public: // functions
  
  /// Contructor
  /// @param array The table that will be interfaced with
  /// @param nbRows The size the buffer will be allocated with
  ListBufferT (Array_t& array, size_t nbRows);

  /// Virtual destructor
  virtual ~ListBufferT();

  /// Get the class name
  static std::string type_name () { return "ListBuffer"; }

  // functions specific to the Buffer component
  
  /// Change the buffer to the new size
  void change_buffersize(const size_t nbRows);
  
  /// flush the buffer in the connectivity Buffer
  void flush();
  // 
  /// Add a row to the buffer.
  /// @param [in] row Row to be added to buffer
  /// @return the index in the array+buffers
  Uint add_row(const value_type& val);
  
  /// Add a row directly to the array
  /// @param [in] row Row to be added to buffer or array
  /// @return the index in the array+buffers
  Uint add_row_directly(const value_type& val);  
  
  /// copy a given row into the array or buffer, depending on the given index
  /// @param [in] array_idx the index of the row that will be set (both in array and buffers)
  /// @param [in] row       the row that will be copied into the buffer or array
  void set_row(const Uint array_idx, const value_type& row);

  /// @return the row with index idx, searching both in array and buffers
  value_type& get_row(const Uint idx);

  /// Mark row as empty in array or buffer
  /// @param [in] array_idx the index of the row to be removed
  void rm_row(const Uint array_idx);
    
  /// @return the array that the buffer operates on
  Array_t& get_appointed() {return m_array;}
  
  /// @return total number of allocated rows, including all buffers and the array
  Uint total_allocated();
  
  /// @return the number of buffers that are created
  Uint buffers_count() const { return m_buffers.size(); }
  
  /// increase the size of the array, only to be used when going to write directly in array
  void increase_array_size(const size_t increase);
  
private: // functions

  /// Create a new buffer, allocate it with m_buffersize, and fill m_newBufferRows with the new ones.
  void add_buffer();

  /// Swap 2 rows in a table
  /// @param [in,out] lhs row to be swapped with rhs
  /// @param [in,out] rhs row to be swapped with lhs
  void swap(value_type& lhs,
            value_type& rhs);

  /// @return true if the given row is empty
  bool is_empty(const value_type& val) const  { return val==INVALID; }

  /// mark the given row as empty
  /// @param [in] row the row to be marked as empty
  void set_empty(value_type& val) { val=INVALID; }

private: // data
          
  /// reference to the array that is buffered
  Array_t& m_array;
  
  /// The size newly created buffers will have
  /// @note it is safe to change in the middle of buffer operations
  Uint m_buffersize;
  
  /// definition of an invalid element
  static const T INVALID;
  
  /// vector of temporary buffers
  std::vector<Array_t> m_buffers;
  
  /// storage of removed array rows
  std::deque<Uint> m_emptyArrayRows;
  
  /// storage of array rows where rows can be added directly using add_row_directly
  std::deque<Uint> m_newArrayRows;
  
  /// storage of removed buffer rows
  std::deque<Uint> m_emptyBufferRows;
  
  /// storage of buffer rows where rows can be added
  std::deque<Uint> m_newBufferRows;

}; // end of class ConnectivityTable

//////////////////////////////////////////////////////////////////////////////

template<typename T>
const T ListBufferT<T>::INVALID = std::numeric_limits<T>::max();
  
////////////////////////////////////////////////////////////////////////////////

template<typename T>
ListBufferT<T>::ListBufferT (typename ListBufferT<T>::Array_t& array, size_t nbRows) :
  m_array(array),
  m_buffersize(nbRows)
{
}

////////////////////////////////////////////////////////////////////////////////

template<typename T>
ListBufferT<T>::~ListBufferT()
{
  // make sure to flush before deleting the buffer
  flush();
}

////////////////////////////////////////////////////////////////////////////////

template<typename T>
inline Uint ListBufferT<T>::total_allocated()
{  
  Uint allocated=m_array.size();
  BOOST_FOREACH(const Array_t& buffer, m_buffers)
    allocated += buffer.size();
  return allocated;
}
  
////////////////////////////////////////////////////////////////////////////////

template<typename T>
void ListBufferT<T>::flush()
{
  
  // get total number of allocated rows
  Uint allocated_size = total_allocated();
  Uint old_array_size = m_array.size();
  
  // get total number of empty rows
  Uint nb_emptyRows = m_emptyArrayRows.size() + m_emptyBufferRows.size() + m_newBufferRows.size();
  Uint new_size = allocated_size-nb_emptyRows;
  if (new_size > old_array_size) 
  {
    // make m_array bigger
    m_array.resize(boost::extents[new_size]);
    
    // copy each buffer into the array
    Uint array_idx=old_array_size;
    BOOST_FOREACH (Array_t& buffer, m_buffers)
    BOOST_FOREACH (value_type& row, buffer)
    if (!is_empty(row))   // for each non-empty row from all buffers
    {      
      // first find empty rows inside the old part array
      if (!m_emptyArrayRows.empty()) 
      {
        value_type& empty_array_row = get_row(m_emptyArrayRows.front());
        m_emptyArrayRows.pop_front();
        empty_array_row = row;
      }
      else // then select the new array rows to be filled
      {
        value_type& empty_array_row=m_array[array_idx++];
        empty_array_row = row;
      }
    }
  }
  else // More rows to be removed than added, now we need to swap rows
  {
    // copy all buffer rows in the m_array
    BOOST_FOREACH (Array_t& buffer, m_buffers)
    BOOST_FOREACH (value_type& row, buffer)
    if (!is_empty(row))   // for each non-empty row from all buffers
    {     
      Uint empty_array_row_idx = m_emptyArrayRows.front();
      m_emptyArrayRows.pop_front();
      value_type& empty_array_row = get_row(empty_array_row_idx);
      empty_array_row = row;
    }
    
    Uint full_row_idx = new_size;
    
    // The part of the table with rows > new_size will be deallocated
    // The empty rows from the allocated part must be swapped with filled 
    // rows from the part that will be deallocated
    BOOST_FOREACH(Uint empty_row_idx, m_emptyArrayRows)
    {
      // swap only necessary if it the empty row is in the allocated part
      if (empty_row_idx < new_size)
      {        
        // swap this empty row with a full one in the part that will be deallocated

        // 1) find next full row
        while(is_empty(m_array[full_row_idx]))
          full_row_idx++; 
        
        // 2) swap them   
        swap(m_array[empty_row_idx],m_array[full_row_idx]);
        full_row_idx++;
      }
    }
    
    // make m_array smaller
    m_array.resize(boost::extents[new_size]);
  }

  // clear all buffers
  m_buffers.resize(0);
  m_emptyArrayRows.clear();
  m_emptyBufferRows.clear();
  m_newBufferRows.clear();
}
  
//////////////////////////////////////////////////////////////////////////////

template<typename T>
inline typename ListBufferT<T>::value_type& ListBufferT<T>::get_row(const Uint idx)
{  
  Uint cummulative_size = m_array.size();
  if (idx < cummulative_size) 
  {
    return m_array[idx];
  }
  else 
  {
    BOOST_FOREACH(Array_t& buffer, m_buffers)
    {
      if (idx<cummulative_size+buffer.size())
        return buffer[idx-cummulative_size];
      cummulative_size += buffer.size();
    }
  }
  throw Common::BadValue(FromHere(),"Trying to access index that is not allocated");
}

//////////////////////////////////////////////////////////////////////

template<typename T>
inline void ListBufferT<T>::increase_array_size(const size_t increase)
{
  Uint old_size = m_array.size();
  Uint new_size = old_size+increase;
  m_array.resize(boost::extents[new_size]);
  for (Uint i_new=old_size; i_new<new_size; ++i_new)
  {
    set_empty(m_array[i_new]);
    m_newArrayRows.push_back(i_new);
  }
}
  
//////////////////////////////////////////////////////////////////////

template<typename T>
inline void ListBufferT<T>::add_buffer()
{
  Uint idx = total_allocated();
  m_buffers.push_back(Array_t(boost::extents[m_buffersize]));
  BOOST_FOREACH(value_type& new_row, m_buffers.back())
  { 
    set_empty(new_row);
    m_newBufferRows.push_back(idx++);
  }
  cf_assert(total_allocated()==idx);
}
  
//////////////////////////////////////////////////////////////////////////////

template<typename T>
inline Uint ListBufferT<T>::add_row(const value_type& row)
{ 
  if (m_newBufferRows.empty())
    add_buffer(); // will make a whole lot of new newBufferRows
  Uint idx = m_newBufferRows.front();
  set_row(idx,row);
  m_newBufferRows.pop_front();
  return idx;
}
  
//////////////////////////////////////////////////////////////////////////////

template<typename T>
inline Uint ListBufferT<T>::add_row_directly(const value_type& row)
{ 
  cf_assert(!m_newArrayRows.empty());
  Uint idx = m_newArrayRows.front();
  set_row(idx,row);
  m_newArrayRows.pop_front();
  return idx;
}  
  
//////////////////////////////////////////////////////////////////////

template<typename T>
inline void ListBufferT<T>::set_row(const Uint array_idx, const value_type& row)
{
  value_type& row_to_set = get_row(array_idx);
	row_to_set = row;
}

//////////////////////////////////////////////////////////////////////////////

template<typename T>
inline void ListBufferT<T>::rm_row(const Uint array_idx)
{
  set_empty(get_row(array_idx));
  if (array_idx < m_array.size()) 
    m_emptyArrayRows.push_back(array_idx);
  else
    m_emptyBufferRows.push_back(array_idx);
  
}

//////////////////////////////////////////////////////////////////////////////

template<typename T>
inline void ListBufferT<T>::change_buffersize(const size_t buffersize)
{
  m_buffersize = buffersize;
}

//////////////////////////////////////////////////////////////////////////////

template<typename T>
inline void ListBufferT<T>::swap(
    value_type& lhs,
    value_type& rhs)
{
  value_type tmp = lhs;
  lhs = rhs;
  rhs = tmp;
}


////////////////////////////////////////////////////////////////////////////////

} // Mesh
} // CF

////////////////////////////////////////////////////////////////////////////////

#endif // CF_Mesh_Buffer_hpp