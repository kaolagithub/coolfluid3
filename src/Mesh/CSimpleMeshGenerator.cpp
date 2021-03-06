// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include "Common/CBuilder.hpp"
#include "Common/OptionArray.hpp"
#include "Common/OptionURI.hpp"
#include "Common/OptionT.hpp"
#include "Common/FindComponents.hpp"
#include "Common/Foreach.hpp"
#include "Common/MPI/PE.hpp"
#include "Common/MPI/debug.hpp"
#include "Common/Log.hpp"
#include "Common/CRoot.hpp"
#include "Common/Core.hpp"

#include "Mesh/CMixedHash.hpp"
#include "Mesh/CHash.hpp"
#include "Mesh/CSimpleMeshGenerator.hpp"
#include "Mesh/CRegion.hpp"
#include "Mesh/CNodes.hpp"
#include "Mesh/CMeshElements.hpp"
#include "Mesh/CCells.hpp"
#include "Mesh/CFaces.hpp"
#include "Mesh/CElements.hpp"

namespace CF {
namespace Mesh {

using namespace Common;
using namespace Common::XML;

////////////////////////////////////////////////////////////////////////////////

ComponentBuilder < CSimpleMeshGenerator, CMeshGenerator, LibMesh > CSimpleMeshGenerator_Builder;

////////////////////////////////////////////////////////////////////////////////

CSimpleMeshGenerator::CSimpleMeshGenerator ( const std::string& name  ) :
  CMeshGenerator ( name )
{
  mark_basic();

  m_options.add_option<OptionArrayT<Uint> >("nb_cells", m_nb_cells)
      ->description("Vector of number of cells in each direction")
      ->pretty_name("Number of Cells")
      ->link_to(&m_nb_cells)
      ->mark_basic();

  m_options.add_option<OptionArrayT<Real> >("lengths", m_lengths)
      ->description("Vector of lengths each direction")
      ->pretty_name("Lengths")
      ->link_to(&m_lengths)
      ->mark_basic();

  m_options.add_option(OptionT<Uint>::create("nb_parts", mpi::PE::instance().size()))
      ->description("Total number of partitions (e.g. number of processors)")
      ->pretty_name("Number of Partitions");

  m_options.add_option(OptionT<bool>::create("bdry", true))
      ->description("Generate Boundary")
      ->pretty_name("Boundary");
}

////////////////////////////////////////////////////////////////////////////////

CSimpleMeshGenerator::~CSimpleMeshGenerator()
{
}

//////////////////////////////////////////////////////////////////////////////

void CSimpleMeshGenerator::execute()
{
  if ( m_parent.expired() )
    throw SetupError(FromHere(), "Parent component not set");

  m_mesh = m_parent.lock()->create_component_ptr<CMesh>(m_name);

  if (m_nb_cells.size() == 1 && m_lengths.size() == 1)
  {
    create_line(*m_mesh.lock(), m_lengths[0],m_nb_cells[0],option("nb_parts").value<Uint>(), option("bdry").value<bool>());
  }
  else if (m_nb_cells.size() == 2  && m_lengths.size() == 2)
  {
    create_rectangle(*m_mesh.lock(), m_lengths[0],m_lengths[1],m_nb_cells[0],m_nb_cells[1],option("nb_parts").value<Uint>(),option("bdry").value<bool>());
  }
  else
  {
    throw SetupError(FromHere(), "Invalid size of the vector number of cells. Only 1D and 2D supported now.");
  }

}

////////////////////////////////////////////////////////////////////////////////

void CSimpleMeshGenerator::create_line(CMesh& mesh, const Real x_len, const Uint x_segments, const Uint nb_parts, const bool bdry)
{
  Uint part = mpi::PE::instance().rank();
  enum HashType { NODES=0, ELEMS=1 };
  // Create a hash
  CMixedHash& hash = Core::instance().root().create_component<CMixedHash>("tmp_hash");
  std::vector<Uint> num_obj(2);
  num_obj[NODES] = x_segments+1;
  num_obj[ELEMS] = x_segments;
  hash.configure_option("nb_obj",num_obj);

  CRegion& region = mesh.topology().create_region("fluid");
  CNodes& nodes = mesh.nodes();
  mesh.initialize_nodes(hash.subhash(ELEMS).nb_objects_in_part(part) + 1 , DIM_1D);

  CCells& cells = region.create_component<CCells>("Line");
  cells.initialize("CF.Mesh.SF.Line1DLagrangeP1",nodes);
  CConnectivity& connectivity = cells.node_connectivity();
  connectivity.resize(hash.subhash(ELEMS).nb_objects_in_part(part));
  CList<Uint>& elem_rank = cells.rank();
  elem_rank.resize(connectivity.size());
  const Real x_step = x_len / static_cast<Real>(x_segments);
  Uint node_idx(0);
  Uint elem_idx(0);
  for(Uint i = hash.subhash(NODES).start_idx_in_part(part); i < hash.subhash(NODES).end_idx_in_part(part); ++i)
  {
    nodes.coordinates()[node_idx][XX] = static_cast<Real>(i) * x_step;
    nodes.rank()[node_idx] = part;
    ++node_idx;
  }
  for(Uint i = hash.subhash(ELEMS).start_idx_in_part(part); i < hash.subhash(ELEMS).end_idx_in_part(part); ++i)
  {
    if (hash.subhash(NODES).owns(i) == false)
    {
      nodes.coordinates()[node_idx][XX] = static_cast<Real>(i) * x_step;
      nodes.rank()[node_idx] = hash.subhash(NODES).proc_of_obj(i);
      connectivity[elem_idx][0]=node_idx;
      elem_rank[elem_idx] = part;
      ++node_idx;
    }
    else
    {
      connectivity[elem_idx][0]=elem_idx;
      elem_rank[elem_idx] = part;
    }

    if (hash.subhash(NODES).owns(i+1) == false)
    {
      nodes.coordinates()[node_idx][XX] = static_cast<Real>(i+1) * x_step;
      nodes.rank()[node_idx] = hash.subhash(NODES).proc_of_obj(i+1);
      connectivity[elem_idx][1]=node_idx;
      elem_rank[elem_idx] = part;
      ++node_idx;
    }
    else
    {
      connectivity[elem_idx][1]=elem_idx+1;
      elem_rank[elem_idx] = part;
    }
    ++elem_idx;
  }

  if (bdry)
  {
    // Left boundary point
    CFaces& xneg = mesh.topology().create_region("xneg").create_component<CFaces>("Point");
    xneg.initialize("CF.Mesh.SF.Point1DLagrangeP0", nodes);
    if (part == 0)
    {
      CConnectivity& xneg_connectivity = xneg.node_connectivity();
      xneg_connectivity.resize(1);
      xneg_connectivity[0][0] = 0;
      CList<Uint>& xneg_rank = xneg.rank();
      xneg_rank.resize(1);
      xneg_rank[0] = part;
    }

    // right boundary point
    CFaces& xpos = mesh.topology().create_region("xpos").create_component<CFaces>("Point");
    xpos.initialize("CF.Mesh.SF.Point1DLagrangeP0", nodes);
    if(part == nb_parts-1)
    {
      CConnectivity& xpos_connectivity = xpos.node_connectivity();
      xpos_connectivity.resize(1);
      xpos_connectivity[0][0] = connectivity[hash.subhash(ELEMS).nb_objects_in_part(part)-1][1];
      CList<Uint>& xpos_rank = xpos.rank();
      xpos_rank.resize(1);
      xpos_rank[0] = part;
    }
  }
  mesh_loaded(mesh);

  Core::instance().root().remove_component(hash);
}

////////////////////////////////////////////////////////////////////////////////

void CSimpleMeshGenerator::create_rectangle(CMesh& mesh, const Real x_len, const Real y_len, const Uint x_segments, const Uint y_segments , const Uint nb_parts , const bool bdry)
{
  Uint part = mpi::PE::instance().rank();
  enum HashType { NODES=0, ELEMS=1 };
  // Create a hash
  CMixedHash& hash = Core::instance().root().create_component<CMixedHash>("tmp_hash");
  std::vector<Uint> num_obj(2);
  num_obj[NODES] = (x_segments+1)*(y_segments+1);
  num_obj[ELEMS] = x_segments*y_segments;
  hash.configure_option("nb_obj",num_obj);

  CRegion& region = mesh.topology().create_region("region");
  CNodes& nodes = mesh.nodes();


  // find ghost nodes
  std::map<Uint,Uint> ghost_nodes_loc;
  Uint glb_node_idx;
  for(Uint j = 0; j < y_segments; ++j)
  {
    for(Uint i = 0; i < x_segments; ++i)
    {
      if (hash.subhash(ELEMS).owns(i+j*x_segments))
      {
        glb_node_idx = j * (x_segments+1) + i;
        if (hash.subhash(NODES).owns(glb_node_idx) == false)
        {
          ghost_nodes_loc[glb_node_idx]=0;
        }

        glb_node_idx = j * (x_segments+1) + (i+1);
        if (hash.subhash(NODES).owns(glb_node_idx) == false)
        {
          ghost_nodes_loc[glb_node_idx]=0;
        }
        glb_node_idx = (j+1) * (x_segments+1) + i;
        \
        if (hash.subhash(NODES).owns(glb_node_idx) == false)
        {
          ghost_nodes_loc[glb_node_idx]=0;
        }

        glb_node_idx = (j+1) * (x_segments+1) + (i+1);
        if (hash.subhash(NODES).owns(glb_node_idx) == false)
        {
          ghost_nodes_loc[glb_node_idx]=0;
        }
      }
    }
  }

  mesh.initialize_nodes(hash.subhash(NODES).nb_objects_in_part(part)+ghost_nodes_loc.size(), DIM_2D);
  Uint glb_node_start_idx = hash.subhash(NODES).start_idx_in_part(part);

  const Real x_step = x_len / static_cast<Real>(x_segments);
  const Real y_step = y_len / static_cast<Real>(y_segments);
  Real y;
  for(Uint j = 0; j <= y_segments; ++j)
  {
    y = static_cast<Real>(j) * y_step;
    for(Uint i = 0; i <= x_segments; ++i)
    {
      glb_node_idx = j * (x_segments+1) + i;

      if (hash.subhash(NODES).owns(glb_node_idx))
      {
        cf_assert(glb_node_idx-glb_node_start_idx < nodes.size());
        CTable<Real>::Row row = nodes.coordinates()[glb_node_idx-glb_node_start_idx];
        row[XX] = static_cast<Real>(i) * x_step;
        row[YY] = y;
        nodes.rank()[glb_node_idx-glb_node_start_idx]=part;
      }
    }
  }
  // add ghost nodes
  Uint glb_ghost_node_start_idx = hash.subhash(NODES).nb_objects_in_part(part);
  Uint loc_node_idx = glb_ghost_node_start_idx;
  cf_assert(glb_ghost_node_start_idx <= nodes.size());
  foreach_container((const Uint glb_ghost_node_idx) (Uint& loc_ghost_node_idx),ghost_nodes_loc)
  {
    Uint j = glb_ghost_node_idx / (x_segments+1);
    Uint i = glb_ghost_node_idx - j*(x_segments+1);
    loc_ghost_node_idx = loc_node_idx++;
    cf_assert(loc_ghost_node_idx < nodes.size());
    CTable<Real>::Row row = nodes.coordinates()[loc_ghost_node_idx];
    row[XX] = static_cast<Real>(i) * x_step;
    row[YY] = static_cast<Real>(j) * y_step;
    nodes.rank()[loc_ghost_node_idx]=hash.subhash(NODES).proc_of_obj(glb_ghost_node_idx);
  }
  CCells::Ptr cells = region.create_component_ptr<CCells>("Quad");
  cells->initialize("CF.Mesh.SF.Quad2DLagrangeP1",nodes);

  CConnectivity& connectivity = cells->node_connectivity();
  connectivity.resize(hash.subhash(ELEMS).nb_objects_in_part(part));

  CList<Uint>& elem_rank = cells->rank();
  elem_rank.resize(connectivity.size());

  Uint glb_elem_start_idx = hash.subhash(ELEMS).start_idx_in_part(part);
  Uint glb_elem_idx;
  for(Uint j = 0; j < y_segments; ++j)
  {
    for(Uint i = 0; i < x_segments; ++i)
    {
      glb_elem_idx = j * x_segments + i;

      if (hash.subhash(ELEMS).owns(glb_elem_idx))
      {
        CConnectivity::Row nodes = connectivity[glb_elem_idx-glb_elem_start_idx];
        elem_rank[glb_elem_idx-glb_elem_start_idx] = part;

        glb_node_idx = j * (x_segments+1) + i;
        if (hash.subhash(NODES).owns(glb_node_idx) == false)
          nodes[0] = ghost_nodes_loc[glb_node_idx];
        else
          nodes[0] = glb_node_idx-glb_node_start_idx;

        glb_node_idx = j * (x_segments+1) + (i+1);
        if (hash.subhash(NODES).owns(glb_node_idx) == false)
          nodes[1] = ghost_nodes_loc[glb_node_idx];
        else
          nodes[1] = glb_node_idx-glb_node_start_idx;

        glb_node_idx = (j+1) * (x_segments+1) + i;
        if (hash.subhash(NODES).owns(glb_node_idx) == false)
          nodes[3] = ghost_nodes_loc[glb_node_idx];
        else
          nodes[3] = glb_node_idx-glb_node_start_idx;

        glb_node_idx = (j+1) * (x_segments+1) + (i+1);
        if (hash.subhash(NODES).owns(glb_node_idx) == false)
          nodes[2] = ghost_nodes_loc[glb_node_idx];
        else
          nodes[2] = glb_node_idx-glb_node_start_idx;
      }

    }
  }

  if (bdry)
  {
    std::vector<Real> line_nodes(2);
    CFaces::Ptr left = mesh.topology().create_region("left").create_component_ptr<CFaces>("Line");
    left->initialize("CF.Mesh.SF.Line2DLagrangeP1", nodes);
    CConnectivity::Buffer left_connectivity = left->node_connectivity().create_buffer();
    CList<Uint>::Buffer left_rank = left->rank().create_buffer();
    for(Uint j = 0; j < y_segments; ++j)
    {
      if (hash.subhash(ELEMS).owns(j*x_segments))
      {
        glb_node_idx = j * (x_segments+1);
        if (hash.subhash(NODES).owns(glb_node_idx) == false)
          line_nodes[0] = ghost_nodes_loc[glb_node_idx];
        else
          line_nodes[0] = glb_node_idx-glb_node_start_idx;

        glb_node_idx = (j+1) * (x_segments+1);
        if (hash.subhash(NODES).owns(glb_node_idx) == false)
          line_nodes[1] = ghost_nodes_loc[glb_node_idx];
        else
          line_nodes[1] = glb_node_idx-glb_node_start_idx;

        left_connectivity.add_row(line_nodes);
        left_rank.add_row(part);
      }
    }

    CFaces::Ptr right = mesh.topology().create_region("right").create_component_ptr<CFaces>("Line");
    right->initialize("CF.Mesh.SF.Line2DLagrangeP1", nodes);
    CConnectivity::Buffer right_connectivity = right->node_connectivity().create_buffer();
    CList<Uint>::Buffer right_rank = right->rank().create_buffer();

    for(Uint j = 0; j < y_segments; ++j)
    {
      if (hash.subhash(ELEMS).owns(j*x_segments+x_segments-1))
      {
        glb_node_idx = j * (x_segments+1) + x_segments;
        if (hash.subhash(NODES).owns(glb_node_idx) == false)
          line_nodes[1] = ghost_nodes_loc[glb_node_idx];
        else
          line_nodes[1] = glb_node_idx-glb_node_start_idx;

        glb_node_idx = (j+1) * (x_segments+1) + x_segments;
        if (hash.subhash(NODES).owns(glb_node_idx) == false)
          line_nodes[0] = ghost_nodes_loc[glb_node_idx];
        else
          line_nodes[0] = glb_node_idx-glb_node_start_idx;

        Uint idx = right_connectivity.add_row(line_nodes);
        cf_always_assert(right_rank.add_row(part) == idx);
      }
    }

    CFaces::Ptr bottom = mesh.topology().create_region("bottom").create_component_ptr<CFaces>("Line");
    bottom->initialize("CF.Mesh.SF.Line2DLagrangeP1", nodes);
    CConnectivity::Buffer bottom_connectivity = bottom->node_connectivity().create_buffer();
    CList<Uint>::Buffer bottom_rank = bottom->rank().create_buffer();

    for(Uint i = 0; i < x_segments; ++i)
    {
      if (hash.subhash(ELEMS).owns(i))
      {
        glb_node_idx = i;
        if (hash.subhash(NODES).owns(glb_node_idx) == false)
          line_nodes[0] = ghost_nodes_loc[glb_node_idx];
        else
          line_nodes[0] = glb_node_idx-glb_node_start_idx;

        glb_node_idx = i+1;
        if (hash.subhash(NODES).owns(glb_node_idx) == false)
          line_nodes[1] = ghost_nodes_loc[glb_node_idx];
        else
          line_nodes[1] = glb_node_idx-glb_node_start_idx;

        bottom_connectivity.add_row(line_nodes);
        bottom_rank.add_row(part);
      }
    }

    CFaces::Ptr top = mesh.topology().create_region("top").create_component_ptr<CFaces>("Line");
    top->initialize("CF.Mesh.SF.Line2DLagrangeP1", nodes);
    CConnectivity::Buffer top_connectivity = top->node_connectivity().create_buffer();
    CList<Uint>::Buffer top_rank = top->rank().create_buffer();

    for(Uint i = 0; i < x_segments; ++i)
    {
      if (hash.subhash(ELEMS).owns(y_segments*(x_segments)+i))
      {
        glb_node_idx = y_segments * (x_segments+1) + i;
        if (hash.subhash(NODES).owns(glb_node_idx) == false)
          line_nodes[1] = ghost_nodes_loc[glb_node_idx];
        else
          line_nodes[1] = glb_node_idx-glb_node_start_idx;

        glb_node_idx = y_segments * (x_segments+1) + i+1;
        if (hash.subhash(NODES).owns(glb_node_idx) == false)
          line_nodes[0] = ghost_nodes_loc[glb_node_idx];
        else
          line_nodes[0] = glb_node_idx-glb_node_start_idx;

        top_connectivity.add_row(line_nodes);
        top_rank.add_row(part);
      }
    }
  }
  mesh_loaded(mesh);


  // sanity checks

  boost_foreach(CElements& elements, find_components_recursively<CElements>(mesh.topology()))
  {
    cf_assert_desc(elements.uri().string() + " ( "+to_str(elements.size())+"!="+to_str(elements.rank().size()),elements.size() == elements.rank().size());
    boost_foreach(Uint r, elements.rank().array())
    {
      cf_assert( r == part);
    }
  }

  cf_assert(nodes.rank().size() == nodes.size());
  for (Uint n=0; n<nodes.size(); ++n)
  {
    if ( nodes.rank()[n] != part )
    {
      cf_assert( nodes.is_ghost(n) == true );
    }
    else
    {
      cf_assert( nodes.is_ghost(n) == false );
    }
  }

}

////////////////////////////////////////////////////////////////////////////////

} // Mesh
} // CF
