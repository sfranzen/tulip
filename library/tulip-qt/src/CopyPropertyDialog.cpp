/**
 *
 * This file is part of Tulip (www.tulip-software.org)
 *
 * Authors: David Auber and the Tulip development Team
 * from LaBRI, University of Bordeaux 1 and Inria Bordeaux - Sud Ouest
 *
 * Tulip is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * Tulip is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef  _WIN32
// compilation pb workaround
#include <windows.h>
#endif
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>
#include <QtGui/qlistview.h>
#include <QtGui/qpushbutton.h>
#include <QtGui/qmessagebox.h>
#include <QtGui/qinputdialog.h>
#include <QtGui/qlabel.h>
#include <QtGui/qtabwidget.h>

#include "tulip/CopyPropertyDialog.h"

using namespace std;
using namespace tlp;

//=============================================================================
CopyPropertyDialog::CopyPropertyDialog(QWidget* parent)
  : QDialog(parent) {
  setupUi(this);
  connect((QObject *) buttonOK , SIGNAL(clicked()), this, SLOT(accept()) );
  connect((QObject *) buttonCancel , SIGNAL(clicked()), this, SLOT(reject()) );
}

void CopyPropertyDialog::setProperties(std::string& srcProp,
				       std::vector<std::string>& localProps,
				       std::vector<std::string>& inheritedProps) {
  setWindowTitle(QString::fromUtf8((std::string("Copy property ") + srcProp).c_str()));
  unsigned int i = 0;
  if (localProps.size() == 0)
    localPropertyButton->setEnabled(false);
  else
    for (; i < localProps.size(); ++i)
      localProperties->addItem(QString::fromUtf8(localProps[i].c_str()));
  if (inheritedProps.size() == 0)
    inheritedPropertyButton->setEnabled(false);
  else
    for (i = 0; i < inheritedProps.size(); ++i)
      inheritedProperties->addItem(QString::fromUtf8(inheritedProps[i].c_str()));
}

bool CopyPropertyDialog::getDestinationProperty(CopyPropertyDialog::destType& type, std::string& prop) {
  type = CopyPropertyDialog::NEW;
  if (exec() == QDialog::Accepted) {
    if (newPropertyText->isEnabled())
      prop = std::string(newPropertyText->text().toUtf8().data());
    else {
      type = CopyPropertyDialog::LOCAL;
      if (localProperties->isEnabled())
	prop = std::string(localProperties->currentText().toUtf8().data());
      else {
	type = CopyPropertyDialog::INHERITED;
	prop = std::string(inheritedProperties->currentText().toUtf8().data());
      }
    }
    return true;
  }
  return false;
}

void CopyPropertyDialog::setNew(bool flag) {
  newPropertyText->setEnabled(flag);
}

void CopyPropertyDialog::setLocal(bool flag) {
  localProperties->setEnabled(flag);
}

void CopyPropertyDialog::setInherited(bool flag) {
  inheritedProperties->setEnabled(flag);
}
