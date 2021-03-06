// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef CF_GUI_Server_ServerRoot_hpp
#define CF_GUI_Server_ServerRoot_hpp

//////////////////////////////////////////////////////////////////////////////

#include <QObject>

#include "Common/CRoot.hpp"
#include "Common/CJournal.hpp"

#include "UI/Server/ServerRoot.hpp"

#include "UI/Server/CCore.hpp"

class QMutex;

//////////////////////////////////////////////////////////////////////////////

namespace CF {
namespace UI {
namespace Server {

  class ProcessingThread;
  class Notifier;

  ///////////////////////////////////////////////////////////////////////////

  class SignalCatcher : public QObject
  {
    Q_OBJECT

  public slots:

    void finished();
  };

  class ServerRoot :
      public boost::noncopyable,
      public CF::NonInstantiable<ServerRoot>
  {
  public:

    static CF::Common::CRoot::Ptr root();

    static void processSignal(const std::string & target,
                              const CF::Common::URI & receiver,
                              const std::string & clientid,
                              const std::string & frameid,
                              Common::SignalArgs & node);

    static CCore::Ptr core();

    static CF::Common::CJournal::Ptr journal();

    static void listenToEvents();

  private:

    static Common::XML::XmlDoc::Ptr m_doc;

    static ProcessingThread * m_thread;

    static SignalCatcher * m_catcher;

    static QMutex m_mutex;

    static Common::NotificationQueue * m_queue;

    static Notifier * m_notifier;

    friend void SignalCatcher::finished();

  }; // class ServerRoot


} // Server
} // UI
} // CF

/////////////////////////////////////////////////////////////////////////////

#endif // CF_GUI_Server_ServerRoot_hpp
