#ifdef  _WIN32
// compilation pb workaround
#include <windows.h>
#endif
#include <QtGui/qevent.h>
#include <QtGui/qimage.h>
#include <QtGui/qtooltip.h>

#include "tulip/GlMainWidget.h"

#include <tulip/Graph.h>
#include <tulip/Iterator.h>
#include <tulip/BooleanProperty.h>
#include <tulip/LayoutProperty.h>
#include <tulip/SizeProperty.h>
#include <tulip/GlTools.h>
#include <tulip/StlIterator.h>
#include <tulip/GlDisplayListManager.h>
#include <tulip/GlTextureManager.h>
#include <tulip/GlRectTextured.h>
#include "tulip/QtCPULODCalculator.h"
#include "tulip/InteractorManager.h"

using namespace std;

namespace tlp {

  // indicates if we can deal with GL_AUX* buffers
  bool _glAuxBufferAvailable = false;
  // This has to be done just before the first time we draw
  // to ensure a right QGLContext already exist
  static void checkIfGlAuxBufferAvailable() {
    static bool done = false;
    if (!done) {
      GLint res;
      glGetIntegerv(GL_AUX_BUFFERS, &res);
      tlp::glTest(__PRETTY_FUNCTION__);
      _glAuxBufferAvailable = (res > 0);
      done = true;
#ifndef NDEBUG
      if (_glAuxBufferAvailable)
	cerr << "Auxillary Buffer Available" << endl;
      else
	cerr << "Auxillary Buffer Not Available" << endl;
#endif
    }
  }
  //==================================================
  static void setRasterPosition(unsigned int x, unsigned int y) {
    //  cerr << __PRETTY_FUNCTION__ << endl;
    float val[4];
    unsigned char tmp[10];
    glGetFloatv(GL_CURRENT_RASTER_POSITION, (float*)&val);
    glBitmap(0,0,0,0,-val[0] + x, -val[1] + y, tmp);
    glGetFloatv(GL_CURRENT_RASTER_POSITION, (float*)&val);
    tlp::glTest(__PRETTY_FUNCTION__);
  }
  //==================================================
  static QGLFormat GlInit() {
    QGLFormat tmpFormat;
    tmpFormat.setDirectRendering(true);
    tmpFormat.setDoubleBuffer(true);
    tmpFormat.setAccum(false);
    tmpFormat.setStencil(true);
    tmpFormat.setOverlay(false);
    tmpFormat.setDepth(true);
    tmpFormat.setRgba(true);
    tmpFormat.setAlpha(true);
    tmpFormat.setOverlay(false);
    tmpFormat.setStereo(false);
    return tmpFormat;
  }

  //==================================================
  GlMainWidget::GlMainWidget(QWidget *parent, const char *name):
    QGLWidget(GlInit(), parent),
    _id(Interactor::invalidID) {
    //  scene(new QtCPULODCalculator()){
    setObjectName(name);
    //  cerr << __PRETTY_FUNCTION__ << endl;
    setFocusPolicy(Qt::StrongFocus);
    //_composite = new GlADComposite();
    // the following lines are for use in designer
    setMinimumWidth(50);
    setMinimumHeight(50);
  }
  //==================================================
  GlMainWidget::~GlMainWidget() {
    for (unsigned int i = 0; i > _interactors.size(); ++i)
      delete _interactors[i];
  }
  //==================================================
  void GlMainWidget::setData(Graph *graph,DataSet dataSet) {
    if(!dataSet.exist("scene")) {
      //Default scene
      GlLayer* layer=new GlLayer("Main");
      GlLayer *backgroundLayer=new GlLayer("Background");
      backgroundLayer->setVisible(false);
      GlLayer *foregroundLayer=new GlLayer("Foreground");
      foregroundLayer->setVisible(false);

      backgroundLayer->set2DMode();
      foregroundLayer->set2DMode();
      string dir=TulipLibDir;
      dir += "tlp/bitmaps/";
      GlRectTextured *background=new GlRectTextured(0,1.,0,1.,dir + "tex_back.png",true);
      backgroundLayer->addGlEntity(background,"background");
      
      GlRectTextured *labri=new GlRectTextured(5.,55.,5.,55.,dir + "logolabri.jpg");
      foregroundLayer->addGlEntity(labri,"labrilogo");
      
      GlComposite *hulls=new GlComposite;
      hulls->setVisible(false);
      layer->addGlEntity(hulls,"Hulls");
      
      scene.addLayer(backgroundLayer);
      scene.addLayer(layer);
      scene.addLayer(foregroundLayer);
      GlGraphComposite* graphComposite=new GlGraphComposite(graph);
      scene.addGlGraphCompositeInfo(scene.getLayer("Main"),graphComposite);
      scene.getLayer("Main")->addGlEntity(graphComposite,"graph");
    }else{
      string sceneInput;
      dataSet.get("scene",sceneInput);
      scene.setWithXML(sceneInput,graph);
      DataSet renderingParameters;
      dataSet.get("Display",renderingParameters);
      GlGraphRenderingParameters rp=scene.getGlGraphComposite()->getRenderingParameters();
      rp.setParameters(renderingParameters);
    }
  }
  //==================================================
  DataSet GlMainWidget::getData() {
    DataSet outDataSet;
    outDataSet.set<DataSet>("Display",scene.getGlGraphComposite()->getRenderingParameters().getParameters());
    string out;
    scene.getXML(out);
    outDataSet.set<string>("scene",out);
    return outDataSet;
  }
  //==================================================
  Graph *GlMainWidget::getGraph() {
    return scene.getGlGraphComposite()->getInputData()->getGraph();
  }
  //==================================================
  void GlMainWidget::paintEvent( QPaintEvent* e) {
    QRegion rect = this->visibleRegion();
    //If the visible are changed we need to draw the entire scene
    //Because the saved snapshot only backup the visibile part of the
    //Graph
    if (rect.boundingRect() != _visibleArea.boundingRect()) {
      _visibleArea = rect;
      draw();
    }
    else {
      redraw();
    }
    _visibleArea = rect; //Save the new visible area.
  }
  //==================================================
  void GlMainWidget::closeEvent(QCloseEvent *e) {
    emit closing(this, e);
  }
  //==================================================
  void GlMainWidget::setupOpenGlContext() {
    //  cerr << __PRETTY_FUNCTION__ << " (" << (int)this << ")" << endl;
    assert(context()->isValid());
    makeCurrent();
  }

  //==================================================
  void GlMainWidget::paintGL() { 
    //  cerr << __PRETTY_FUNCTION__ << endl;
  }
  //==================================================
  void GlMainWidget::glInit() {
    //  cerr << __PRETTY_FUNCTION__ << endl;
  }
  //==================================================
  void GlMainWidget::glDraw() {
    //  cerr << __PRETTY_FUNCTION__ << endl;
  }
  //==================================================
  void GlMainWidget::updateGL() {
    //  cerr << __PRETTY_FUNCTION__ << endl;
  }
  //==================================================
  void GlMainWidget::initializeGL() {
    //  cerr << __PRETTY_FUNCTION__ << endl;
  }
  //==================================================
  void GlMainWidget::redraw() {
    if (isDrawing()) return; //the graph drawn should be tested 
    //when multithreading will be enable.
    checkIfGlAuxBufferAvailable();
    //Gl_AUX is not available we must redraw everything
    //TODO : make copy into memory in that case
    if (!_glAuxBufferAvailable) {
      draw();
      return;
    }
   
    makeCurrent();
    //Restore the graph image
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glDrawBuffer(GL_BACK);
    glReadBuffer(GL_AUX0);
    setRasterPosition(0,0);
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glCopyPixels(0,0,width(), height(), GL_COLOR);
    //draw the glAugmented on top of the graph
    //_composite->draw(this);
    //draw the interactors on top of graph and augmented display
    for(vector<Interactor *>::iterator it =
	  _interactors.begin(); it != _interactors.end(); ++it) {
      if ((*it)->draw(this))
	break;
    }
    tlp::glTest(__PRETTY_FUNCTION__);
    swapBuffers();
  }
  //==================================================
  bool GlMainWidget::isDrawing() {
#ifndef NDEBUG
    cerr << __PRETTY_FUNCTION__ << " : not implemented" << endl;
#endif
    return false;
  }
  //==================================================
  void GlMainWidget::draw() {
    if (isVisible()) {
      checkIfGlAuxBufferAvailable();
      makeCurrent();

      computeInteractors();
      if(scene.getGlGraphComposite()) {
	hulls.compute(scene.getLayer("Main"),scene.getGlGraphComposite()->getInputData()->getGraph());
      }
      scene.draw();
      drawInteractors();

      //save the drawing of the graph in order to prevent redrawing during
      //Interactor draw
      if (_glAuxBufferAvailable) {
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);
	glReadBuffer(GL_BACK);
	glDrawBuffer(GL_AUX0);
	glClear(GL_COLOR_BUFFER_BIT);
	setRasterPosition(0,0);
	glCopyPixels(0,0,width(), height(), GL_COLOR);
	glFlush();
	glDrawBuffer(GL_BACK);
      }
      glTest(__PRETTY_FUNCTION__);  
  
      swapBuffers();
      emit graphRedrawn(this);
    }
  }
  //==================================================
  void GlMainWidget::stopDrawing() {
    cerr << __PRETTY_FUNCTION__ << " :: Not implemented" << endl;
    //  _drawTimer->stop();
  }
  //==================================================
  void GlMainWidget::waitDrawing() {
#ifndef NDEBUG
    cerr << __PRETTY_FUNCTION__ << " :: Not implemented" << endl;
#endif
  }
  //==================================================
  void GlMainWidget::computeInteractors() {
    for(vector<Interactor *>::iterator it =
	  _interactors.begin(); it != _interactors.end(); ++it) {
      if ((*it)->compute(this))
	break;
    }
  }
  //==================================================
  void GlMainWidget::drawInteractors() {
    for(vector<Interactor *>::iterator it =
	  _interactors.begin(); it != _interactors.end(); ++it) {
      if ((*it)->draw(this))
	break;
    }
  }
  //==================================================
  Iterator<Interactor *> *GlMainWidget::getInteractors() const {
    return new StlIterator<Interactor *, vector<Interactor *>::const_iterator>(_interactors.begin(), _interactors.end());
  }
  //==================================================
  Interactor::ID GlMainWidget::pushInteractor(Interactor* interactor) {
    if (interactor) {
      interactor = interactor->clone();
      interactor->setID(++_id);
      _interactors.push_back(interactor);
      installEventFilter(interactor);
      interactor->compute(this);
      updateGL();
    }
    return _id;
  }
  //==================================================
  void GlMainWidget::popInteractor() {
    if (_interactors.size()) {
      Interactor *interactor = _interactors[_interactors.size() - 1];
      _interactors.pop_back();
      removeEventFilter(interactor);
      delete interactor;
    }
  }
  //==================================================
  void GlMainWidget::removeInteractor(Interactor::ID i) {
    for(vector<Interactor *>::iterator it =
	  _interactors.begin(); it != _interactors.end(); ++it) {
      if ((*it)->getID() == i) {
	removeEventFilter(*it);
	delete *it;
	_interactors.erase(it);
	break;
      }
    }
  }
  //==================================================
  Interactor::ID GlMainWidget::resetInteractors(Interactor *interactor) {
    for(vector<Interactor *>::iterator it =
	  _interactors.begin(); it != _interactors.end(); ++it) {
      removeEventFilter(*it);
      delete *it;
    }
    _interactors.clear();
    return pushInteractor(interactor);
  }
  //==================================================
  std::vector<tlp::Interactor::ID> GlMainWidget::resetInteractors(const std::vector<Interactor *> &new_interactors) {
    for(vector<Interactor *>::iterator it =
	  _interactors.begin(); it != _interactors.end(); ++it) {
      removeEventFilter(*it);
      delete *it;
    }
    _interactors.clear();
    vector<Interactor::ID> ids;
    for (vector<Interactor *>::const_iterator it =
	   new_interactors.begin(); it != new_interactors.end(); ++it)
      ids.push_back(pushInteractor(*it));
    return ids;
  }
  //==================================================
  //QGLWidget
  //==================================================
  QImage GlMainWidget::grabFrameBuffer(bool withAlpha) {
    waitDrawing();
    QImage img = QGLWidget::grabFrameBuffer(withAlpha);
    return img;
  }
  //==================================================
  //QGLWidget slots
  //==================================================
  void GlMainWidget::resizeGL(int w, int h) {
#ifndef NDEBUG
    cerr << __PRETTY_FUNCTION__ << endl;
#endif
    if (w == 0 || h == 0) {
      cerr << "warning: GlMainWidget::resizeGL(" << w << ", " << h << ")" <<  endl;
      return ;
    }
    scene.setViewport(0,0,w,h);
  }
  //==================================================
  void GlMainWidget::makeCurrent() {
    QGLWidget::makeCurrent();
    GlDisplayListManager::getInst().changeContext((unsigned long)this);
    GlTextureManager::getInst().changeContext((unsigned long)this);
  }
  //==================================================
  bool GlMainWidget::selectGlEntities(const int x, const int y, 
				      const int width, const int height,
				      std::vector<GlEntity *> &pickedEntities,
				      GlLayer* layer) {
    makeCurrent();
    return scene.selectEntities(SelectSimpleEntities,x, y, 
				width, height, 
				layer,
				pickedEntities);
  }
  //==================================================
  bool GlMainWidget::selectGlEntities(const int x, const int y,
				      std::vector <GlEntity *> &pickedEntities,
				      GlLayer* layer) {
    makeCurrent();
    return scene.selectEntities(SelectSimpleEntities,x, y, 
				2, 2,
				layer,
				pickedEntities);
  }
  //==================================================
  void GlMainWidget::doSelect(const int x, const int y, 
			      const int width ,const int height,
			      vector<node> &sNode, vector<edge> &sEdge,
			      GlLayer* layer) {
#ifndef NDEBUG
    cerr << __PRETTY_FUNCTION__ << " x:" << x << ", y:" <<y <<", wi:"<<width<<", height:" << height << endl;
#endif
    makeCurrent();
    vector<GlEntity*> selectedElements;
    scene.selectEntities(SelectNodes, x, y, width, height, layer, selectedElements);
    for(vector<GlEntity*>::iterator it=selectedElements.begin();it!=selectedElements.end();++it) {
      sNode.push_back(node( ((GlNode*)(*it))->id ));
    }
    selectedElements.clear();
    scene.selectEntities(SelectEdges, x, y, width, height, layer, selectedElements);
    for(vector<GlEntity*>::iterator it=selectedElements.begin();it!=selectedElements.end();++it) {
      sEdge.push_back(edge( ((GlEdge*)(*it))->id ));
    }
  }
  //=====================================================
  bool GlMainWidget::doSelect(const int x, const int y,tlp::ElementType &type ,node &n,edge &e, GlLayer* layer) {
#ifndef NDEBUG
    cerr << __PRETTY_FUNCTION__ << endl;
#endif
    makeCurrent();
    vector<GlEntity*> selectedElements;
    scene.selectEntities(SelectNodes, x-1, y-1, 3, 3, layer, selectedElements);
    if(selectedElements.size()!=0) {
      type=NODE;
      n=node(((GlNode*)selectedElements[0])->id);
      return true;
    }
    scene.selectEntities(SelectEdges, x-1, y-1, 3, 3, layer, selectedElements);
    if(selectedElements.size()!=0) {
      type=EDGE;
      e=edge(((GlEdge*)selectedElements[0])->id);
      return true;
    }
    return false;
  }
  //=====================================================
  unsigned char * GlMainWidget::getImage() {
    makeCurrent();
    return scene.getImage();
    //return GlGraph::getImage();
  }
  //=====================================================
  bool GlMainWidget::outputEPS(int size, int doSort, const char *filename) {
    makeCurrent();
    scene.outputEPS(size, filename);
    return true;
  }
  //=====================================================
  bool GlMainWidget::outputSVG(int size, const char* filename) {
    makeCurrent();
    scene.outputSVG(size, filename);
    return true;
  }

}

