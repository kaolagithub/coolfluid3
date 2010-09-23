// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include "GUI/Client/Core/ClientRoot.hpp"
#include "GUI/Client/Core/NLog.hpp"

#include "GUI/Network/LogMessage.hpp"

#include "GUI/Client/UI/LoggingList.hpp"

using namespace CF::GUI::Network;
using namespace CF::GUI::ClientCore;
using namespace CF::GUI::ClientUI;

LoggingList::LoggingList(QWidget * parent, unsigned int maxLogLines)
  : QTextEdit(parent),
    m_maxLogLines(maxLogLines)
{
  this->setWordWrapMode(QTextOption::NoWrap);
  this->setReadOnly(true);

  connect(ClientRoot::log().get(), SIGNAL(newMessage(QString,CF::GUI::Network::LogMessage::Type)),
           this, SLOT(newMessage(QString,CF::GUI::Network::LogMessage::Type)));
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

LoggingList::~LoggingList()
{

}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void LoggingList::setMaxLogLines(unsigned int maxLogLines)
{
  m_maxLogLines = maxLogLines;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

unsigned int LoggingList::getMaxLogLines() const
{
  return m_maxLogLines;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

unsigned int LoggingList::getLogLinesCounter() const
{
  return m_logLinesCounter;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void LoggingList::clearLog()
{
  m_logLinesCounter = 0;
  this->clear();
}

 // PUBLIC SLOT

void LoggingList::newMessage(const QString & message, LogMessage::Type type)
{
  QString msgToAppend = "<font face=\"monospace\" color=\"%1\">%2</font>";
  QString imgTag = "<img src=\":/Icons/%1.png\" height=\"%2\" width=\"%2\"> ";
  QString color;
  QString msg = message;
  QString typeName = LogMessage::Convert::to_str(type).c_str();
  int size = 14;

  msg.replace(" ", "&nbsp;");
  msg.replace("<", "&lt;");
  msg.replace(">", "&gt;");

  if(type == LogMessage::ERROR || type == LogMessage::EXCEPTION)
    color = "red";
  else
    color = "black";

  msg.prepend(imgTag.arg(typeName).arg(size));

  this->append(msgToAppend.arg(color).arg(msg.replace("\n", "<br>")));
}