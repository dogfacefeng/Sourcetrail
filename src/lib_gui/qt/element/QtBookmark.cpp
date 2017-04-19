#include "QtBookmark.h"

#include <QHBoxLayout>
#include <QMessageBox>
#include <QPixmap>
#include <QTimer>
#include <QVBoxLayout>

#include "utility/messaging/type/MessageActivateBookmark.h"
#include "utility/messaging/type/MessageDisplayBookmarkEditor.h"
#include "utility/ResourcePaths.h"

#include "data/bookmark/EdgeBookmark.h"
#include "qt/window/QtBookmarkCreator.h"
#include "qt/utility/utilityQt.h"

QtBookmark::QtBookmark()
	: m_treeWidgetItem(NULL)
	, m_arrowImageName("arrow_right.png")
	, m_hovered(false)
	, m_ignoreNextResize(false)
{
	setObjectName("bookmark");

	QVBoxLayout* layout = new QVBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setAlignment(Qt::AlignTop);
	setLayout(layout);

	QHBoxLayout* buttonsLayout = new QHBoxLayout();
	buttonsLayout->setSpacing(0);
	buttonsLayout->setContentsMargins(0, 0, 0, 0);
	buttonsLayout->setAlignment(Qt::AlignTop);

	m_toggleCommentButton = new QPushButton();
	m_toggleCommentButton->setObjectName("comment_button");
	m_toggleCommentButton->setToolTip("Show Comment");
	m_toggleCommentButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
	m_toggleCommentButton->setIconSize(QSize(10, 10));
	utility::setWidgetRetainsSpaceWhenHidden(m_toggleCommentButton);
	buttonsLayout->addWidget(m_toggleCommentButton);
	updateArrow();

	m_activateButton = new QPushButton();
	m_activateButton->setObjectName("activate_button");
	m_activateButton->setToolTip("Activate bookmark");
	m_activateButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
	buttonsLayout->addWidget(m_activateButton);

	buttonsLayout->addStretch();

	m_dateLabel = new QLabel();
	m_dateLabel->setObjectName("date_label");
	buttonsLayout->addWidget(m_dateLabel);

	m_editButton = new QPushButton();
	m_editButton->setObjectName("edit_button");
	m_editButton->setToolTip("Edit bookmark");
	m_editButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
	m_editButton->setIcon(QPixmap((ResourcePaths::getGuiPath() + "bookmark_view/images/bookmark_edit_icon.png").c_str()));
	utility::setWidgetRetainsSpaceWhenHidden(m_editButton);
	m_editButton->hide();
	buttonsLayout->addWidget(m_editButton);

	m_deleteButton = new QPushButton();
	m_deleteButton->setObjectName("delete_button");
	m_deleteButton->setToolTip("Delete bookmark");
	m_deleteButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
	m_deleteButton->setIcon(QPixmap((ResourcePaths::getGuiPath() + "bookmark_view/images/bookmark_delete_icon.png").c_str()));
	utility::setWidgetRetainsSpaceWhenHidden(m_deleteButton);
	m_deleteButton->hide();
	buttonsLayout->addWidget(m_deleteButton);

	layout->addLayout(buttonsLayout);

	m_comment = new QLabel("");
	m_comment->setObjectName("bookmark_comment");
	m_comment->setWordWrap(true);
	m_comment->hide();
	layout->addWidget(m_comment);

	connect(m_activateButton, SIGNAL(clicked()), this, SLOT(activateClicked()));
	connect(m_editButton, SIGNAL(clicked()), this, SLOT(editClicked()));
	connect(m_deleteButton, SIGNAL(clicked()), this, SLOT(deleteClicked()));
	connect(m_toggleCommentButton, SIGNAL(clicked()), this, SLOT(commentToggled()));
}

QtBookmark::~QtBookmark()
{
}

void QtBookmark::setBookmark(const std::shared_ptr<Bookmark> bookmark)
{
	if (bookmark)
	{
		m_bookmark = bookmark;

		m_activateButton->setText(m_bookmark->getName().c_str());

		if (m_bookmark->isValid() == false)
		{
			m_activateButton->setEnabled(false);
			m_editButton->setEnabled(false);
		}

		if (m_bookmark->getComment().length() > 0)
		{
			m_comment->setText(m_bookmark->getComment().c_str());
			m_toggleCommentButton->show();
		}
		else
		{
			m_toggleCommentButton->hide();
		}

		m_dateLabel->setText(getDateString().c_str());
	}
}

Id QtBookmark::getBookmarkId() const
{
	return m_bookmark->getId();
}

QTreeWidgetItem* QtBookmark::getTreeWidgetItem() const
{
	return m_treeWidgetItem;
}

void QtBookmark::setTreeWidgetItem(QTreeWidgetItem* treeWidgetItem)
{
	m_treeWidgetItem = treeWidgetItem;
}

void QtBookmark::commentToggled()
{
	if (!m_toggleCommentButton->isVisible())
	{
		return;
	}

	m_ignoreNextResize = true;

	if (m_comment->isVisible() == false)
	{
		m_arrowImageName = "arrow_down.png";
		m_comment->show();
		m_comment->setMinimumHeight(m_comment->heightForWidth(m_comment->width()));
	}
	else
	{
		m_arrowImageName = "arrow_right.png";
		m_comment->hide();
	}

	updateArrow();

	// forces the parent tree view to rescale
	if (m_treeWidgetItem)
	{
		m_treeWidgetItem->setExpanded(false);
		m_treeWidgetItem->setExpanded(true);
	}
}

void QtBookmark::resizeEvent(QResizeEvent* event)
{
	if (m_ignoreNextResize)
	{
		m_ignoreNextResize = false;
		return;
	}

	m_activateButton->setText(m_bookmark->getName().c_str());
	QTimer::singleShot(10, this, SLOT(elideButtonText()));
}

void QtBookmark::showEvent(QShowEvent* event)
{
	elideButtonText();
}

void QtBookmark::enterEvent(QEvent *event)
{
	m_editButton->show();
	m_deleteButton->show();

	m_hovered = true;
	updateArrow();
}

void QtBookmark::leaveEvent(QEvent *event)
{
	m_editButton->hide();
	m_deleteButton->hide();

	m_hovered = false;
	updateArrow();
}

void QtBookmark::activateClicked()
{
	MessageActivateBookmark(m_bookmark).dispatch();
}

void QtBookmark::editClicked()
{
	MessageDisplayBookmarkEditor(m_bookmark).dispatch();
}

void QtBookmark::deleteClicked()
{
	QMessageBox msgBox;
	msgBox.setText("Delete Bookmark");
	msgBox.setInformativeText("Do you really want to delete this bookmark?");
	msgBox.addButton("Delete", QMessageBox::ButtonRole::YesRole);
	msgBox.addButton("Keep", QMessageBox::ButtonRole::NoRole);
	msgBox.setIcon(QMessageBox::Icon::Question);
	int ret = msgBox.exec();

	if (ret == 0) // QMessageBox::Yes)
	{
		MessageDeleteBookmark(m_bookmark->getId()).dispatch();
	}
}

void QtBookmark::elideButtonText()
{
	m_activateButton->setText(m_activateButton->fontMetrics().elidedText(
		m_bookmark->getName().c_str(), Qt::ElideMiddle, m_activateButton->width() - 16));
}

void QtBookmark::handleMessage(MessageEditBookmark* message)
{
	if (m_bookmark->getId() == message->bookmarkId)
	{
		m_bookmark->setName(message->displayName);
		m_bookmark->setComment(message->comment);

		m_activateButton->setText(message->displayName.c_str());
	}
}

void QtBookmark::updateArrow()
{
	QPixmap pixmap((ResourcePaths::getGuiPath() + "bookmark_view/images/" + m_arrowImageName).c_str());
	m_toggleCommentButton->setIcon(QIcon(utility::colorizePixmap(pixmap, m_hovered ? "black" : "#707070")));
}

std::string QtBookmark::getDateString() const
{
	std::string result = "n/a";

	TimePoint creationDate = m_bookmark->getTimeStamp();

	float delta = TimePoint::now() - creationDate;

	if (delta < 3600.0f) // less than an hour ago
	{
		result = std::to_string(int(delta / 60.0f)) + " minutes ago";
	}
	else if (delta < (3600.0f * 6.0f)) // less than 6 hours ago
	{
		result = std::to_string(int(delta / 3600.0f)) + " hours ago";
	}
	else if (creationDate.isSameDay(TimePoint::now())) // today
	{
		result = "today";
	}
	else if (creationDate.deltaDays(TimePoint::now()) == 1) // yesterday
	{
		result = "yesterday";
	}
	else // whenever
	{
		result = creationDate.getDDMMYYYYString();
	}

	return result;
}