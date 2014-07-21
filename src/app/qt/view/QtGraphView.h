#ifndef QT_GRAPH_VIEW_H
#define QT_GRAPH_VIEW_H

#include "component/view/GraphView.h"
#include "qt/utility/QtThreadedFunctor.h"
#include "utility/math/Vector4.h"
#include "utility/types.h"

struct DummyEdge;
struct DummyNode;
class QGraphicsView;
class QtGraphEdge;
class QtGraphNode;

class QtGraphView: public GraphView
{
public:
	QtGraphView(ViewLayout* viewLayout);
	virtual ~QtGraphView();

	// View implementation
	virtual void createWidgetWrapper();
	virtual void initView();
	virtual void refreshView();

	virtual void rebuildGraph(const std::vector<DummyNode>& nodes, const std::vector<DummyEdge>& edges);

	virtual void clear();

private:
	QGraphicsView* getView();

	void doRebuildGraph(const std::vector<DummyNode>& nodes, const std::vector<DummyEdge>& edges);
	void doClear();

	std::shared_ptr<GraphNode> findOrCreateNode(QGraphicsView* view, const DummyNode& node);
	std::shared_ptr<GraphNode> findNode(const Id id);
	std::shared_ptr<GraphNode> findSubNode(const std::shared_ptr<GraphNode> node, const Id id);
	std::shared_ptr<GraphNode> createNode(QGraphicsView* view, const DummyNode& node);
	std::shared_ptr<QtGraphNode> createSubNode(QGraphicsView* view, const DummyNode& node);

	std::shared_ptr<QtGraphEdge> createEdge(QGraphicsView* view, const DummyEdge& edge);

	QtThreadedFunctor<const std::vector<DummyNode>&, const std::vector<DummyEdge>&> m_rebuildGraph;
	QtThreadedFunctor<void> m_clear;

	std::vector<Vec4i> m_edgeColors;
};

#endif // QT_GRAPH_VIEW_H
