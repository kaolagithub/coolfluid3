list( APPEND coolfluid_rdm_files
  LibRDM.cpp
  LibRDM.hpp
  RDSolver.cpp
  RDSolver.hpp
  SupportedCells.hpp
  SupportedFaces.hpp
  Quadrature.hpp
  ElementLoop.hpp
  CellLoop.hpp
  FaceLoop.hpp
  Tags.hpp
  BoundaryConditions.hpp
  BoundaryConditions.cpp
  InitialConditions.hpp
  InitialConditions.cpp
  DomainDiscretization.hpp
  DomainDiscretization.cpp
  TimeStepping.hpp
  TimeStepping.cpp
  IterativeSolver.hpp
  IterativeSolver.cpp
  SetupSingleSolution.hpp
  SetupSingleSolution.cpp
  SetupMultipleSolutions.hpp
  SetupMultipleSolutions.cpp
# wizards
  UnsteadyExplicit.hpp
  UnsteadyExplicit.cpp
  SteadyExplicit.hpp
  SteadyExplicit.cpp
  MySim.cpp
  MySim.hpp
# actions
  Reset.hpp
  Reset.cpp
  BoundaryTerm.hpp
  BoundaryTerm.cpp
  ComputeDualArea.hpp
  ComputeDualArea.cpp
  CellTerm.hpp
  CellTerm.cpp
  FaceTerm.hpp
  FaceTerm.cpp
  BcDirichlet.hpp
  BcDirichlet.cpp
  BcBase.hpp
  SchemeBase.hpp
  WeakDirichlet.cpp
  WeakDirichlet.hpp
  Init.hpp
  Init.cpp
# time stepping
  FwdEuler.hpp
  FwdEuler.cpp
  RK.hpp
  RK.cpp
)

list( APPEND coolfluid_rdm_cflibs coolfluid_physics coolfluid_mesh coolfluid_mesh_sf coolfluid_mesh_actions coolfluid_solver_actions coolfluid_solver )

coolfluid_add_library( coolfluid_rdm )

add_subdirectory( Schemes )       # coolfluid_rdm_schemes library
add_subdirectory( NavierStokes )  # coolfluid_rdm_navierstokes library
add_subdirectory( Scalar )        # coolfluid_rdm_scalar library
add_subdirectory( LinEuler )      # coolfluid_rdm_lineuler library
add_subdirectory( GPU )           # coolfluid_rdm_gpu library
