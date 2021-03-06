// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef CF_GUI_Core_CNode_hpp
#define CF_GUI_Core_CNode_hpp

//////////////////////////////////////////////////////////////////////////////

#include <QStringList>

#include "Common/Component.hpp"

#include "UI/Core/LibCore.hpp"

class QMutex;
class QString;

template<typename T> class QList;
template<typename T, typename V> class QMap;

//////////////////////////////////////////////////////////////////////////////

namespace CF {
namespace UI {
namespace Core {

  class NLink;
  class CNode;

  /////////////////////////////////////////////////////////////////////////

  /// @brief Handles signal emitting for @c CNode class.

  /// @c #CNode class cannot derive from @c QObject (thus, cannot emit or catch
  /// Qt signals) because one of its subclasses (@c #NTree) also derives from
  /// another Qt class. It means that this subclass would derive twice from
  /// @c QObject, which is illegal in C++.
  /// @author Quentin Gasper
  class Core_API CNodeNotifier :
      public QObject
  {
    Q_OBJECT

  public:

    /// Constructor

    /// @param parent The parent @c CNode. May be null.
    CNodeNotifier(CNode * parent = nullptr);

    /// Emits childCountChanged() signal.
    void notifyChildCountChanged();

    /// @warning fix this link "contentChanged()" [wdeconinck]
    /// Emits @c #contentChanged() signal.
    void notifySignalSignature(Common::SignalArgs * node);

  signals:

    /// Signal emitted when children have been added or removed.
    void childCountChanged();

    /// Signal emitted when a signal signature has been received.
    void signalSignature(Common::SignalArgs * node);

  private:

    /// The parent @c CNode.
    CNode * m_parent;

  }; // class CNodeNotifier

  ////////////////////////////////////////////////////////////////////////////

  /// Handles information about actions (signals).
  /// @author Quentin Gasper
  struct Core_API ActionInfo
  {
    /// The action name.
    QString name;

    /// The action readable name. This name is intended to be displayed and
    /// should spaces instead of undescores.
    QString readableName;

    /// The action description.
    QString description;

    /// Indicates whether the action is local or not.

    /// If @c true, the action is considered as local and has to be executed on the
    /// local component. If @c false, the action has to be executed on the remote
    /// component (on COOLFluiD side)
    bool isLocal;

    /// Indicates wheter the action is enable or not.
    bool isEnabled;
  };

  ////////////////////////////////////////////////////////////////////////////

  /// Base component adapted to fit the client needs.

  /// Every component class created in the client should derive from this
  /// class.
  /// Each CNode has a special type, that helps a potential model to
  /// determine whether it should be showed or hidden, depending on
  /// the current modes (debug and/or advanced). @n
  /// The types are defined by the nested enum @c CNode::Type. Standard
  /// nodes are typically those comming from the server, they are deleted
  /// everytime the tree is updated. Local nodes are managed by the client
  /// itself to handle some local information, they only exist on the client-side.
  /// Debug nodes are local ones, except that they are only visible in
  /// debug mode. Local and debug modes exist are not deleted on tree update.

  /// @author Quentin Gasper
  class Core_API CNode :
      public Common::Component
  {
  public:

    ////////////////////////////////////////////

    typedef boost::shared_ptr<CNode> Ptr;
    typedef boost::shared_ptr<CNode const> ConstPtr;

    /// Defines the sub-node types
    enum Type
    {
      /// Standard node.
      STANDARD_NODE = 0,

      /// Local node.
      LOCAL_NODE = 1,

      /// Local and debug node.
      DEBUG_NODE = 2
    }; // enum Type

    ////////////////////////////////////////////

    /// Constructor.
    /// @param name Component name.
    /// @param componentType Corresponding component type name
    /// (on the simulator side)
    /// @param type Node type.
    CNode(const std::string & name, const QString & componentType, Type type);

    /// Gives the corresponding component type name
    /// @return Returns the corresponding component type name
    QString componentType() const;

    /// Gives a child a a certain index.
    /// @param index Index of the wanted child. Should be between 0 (included)
    /// and @c #count_children() (excluded).
    /// @return Returns the found child.
    CNode::Ptr child(Uint index);

    /// Gives the node tooltip.
    /// @return Returns the tooltip text.
    virtual QString toolTip() const = 0;

    /// Indicates whether this node is a client component or not.
    /// A node is considered as a client one if its type is either
    /// @c CNode::LOCAL_TYPE or @c CNode::DEBUB_NODE.
    /// @return Returns @c true if this node is a client component.
    bool isLocalComponent() const
    {
      return m_type != STANDARD_NODE;
    }

    /// Gives the node type.
    /// @return Returns the type of this node.
    Type type() const
    {
      return m_type;
    }

    /// Indicates whether this component is the root or not.
    /// @return Returns @c true if this node is a NRoot component.
    bool isRoot()
    {
      return m_isRoot;
    }

    /// Gives the real component.
    /// @return Returns this component cast to Component, execpt if this node
    /// is a NRoot, in which case the internal CRoot is returned.
    Component::Ptr realComponent();

    /// Gives the real component.
    /// @return Returns this component cast to Component, execpt if this node
    /// is a NRoot, in which case the internal CRoot is returned.
    Component::ConstPtr realComponent() const;

    /// Sets node properties
    /// @param node Node containing the options
    void setProperties(const Common::SignalArgs & node);

    /// Sets node signals
    /// Those are considered as non-local ones, meaning that asking the node
    /// to execute them will result to the sendng of a request to the remote
    /// component.
    /// @param node Node containing the signals
    void setSignals(const Common::SignalArgs & node);

    /// Modifies options

    /// If at least one option has been modified, a @c configure signal is sent
    /// to the corresponding component on the server.
    /// @param options Map of options to modify. The key is the option name.
    /// The value is the new option value, in string format.
    /// @throw BadValue If an option could not be found or could not be
    /// converted to an option.
    void modifyOptions(const QMap<QString, QString> & options);

    /// Gives options
    /// @param options Reference to a list where options will be put. The list
    /// cleared before first use.
    void listOptions(QList<Common::Option::ConstPtr> & list);

    /// Gives properties
    /// @param props Reference to a map where properties will be put. The map
    /// is cleared before first use.
    void listProperties(QMap<QString, QString> & props);

    /// Gives actions.
    /// @param acttions Reference to a list where actions will be put. The list
    /// is cleared before first use.
    void listSignals(QList<ActionInfo> & actions);

    /// Creates an object tree from a given node

    /// @param node Node to convert
    /// @return Retuns a shared pointer to the created node.
    /// @throw XmlError If the tree could not be built.
    static CNode::Ptr createFromXml(Common::XML::XmlNode node);

    /// Casts this node to a constant component of type TYPE.
    /// @return Returns the cast pointer
    /// @throw boost::bad_any_cast if the casting failed.
    template<class TYPE>
    boost::shared_ptr<const TYPE> castTo() const
    {
      return boost::dynamic_pointer_cast<TYPE>(shared_from_this());
    }

    /// Casts this node to a component of type TYPE.
    /// @return Returns the cast pointer
    /// @throw boost::bad_any_cast if the casting failed.
    template<class TYPE>
    boost::shared_ptr<TYPE> castTo()
    {
      return boost::dynamic_pointer_cast<TYPE>(shared_from_this());
    }

    /// Connects a slot a signal provided by the internal notifier.
    /// @param receiver The receiver object, The object on which the slot will be
    /// called.
    /// @param signal The signal.
    /// @param slot The slot to connect.
    /// @see CNodeNotifier
    void connectNotifier(QObject * reciever, const char * signal, const char * slot);

    /// Adds a sub-node.

    /// This method is a wrapper for @c Component::add_component(). It calls
    /// the parent method, but emits
    /// @c CNodeNotifier::notifyChildCountChanged() on success.@c
    /// It is recommended to add child nodes using this method in order to
    /// guarantee the view is correctly updated.
    /// @param node Node to add.
    /// @throw Common::ValueExists Forwards to the upper level any
    /// @c Common::ValueExists exception thrown by
    /// @c Component::add_component()
    void addNode(CNode::Ptr node);

    /// Removes a sub-node.

    /// This method is a wrapper for @c Component::remove_component(). It calls
    /// the parent method, but emits
    /// @c CNodeNotifier::notifyChildCountChanged() on success.@c
    /// It is recommended to remove child nodes using this method in order to
    /// guarantee the view is correctly updated.
    /// @param node Node to remove.
    /// @throw Common::ValueNotFound Forwards to the upper level any
    /// @c Common::ValueNotFound exception thrown by
    /// @c Component::remove_component()
    void removeNode(const QString & nodeName);

    /// Gives the internal notifier.
    /// @return Returns the internal notifier.
    CNodeNotifier * notifier() const;

    /// Lists all children paths in a string list.

    /// Strings have the same format as returned by @c CPath::uri().string().
    /// @param list The string list where paths will be stored. The list is not
    /// cleaned before first use.
    /// @param recursive If @c true, the listing is recursive. Otherwise,
    /// only direct children are listed.
    /// @param clientNode If @c true, client nodes are included into the the
    /// result. Otherwise, they are ignored.
    void listChildPaths(QStringList & list, bool recursive, bool clientNode = true) const;

    void requestSignalSignature(const QString & name);

    /// @name Signals
    //@{

    /// Method called when a @e tree_update event occurs on the server.
    /// This methods calls @c NTree::update_tree() method to resquet an update
    /// of the tree.
    /// @param node Signal data. This parameter is not used.
    void update_tree( Common::SignalArgs & node);

    /// Method called when receiving a reply to a previously sent
    /// "configure" signal.
    /// @param node An XML representation of the modified options.
    void configure_reply(Common::SignalArgs & node);

    /// Method called when the server replies to a @c list_content request.
    /// @param node Signal data.
    void list_content_reply( Common::SignalArgs & node );

    /// Method called when the server replies to a signal
    void signal_signature_reply( Common::SignalArgs & node );

    //@} END Signals

    /// Retrieves the signature of a local signal.

    /// @param name The signal name.
    /// @param node node @c SignalFrame where the signature will be stored.
    void localSignature(const QString & name, Common::SignalArgs& node );

    void finishSetUp();

  protected: // data

    /// This internal notifier.
    CNodeNotifier * m_notifier;

    /// Lists the names of the local signals.
    QStringList m_localSignals;

    QMutex * m_mutex;

    /// @c false until the node content has been retrieved from
    /// the server.
    bool m_contentListed;

    /// Idicates whether this node is already waiting for content
    /// from the server.
    /// This is used to avoid sending multiple requests to the server
    /// in case it is overloaded and takes some time to reply.
    bool m_listingContent;

    /// Indicates whether this component is a NRoot.
    /// If @c true, this component is a NRoot object.
    bool m_isRoot;

  private: // data

    /// Component type name.
    QString m_componentType;

    CNode::Type m_type;

  protected:

    /// List of signals that can be remotely executed
    QList<ActionInfo> m_actionSigs;

    /// Disables the local signals that need to.
    /// @param localSignals Map of local signals. The map is pre-initialiazed
    /// before calling this function with all local signals and the value set
    /// to @c true.
    virtual void disableLocalSignals( QMap<QString, bool> & localSignals) const = 0;

    virtual void setUpFinished() {}

  private: // helper functions

    /// Creates an object tree from a given node

    /// This is a recursive method. The second parameter holds, for each link
    /// the tree, the @c CPath of the target. The methods proceeds in this way
    /// since the target might not exist yet when the link exists. Furthermore,
    /// the method does not guarantee that each target path exists (e.i. the
    /// target is missing in the XML). It is up to the calling code to make
    /// that check.
    /// @param node Node to convert
    /// @param linkTargets Map where links
    /// @return Retuns a shared pointer to the created node.
    /// @throw XmlError If the tree could not be built.
    static CNode::Ptr createFromXmlRec(Common::XML::XmlNode & node,
               QMap<boost::shared_ptr<NLink>, Common::URI> & linkTargets);

    void fetchContent();

  }; // class CNode

  ////////////////////////////////////////////////////////////////////////////

} // Core
} // UI
} // CF

//////////////////////////////////////////////////////////////////////////////

#endif // CF_GUI_Core_CNode_hpp
