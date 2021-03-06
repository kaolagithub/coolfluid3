// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef CF_Common_StatusWorker_hpp
#define CF_Common_StatusWorker_hpp

////////////////////////////////////////////////////////////////////////////////

#include "Common/EnumT.hpp"
#include "Common/CommonAPI.hpp"

////////////////////////////////////////////////////////////////////////////////

namespace CF {
namespace Common {

////////////////////////////////////////////////////////////////////////////////

class Common_API WorkerStatus
{
  public:

  /// Enumeration of the worker statuses recognized in CF
  enum Type  { INVALID     =-1,
               STARTING    = 0,
               CONFIGURING = 1,
               EXITING     = 2,
               RUNNING     = 3,
               NOT_RUNNING = 4,
               WAITING     = 5,
               IDLE        = 6 };

  typedef Common::EnumT< WorkerStatus > ConverterBase;

  struct Common_API Convert : public ConverterBase
  {
    /// constructor where all the converting maps are built
    Convert();
    /// get the unique instance of the converter class
    static Convert& instance();
  };


}; // class StatusWorker

////////////////////////////////////////////////////////////////////////////////

Common_API std::ostream& operator<< ( std::ostream& os, const WorkerStatus::Type& in );
Common_API std::istream& operator>> ( std::istream& is, WorkerStatus::Type& in );

////////////////////////////////////////////////////////////////////////////////

} // Common
} // CF

////////////////////////////////////////////////////////////////////////////////

#endif // CF_Common_StatusWorker_hpp
