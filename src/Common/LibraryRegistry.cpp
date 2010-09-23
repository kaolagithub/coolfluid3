// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include "Common/LibraryRegistry.hpp"
#include "Common/LibraryRegisterBase.hpp"
#include "Common/Log.hpp"
#include "Common/LogLevel.hpp"

////////////////////////////////////////////////////////////////////////////////

namespace CF {
namespace Common {

////////////////////////////////////////////////////////////////////////////////

void LibraryRegistry::regist(Common::LibraryRegisterBase* module)
{
  if ( !m_store.checkEntry( module->getName()) )
  {
    m_store.addEntry(module->getName(),module);
    //    CFLogInfo ( "Module " + module->getName() + " registered\n");
//    CFinfo << "Module " << module->getName() << " registered\n" << CFendl ;
  }
  else
  {
    CFLogWarn("Module " + module->getName() + " already registered : skipping registration\n");
  }
}

////////////////////////////////////////////////////////////////////////////////

void LibraryRegistry::unregist(const std::string& moduleName)
{
  if ( m_store.checkEntry( moduleName ) )
  {
    m_store.removeEntry(moduleName);
  }
  else
  {
    CFLogWarn("Module " + moduleName + " not registered : skipping removal\n");
  }
}

////////////////////////////////////////////////////////////////////////////////

bool LibraryRegistry::isRegistered(const std::string& moduleName)
{
  return m_store.checkEntry(moduleName);
}

////////////////////////////////////////////////////////////////////////////////

Common::SafePtr<Common::LibraryRegisterBase>
LibraryRegistry::getLibraryRegisterBase(const std::string& moduleName)
{
  if ( m_store.checkEntry( moduleName ) )
  {
    return m_store.getEntry(moduleName);
  }
  else
  {
    CFLogWarn("Module " + moduleName + " not registered : returning null pointer\n");
    return Common::SafePtr<Common::LibraryRegisterBase>(CFNULL);
  }
}

////////////////////////////////////////////////////////////////////////////////

std::vector< Common::SafePtr<Common::LibraryRegisterBase> >
LibraryRegistry::getAllModules()
{
  std::vector< SafePtr<LibraryRegisterBase> > all;
  all.reserve(m_store.size());
  std::transform(m_store.begin(),
                 m_store.end(),
                 back_inserter(all),
                 GeneralStorage<LibraryRegisterBase>::extract);
  return all;
}

////////////////////////////////////////////////////////////////////////////////

  } // namespace Common

} // namespace CF

////////////////////////////////////////////////////////////////////////////////