// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef CF_Tools_Growl_hpp
#define CF_Tools_Growl_hpp

#include "Tools/Growl/LibGrowl.hpp"

////////////////////////////////////////////////////////////////////////////////

namespace CF {
namespace Tools {
namespace Growl {

////////////////////////////////////////////////////////////////////////////////

/// @brief %Growl notifier component
///
/// This component can send notifications from Windows / Linux / Mac. Unixes
/// in general should be supported but are untested.
/// Notifications may be received by
/// - <a href="http://growl.info/">Growl</a> on Mac
/// - <a href="http://www.growlforwindows.com">GrowlForWindows</a> on Windows
/// - <a href="https://github.com/mattn/growl-for-linux">growl-for-linux</a> on Linux
///
/// To receive the notifications on %Growl on Mac, in the %Growl System Settings,
/// in the Network tab the following checkboxes need to be checked ON:
/// - Listen for incoming notifications
/// - Allow remote application registration
///
/// No information is known to me how to configure GrowForWindows or growl-for-linux.
/// (feel free to add)
///
/// This component wraps functions from the library <a href="https://github.com/mattn/gntp-send">gntp-send</a>
///
/// @author Willem Deconinck
class Tools_Growl_API Notifier: public Common::Component
{
public: // typedefs

  /// pointers
  typedef boost::shared_ptr<Notifier> Ptr;
  typedef boost::shared_ptr<Notifier const> ConstPtr;

public:

public: // functions

  /// Contructor
  /// @param name of the component
  Notifier ( const std::string& name );

  virtual ~Notifier();

  /// Get the class name
  static std::string type_name () { return "Notifier"; }

  /// @brief Notify %Growl
  /// @param event         Header of the notification
  /// @param description   Description of the notification
  void notify(const std::string& event, const std::string& description);

  /// @brief Signal to notify %Growl
  /// @see notify()
  void signal_notify ( Common::SignalArgs& node );

  /// @brief signature for the signal_notify() function
  void signature_notify ( Common::SignalArgs& node);

private: // data

  /// Configurable application name, telling %Growl which application sends
  /// the notification (default = COOLFluiD)
  std::string m_application_name;

  std::string m_notification_type;
  std::string m_server;
  std::string m_password;
  std::string m_icon;
  std::string m_url;

  Uint m_protocol;
  enum Protocol {UDP=0 , TCP=1};

};

////////////////////////////////////////////////////////////////////////////////

} // Growl
} // Tools
} // CF

////////////////////////////////////////////////////////////////////////////////

#endif // CF_Tools_Growl_hpp

