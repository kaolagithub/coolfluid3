// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef CF_Common_NotificationQueue_hpp
#define CF_Common_NotificationQueue_hpp

/////////////////////////////////////////////////////////////////////////////////

#include <boost/signals2/signal.hpp>

#include "Common/CF.hpp"
#include "Common/CommonAPI.hpp"
#include "Common/CRoot.hpp"

/////////////////////////////////////////////////////////////////////////////////

namespace CF {
namespace Common {

  ///////////////////////////////////////////////////////////////////////////////

  class URI;

  /// @brief Manages a notification queue.

  /// This class listens to a root component and records every emitted event.
  /// The @c #flush method allows to emit all events to attached notifiers.
  /// Once flushed, all event are removed from the internal buffer. @n
  /// Notifiers are external classes that register one or more of their methods
  /// to the queue. The notifier classes have to meet some constraints:
  /// @li all registered methods return nothing and take a string and a CPath as
  /// parameters. The string is the name of event and the CPath is the path
  /// to the component that raised the event.
  /// @li define a public method begin_notify() that will be called before
  /// each flush. The method takes no parameter and returns nothing. This is
  /// useful if the notifier has to clean or set up things between two flushes.
  /// The class guarantees that no event will be emitted before this method
  /// is called.

  /// @author Quentin Gasper
  class Common_API NotificationQueue
  {
  public: // functions

    /// @brief Constructor
    /// @param root The root component this class has to listen to. Cannot be
    /// null.
    NotificationQueue(CRoot::Ptr root);

    /// @brief Desctructor
    ~NotificationQueue();

    /// @brief Adds a notification for the event if which name is specified.

    /// If the event does not exist (i.e. it has never been emitted),  it is
    /// created.
    /// @param name Event name
    /// @param sender_path Path of the component that emitted the event
    void add_notification ( const std::string & name, const URI & sender_path );

    /// @brief Counts the notifications for a speficied event.

    /// @param name The event name. If empty, the number of all notifications
    /// if counted.
    /// @return Returns the notifications count for the event, or the number
    /// of all notification if @c name is empty.
    CF::Uint nb_notifications ( const std::string & name = std::string() ) const;

    /// @brief Flushes the notification buffer.

    /// Attached notifiers receive events they are registered for.
    void flush();

    /// @brief Adds a notifier
    /// @param name The name of the event
    /// @param fcnt Pointer to a method of @c NOTIFIER class. This method
    /// returns nothing and take a string and a CPath as parameters.
    /// @param receiver The object that will receive the event.
    template<class NOTIFIER>
    void add_notifier( const std::string & name,
                       void (NOTIFIER::*fcnt)(const std::string & name, const URI & sender_path),
                       NOTIFIER * receiver);

  private: // typedefs

    /// @brief Typedef for event signals
    typedef boost::signals2::signal< void (const std::string&, const URI&) > SignalType_t;

    /// @brief Boost shared pointer for a signal
    typedef boost::shared_ptr<SignalType_t> SignalPtr_t;

    /// @brief Typedef for flushing signal
    typedef boost::signals2::signal< void () > SignalTypeFlush_t;

    /// @brief Typedef for the map that stores signals assigned to events.

    /// The key is the event name. The value is the corresponding
    /// signal, to which are connected notifiers.
    typedef std::map<std::string, SignalPtr_t> EventSigsStorage_t;

  private: // data

    /// @brief Notification buffer.

    /// For each pair, the @c first if the event name, and the @c second is
    /// the path to the component that emitted the event.
    std::vector< std::pair<std::string, URI> > m_notifications;

    /// @brief Signal used to raise events when #flush() method is called.
    boost::shared_ptr< SignalTypeFlush_t > m_sig_begin_flush;

    /// @brief Event signals

    /// The map stores all event names that are listened to by at least one
    /// notifier.
    EventSigsStorage_t m_event_signals;

  }; // class NotificationQueue

  ///////////////////////////////////////////////////////////////////////////////

  template<class NOTIFIER>
  void NotificationQueue::add_notifier( const std::string & name,
                                        void (NOTIFIER::*fcnt)(const std::string & name, const URI & sender_path),
                                        NOTIFIER * receiver)
  {
    cf_assert( receiver != nullptr );

    SignalPtr_t sig;

    if( m_event_signals.find(name) == m_event_signals.end() )
    {
      sig = SignalPtr_t(new SignalType_t());
      m_event_signals[name] = sig;
    }
    else
      sig = m_event_signals[name];

    m_sig_begin_flush->connect(boost::bind(&NOTIFIER::begin_notify, receiver));
    sig->connect( boost::bind(fcnt, receiver, _1, _2) ); // _2 because 2 arguments
  }

  ///////////////////////////////////////////////////////////////////////////////

} // Common
} // CF

#endif // CF_Common_NotificationQueue_hpp
