// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include "Mesh/Neu/Shared.hpp"

//////////////////////////////////////////////////////////////////////////////

namespace CF {
namespace Mesh {
namespace Neu {
  
//////////////////////////////////////////////////////////////////////////////

Shared::Shared() :
    m_faces_cf_to_neu(10),
    m_faces_neu_to_cf(10),
    m_nodes_cf_to_neu(10),
    m_nodes_neu_to_cf(10)
{
  m_supported_types.reserve(9);
  m_supported_types.push_back("CF.Mesh.SF.Line1DLagrangeP1");
  m_supported_types.push_back("CF.Mesh.SF.Line2DLagrangeP1");
  m_supported_types.push_back("CF.Mesh.SF.Line3DLagrangeP1");
  m_supported_types.push_back("CF.Mesh.SF.Quad2DLagrangeP1");
  m_supported_types.push_back("CF.Mesh.SF.Quad3DLagrangeP1");
  m_supported_types.push_back("CF.Mesh.SF.Triag2DLagrangeP1");
  m_supported_types.push_back("CF.Mesh.SF.Triag3DLagrangeP1");
  m_supported_types.push_back("CF.Mesh.SF.Hexa3DLagrangeP1");
  m_supported_types.push_back("CF.Mesh.SF.Tetra3DLagrangeP1");

  m_CFelement_to_NeuElement[GeoShape::LINE ]=LINE;
  m_CFelement_to_NeuElement[GeoShape::QUAD ]=QUAD;
  m_CFelement_to_NeuElement[GeoShape::TRIAG]=TRIAG;
  m_CFelement_to_NeuElement[GeoShape::HEXA ]=HEXA;
  m_CFelement_to_NeuElement[GeoShape::TETRA]=TETRA;

  // ------------------------------------------------------- FACES
  // line
  m_faces_cf_to_neu[LINE].resize(2);
  m_faces_cf_to_neu[LINE][0]=1;
  m_faces_cf_to_neu[LINE][1]=2;

  m_faces_neu_to_cf[LINE].resize(3);
  m_faces_neu_to_cf[LINE][1]=0;
  m_faces_neu_to_cf[LINE][2]=1;

  // quad
  m_faces_cf_to_neu[QUAD].resize(4);
  m_faces_cf_to_neu[QUAD][0]=1;
  m_faces_cf_to_neu[QUAD][1]=2;
  m_faces_cf_to_neu[QUAD][2]=3;
  m_faces_cf_to_neu[QUAD][3]=4;

  m_faces_neu_to_cf[QUAD].resize(5);
  m_faces_neu_to_cf[QUAD][1]=0;
  m_faces_neu_to_cf[QUAD][2]=1;
  m_faces_neu_to_cf[QUAD][3]=2;
  m_faces_neu_to_cf[QUAD][4]=3;

  // triag
  m_faces_cf_to_neu[TRIAG].resize(3);
  m_faces_cf_to_neu[TRIAG][0]=1;
  m_faces_cf_to_neu[TRIAG][1]=2;
  m_faces_cf_to_neu[TRIAG][2]=3;

  m_faces_neu_to_cf[TRIAG].resize(4);
  m_faces_neu_to_cf[TRIAG][1]=0;
  m_faces_neu_to_cf[TRIAG][2]=1;
  m_faces_neu_to_cf[TRIAG][3]=2;

  // hexa
  m_faces_cf_to_neu[HEXA].resize(6);
  m_faces_cf_to_neu[HEXA][0]=1;
  m_faces_cf_to_neu[HEXA][1]=3;
  m_faces_cf_to_neu[HEXA][2]=6;
  m_faces_cf_to_neu[HEXA][3]=2;
  m_faces_cf_to_neu[HEXA][4]=5;
  m_faces_cf_to_neu[HEXA][5]=4;

  m_faces_neu_to_cf[HEXA].resize(7);
  m_faces_neu_to_cf[HEXA][1]=0;
  m_faces_neu_to_cf[HEXA][2]=3;
  m_faces_neu_to_cf[HEXA][3]=1;
  m_faces_neu_to_cf[HEXA][4]=5;
  m_faces_neu_to_cf[HEXA][5]=4;
  m_faces_neu_to_cf[HEXA][6]=2;

  // tetra
  m_faces_cf_to_neu[TETRA].resize(4);
  m_faces_cf_to_neu[TETRA][0]=1;
  m_faces_cf_to_neu[TETRA][1]=2;
  m_faces_cf_to_neu[TETRA][2]=3;
  m_faces_cf_to_neu[TETRA][3]=4;

  m_faces_neu_to_cf[TETRA].resize(5);
  m_faces_neu_to_cf[TETRA][1]=0;
  m_faces_neu_to_cf[TETRA][2]=1;
  m_faces_neu_to_cf[TETRA][3]=2;
  m_faces_neu_to_cf[TETRA][4]=3;


  // --------------------------------------------------- NODES

  // line
  m_nodes_cf_to_neu[LINE].resize(2);
  m_nodes_cf_to_neu[LINE][0]=0;
  m_nodes_cf_to_neu[LINE][1]=1;

  m_nodes_neu_to_cf[LINE].resize(2);
  m_nodes_neu_to_cf[LINE][0]=0;
  m_nodes_neu_to_cf[LINE][1]=1;

  // quad
  m_nodes_cf_to_neu[QUAD].resize(4);
  m_nodes_cf_to_neu[QUAD][0]=0;
  m_nodes_cf_to_neu[QUAD][1]=1;
  m_nodes_cf_to_neu[QUAD][2]=2;
  m_nodes_cf_to_neu[QUAD][3]=3;

  m_nodes_neu_to_cf[QUAD].resize(4);
  m_nodes_neu_to_cf[QUAD][0]=0;
  m_nodes_neu_to_cf[QUAD][1]=1;
  m_nodes_neu_to_cf[QUAD][2]=2;
  m_nodes_neu_to_cf[QUAD][3]=3;

  // triag
  m_nodes_cf_to_neu[TRIAG].resize(3);
  m_nodes_cf_to_neu[TRIAG][0]=0;
  m_nodes_cf_to_neu[TRIAG][1]=1;
  m_nodes_cf_to_neu[TRIAG][2]=2;

  m_nodes_neu_to_cf[TRIAG].resize(3);
  m_nodes_neu_to_cf[TRIAG][0]=0;
  m_nodes_neu_to_cf[TRIAG][1]=1;
  m_nodes_neu_to_cf[TRIAG][2]=2;


  // tetra
  m_nodes_cf_to_neu[TETRA].resize(4);
  m_nodes_cf_to_neu[TETRA][0]=0;
  m_nodes_cf_to_neu[TETRA][1]=1;
  m_nodes_cf_to_neu[TETRA][2]=2;
  m_nodes_cf_to_neu[TETRA][3]=3;

  m_nodes_neu_to_cf[TETRA].resize(4);
  m_nodes_neu_to_cf[TETRA][0]=0;
  m_nodes_neu_to_cf[TETRA][1]=1;
  m_nodes_neu_to_cf[TETRA][2]=2;
  m_nodes_neu_to_cf[TETRA][3]=3;


  // hexa
  m_nodes_cf_to_neu[HEXA].resize(8);
  m_nodes_cf_to_neu[HEXA][0]=4;
  m_nodes_cf_to_neu[HEXA][1]=5;
  m_nodes_cf_to_neu[HEXA][2]=1;
  m_nodes_cf_to_neu[HEXA][3]=0;
  m_nodes_cf_to_neu[HEXA][4]=6;
  m_nodes_cf_to_neu[HEXA][5]=7;
  m_nodes_cf_to_neu[HEXA][6]=3;
  m_nodes_cf_to_neu[HEXA][7]=2;

  m_nodes_neu_to_cf[HEXA].resize(8);
  m_nodes_neu_to_cf[HEXA][0]=3;
  m_nodes_neu_to_cf[HEXA][1]=2;
  m_nodes_neu_to_cf[HEXA][2]=7;
  m_nodes_neu_to_cf[HEXA][3]=6;
  m_nodes_neu_to_cf[HEXA][4]=0;
  m_nodes_neu_to_cf[HEXA][5]=1;
  m_nodes_neu_to_cf[HEXA][6]=4;
  m_nodes_neu_to_cf[HEXA][7]=5;

}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////

} // Neu
} // Mesh
} // CF
