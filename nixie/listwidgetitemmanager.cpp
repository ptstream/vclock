#include "listwidgetitemmanager.hpp"
#include <QListWidget>

void CListWidgetItemManager::setBold (QListWidgetItem* item, bool bold)
{
  QFont font = item->font ();
  font.setBold (bold);
  item->setFont (font);
}

void CListWidgetItemManager::remBold (QListWidget* listWidget)
{
  for (int row = 0, count = listWidget->count (); row < count; ++row)
  {
    QListWidgetItem* item = listWidget->item (row);
    QFont            font = item->font ();
    if (font.bold ())
    {
      font.setBold (false);
      item->setFont (font);
    }
  }
}
