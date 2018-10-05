#ifndef LISTWIDGETITEMMANAGER_HPP
#define LISTWIDGETITEMMANAGER_HPP

class QListWidget;
class QListWidgetItem;

/*! \brief Simple list view manager.
 * Actually only set an item bold and reset the bold item.
 */
class CListWidgetItemManager
{
public:
  /*! Set an item bold. */
  static void setBold (QListWidgetItem* item, bool bold);

  /*! Sets unbold all items. */
  static void remBold (QListWidget* listWidget);
};

#endif // LISTWIDGETITEMMANAGER_HPP
