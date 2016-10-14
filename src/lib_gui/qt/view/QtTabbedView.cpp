#include "qt/view/QtTabbedView.h"

#include <QBoxLayout>
#include <QFrame>
#include <QTabBar>
#include <QTabWidget>

#include "qt/utility/utilityQt.h"
#include "qt/view/QtViewWidgetWrapper.h"
#include "settings/ColorScheme.h"
#include "utility/ResourcePaths.h"


QtTabbedView::QtTabbedView(ViewLayout* viewLayout, const std::string& name)
	: TabbedView(viewLayout, name)
	, m_refreshFunctor(std::bind(&QtTabbedView::doRefreshView, this))
{
}

QtTabbedView::~QtTabbedView()
{
}

void QtTabbedView::createWidgetWrapper()
{
	setWidgetWrapper(std::make_shared<QtViewWidgetWrapper>(new QFrame()));
}

void QtTabbedView::initView()
{
	QWidget* widget = QtViewWidgetWrapper::getWidgetOfView(this);

	QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	widget->setLayout(layout);

	m_widget = new QTabWidget(widget);
	layout->addWidget(m_widget);
}

void QtTabbedView::refreshView()
{
	m_refreshFunctor();
}

void QtTabbedView::doRefreshView()
{
	setStyleSheet();
}

void QtTabbedView::addViewWidget(View* view)
{
	m_widget->addTab(QtViewWidgetWrapper::getWidgetOfView(view), view->getName().c_str());

	doRefreshView();
}

void QtTabbedView::setStyleSheet()
{
	utility::setWidgetBackgroundColor(QtViewWidgetWrapper::getWidgetOfView(this), ColorScheme::getInstance()->getColor("tab/background"));

	m_widget->setStyleSheet(
		utility::getStyleSheet(ResourcePaths::getGuiPath() + "tabbed_view/tabbed_view.css").c_str()
	);
}