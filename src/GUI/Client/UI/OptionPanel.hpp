// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef CF_GUI_Client_OptionPanel_h
#define CF_GUI_Client_OptionPanel_h

////////////////////////////////////////////////////////////////////////////////

#include <QDomNamedNodeMap>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QObject>
#include <QWidget>

#include "GUI/Client/Core/OptionType.hpp"

#include "GUI/Client/UI/LibClientUI.hpp"

class QDomNodeList;
class QFormLayout;
class QGridLayout;
class QGroupBox;
class QHBoxLayout;
class QModelIndex;
class QPushButton;
class QScrollArea;
class QSplitter;
class QVBoxLayout;

////////////////////////////////////////////////////////////////////////////////

namespace CF {
namespace GUI {

namespace ClientCore {
  class CommitDetails;
  struct NodeOption;
}

namespace ClientUI {

////////////////////////////////////////////////////////////////////////////////

  class GraphicalOption;
  struct CloseConfirmationInfos;
  struct NodeOption;

  /// @brief Panel to m_view and modify m_options of an object.

  /// This class allows user to display and modify m_options of an object or
  /// add new m_options.

  /// @author Quentin Gasper.

  class ClientUI_API OptionPanel : public QWidget
  {
    Q_OBJECT

  public:
    /// @brief Constructor.

    /// Builds an @c OptionPanel with no m_options. The panel is neither in
    /// read-only mode nor advanced mode.
    /// @param parent The parent widget. Default value is @c CFNULL
    OptionPanel(QWidget * parent = CFNULL);

    /// @brief Destructor.

    /// Frees the allocated memory.  Parent is not destroyed.
    ~OptionPanel();

    /// @brief Indicates wether at least on option has been modified.

    /// @return Returns @c true if at least one option has been modified.
    bool isModified() const;

    /// @brief Build containers with modified m_options.

    /// This method allows to get old and new values of each modified option
    /// (this does not include new m_options). The old value is the original one,
    /// that the option had on calling @c setOptions. The new value is the
    /// current option value. All intermediate values (i.e. : if user modified
    /// several times the same option) are ignored. These values are stored in
    /// @c oldValues and @c newValues respectively. Each modified option name
    /// is stored if the provides string list. Hash map keys have one of these
    /// names. @n @n
    /// The method garantees that:
    /// @li string list and hash map will have exactly the same number of
    /// elements
    /// @li all hash map keys can be found in the string list
    /// @li each string list item has a corresponding key in both hash maps.
    /// New m_options values are not stored in any hash map.
    ///
    /// To ensure consistency of the data returned, these four containers are
    /// cleared before first use.
    /// @param m_options String list where modified option names will be stored.
    /// @param newValues This hash map is used to store old value of an option.
    /// The key is the option name as stored in @c m_options string list. The
    /// value is the old value.
    /// @param newValues This hash map is used to store new value of an option.
    /// The key is the option name as stored in @c m_options string list. The
    /// value is the new value.
    /// @param m_newOptions String list where new option names will be stored.
    void getModifiedOptions(ClientCore::CommitDetails & commitDetails) const;

    /// @brief Gives the current path.

    /// @return Returns the current path.
    QString getCurrentPath() const;

  public slots:

    /// @brief Slot called when user clicks on "Commit changes" button.

    /// If at least one option has been modified, @c changesMade signal is
    /// emitted.
    void btApplyClicked();

  private slots:

    void currentIndexChanged(const QModelIndex & newIndex, const QModelIndex & oldIndex);

    void advancedModeChanged(bool advanced);

    void dataChanged(const QModelIndex & first, const QModelIndex & last);

    void btSeeChangesClicked();

    void btForgetClicked();

    void valueChanged();

  signals:

    /// @brief Signal emitted when user clicks on "Commit changes" button if
    /// at least one option has been modified.

    /// @param modOptions XML document representing all modified m_options.
    /// Each document child is a modified option.
    /// @param m_newOptions XML document representing all new m_options. Each
    /// document child is a new option.
    void changesMade(const QDomDocument & modOptions,
                     const QDomDocument & m_newOptions) const;

  private:

    /// @brief Scroll area for basic m_options
    QScrollArea * m_scrollBasicOptions;

    /// @brief Scroll area for advanced m_options
    QScrollArea * m_scrollAdvancedOptions;

    /// @brief List containing basic m_options components.
    QList<GraphicalOption *> m_basicOptions;

    /// @brief List containing advanced m_options components.
    QList<GraphicalOption *> m_advancedOptions;

    /// @brief Button used to commit changes made.
    QPushButton * m_btApply;

    QPushButton * m_btForget;

    QPushButton * m_btSeeChanges;

    QGridLayout * m_buttonsLayout;

    /// @brief Layout used to display basic options components.
    QFormLayout * m_basicOptionsLayout;

    /// @brief Layout used to display advanced options components.
    QFormLayout * m_advancedOptionsLayout;

    /// @brief Main layout containing all widgets.

    /// This layout is composed of two lines and one column.
    QGridLayout * m_mainLayout;

    QGridLayout * m_topLayout;

    /// @brief Groupbox used to display basic m_options components
    /// with a titled border.

    ///  Its m_layout is @c #basicOptionsLayout.
    QGroupBox * m_gbBasicOptions;

    /// @brief Groupbox used to display advanced m_options components
    /// with a titled border.

    ///  Its m_layout is @c #advancedOptionsLayout.
    QGroupBox * m_gbAdvancedOptions;

     /// @brief Indicates if the panel is in advanced mode or not.

    /// If @c true, the panel is in advanced mode. Advanced m_options (if any)
    /// are displayed. Otherwise, they are m_hidden.
    bool m_advancedMode;

    QString m_currentPath;

    QSplitter * m_splitter;

    bool m_modelReset;

    /// @brief Puts all modified options in a provided hashmap.

    /// Only modified options will be set, meaning that the hashmap may be empty
    /// if no option has been modified. The map is cleared before first use.
    /// @param options A hashmap were modified options will be written. The
    /// key is the option name and the value is the option new value.
    void getOptions(QMap<QString, QString> & options) const;

    /// @brief Clears the given list by deleting the @c TOption
    /// objects its elements point to.

    /// After calling this method, the list is empty.
    /// @param list The list to clear.
    void clearList(QList<GraphicalOption *> & list);

    /// @brief Builds a part (basic or advanced m_options) of the XML document
    /// returned by @c #getOptions() and @c #getNewOptions().

    /// This document is built by comparing original m_options nodes to
    /// corresponding m_options components, which may have different values.
    /// If the values differ, the node is considered to have been modified
    /// and components values are taken as new values. Only modified nodes
    /// are appended to the document, which means that the document may be
    /// empty (if no option has been modified).
    /// @param nodes Original m_options nodes.
    /// @param graphOptions Options components.
    /// @param options A hashmap were modified options will be written. The
    /// key is the option name and the value is the option new value.
    void buildOptions(const QList<GraphicalOption *> & graphOptions,
                      QMap<QString, QString> & options) const;

    /// @brief Build containers with modified options.

    /// This method allows to get old and new values of each modified option.
    /// The old value is the original one, that the option had on calling
    /// @c setOptions. The new value is the current option value. All
    /// intermediate values (i.e. : if user modified several times the same
    /// option) are ignored.
    /// @param graphicalOptions Graphical components corresponding the option
    /// nodes.
    /// @param commitDetails Reference to where meodified options will be stored.
    /// @b Not cleared before first use.
    void getModifiedOptions(const QList<GraphicalOption *> & graphicalOptions,
                            ClientCore::CommitDetails & commitDetails) const;

    /// @brief Checks if options has been modified.

    /// @param graphicalOptions Options to check
    /// @return Returns @c true if at least one option has been modified;
    /// otherwise, returns @c false.
    bool isModified(const QList<GraphicalOption *> & graphicalOptions) const;

    void setOptions(const QList<ClientCore::NodeOption> & list);

    void setButtonsVisible(bool visible);

    void setButtonsEnabled(bool enabled);

  }; // class OptionPanel

  /////////////////////////////////////////////////////////////////////////////

} // namespace ClientUI
} // namespace GUI
} // namespace CF

////////////////////////////////////////////////////////////////////////////////

#endif // CF_GUI_Client_OptionPanel_h