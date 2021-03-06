// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE "Test module for the mesh generators"

#include <boost/assign/list_of.hpp>
#include <boost/test/unit_test.hpp>

#include "Common/Core.hpp"
 
#include "Common/CRoot.hpp"
#include "Common/LibLoader.hpp"
#include "Common/OSystem.hpp"

#include "Tools/Growl/Notifier.hpp"

using namespace CF;
using namespace CF::Common;
using namespace CF::Tools::Growl;

BOOST_AUTO_TEST_SUITE( Growl )

/// Test creation of a 2D grid
BOOST_AUTO_TEST_CASE( TestGrowl )
{
  Notifier& notifier = Core::instance().root().create_component<Notifier>("notifier");
  notifier.notify("event","description");
}

BOOST_AUTO_TEST_SUITE_END()
