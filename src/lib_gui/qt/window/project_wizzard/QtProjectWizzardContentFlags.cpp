#include "qt/window/project_wizzard/QtProjectWizzardContentFlags.h"

#include <QFormLayout>

#include "qt/element/QtDirectoryListBox.h"

QtProjectWizzardContentFlags::QtProjectWizzardContentFlags(ProjectSettings* settings, QtProjectWizzardWindow* window)
	: QtProjectWizzardContent(settings, window)
{
}

void QtProjectWizzardContentFlags::populateForm(QGridLayout* layout, int& row)
{
	QLabel* label = createFormLabel("Compiler Flags");
	label->setObjectName("label");
	layout->addWidget(label, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);

	addHelpButton("Define compiler flags used during analysis including the dash e.g. -v", layout, row);

	m_list = new QtDirectoryListBox(this, true);
	layout->addWidget(m_list, row, QtProjectWizzardWindow::BACK_COL);
	row++;
}

void QtProjectWizzardContentFlags::load()
{
	m_list->setStringList(m_settings->getCompilerFlags());
}

void QtProjectWizzardContentFlags::save()
{
	m_settings->setCompilerFlags(m_list->getStringList());
}