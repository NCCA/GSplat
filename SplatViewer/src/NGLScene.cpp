#include <QMouseEvent>
#include <QGuiApplication>

#include "NGLScene.h"
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>
#include <iostream>

NGLScene::NGLScene(std::string_view _filename) : m_filename{_filename}
{
  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  setTitle("Gaussian Splatting");
  m_timer.start();

}


NGLScene::~NGLScene()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}



void NGLScene::resizeGL(int _w , int _h)
{
  m_win.width  = static_cast<int>( _w * devicePixelRatio() );
  m_win.height = static_cast<int>( _h * devicePixelRatio() );
  m_cam.setProjection(45.0f, static_cast<float>(m_win.width) / m_win.width, 0.05f, 2500.0f);

}

constexpr char* PointSplatShader="PointSplatShader";
constexpr char* SplatShader="SplatShader";

void NGLScene::initializeGL()
{
  // we must call that first before any other GL commands to load and link the
  // gl commands from the lib, if that is not done program will crash
  ngl::NGLInit::initialize();
  glClearColor(0.7f, 0.7f, 0.7f, 1.0f);			   // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);
  ngl::ShaderLib::loadShader(SplatShader,"shaders/SplatVertex.glsl","shaders/SplatFragment.glsl","shaders/SplatGeometry.glsl");
  // colour shader for debug
  ngl::ShaderLib::use(ngl::nglColourShader);
  ngl::ShaderLib::setUniform("Colour",1.0f,0.0f,0.0f,1.0f);
  // We now create our view matrix for a static camera
  m_splat = std::make_unique<Splat>(m_filename);
  m_splat->createVAO();
  auto from =m_splat->getMaxBound()+ngl::Vec3(0.0f,0.0f,0.5f);
  //ngl::Vec3 from{0.0f, 2.0f, 2.0f};
  ngl::Vec3 to{0.0f, 0.0f, 0.0f};
  ngl::Vec3 up{0.0f, 1.0f, 0.0f};
  m_cam.set(from,to,up);
  m_cam.setProjection(45.0f, static_cast<float>(m_win.width) / m_win.height, 0.05f, 2500.0f);
  // now load to our new camerax
  ngl::ShaderLib::loadShader(PointSplatShader,"shaders/PointSplatVertex.glsl","shaders/PointSplatFragment.glsl");
  ngl::ShaderLib::use(PointSplatShader);
}



void NGLScene::paintGL()
{
  std::chrono::steady_clock::time_point startPaint = std::chrono::steady_clock::now();

  float currentFrame = m_timer.elapsed() * 0.001f;
  m_deltaTime = currentFrame - m_lastFrame;
  m_lastFrame = currentFrame;
  auto rot=ngl::Mat4::rotateZ(180.0f);
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_win.width,m_win.height);
  /// first we reset the movement values
  float xDirection = 0.0;
  float yDirection = 0.0;
  // now we loop for each of the pressed keys in the the set
  // and see which ones have been pressed. If they have been pressed
  // we set the movement value to be an incremental value
  foreach (Qt::Key key, m_keysPressed)
    {
      switch (key)
      {
        case Qt::Key_Left:
        {
          yDirection = -1.0f;
          break;
        }
        case Qt::Key_Right:
        {
          yDirection = 1.0f;
          break;
        }
        case Qt::Key_Up:
        {
          xDirection = 1.0f;
          break;
        }
        case Qt::Key_Down:
        {
          xDirection = -1.0f;
          break;
        }
        default:
          break;
      }
    }
  // if the set is non zero size we can update the ship movement
  // then tell openGL to re-draw
  if (m_keysPressed.size() != 0)
  {
    m_cam.move(xDirection, yDirection, m_deltaTime);
  }

  if (m_drawMode == DrawMode::Points)
  {
    glEnable(GL_DEPTH_TEST); // Disable depth testing
    glDisable(GL_BLEND);

    m_cam.setSpeed(2.5f);
    ngl::ShaderLib::use(PointSplatShader);
    ngl::ShaderLib::setUniform("posSampler", 0);
    ngl::ShaderLib::setUniform("colourSampler", 1);
    ngl::ShaderLib::setUniform("projection", m_cam.getProjection());
    ngl::ShaderLib::setUniform("view", m_cam.getView() * rot); /// * rot?
    //glPointSize(10.0f);
    m_splat->renderPoints();
  }
  else if (m_drawMode == DrawMode::Splats)
  {
    glDisable(GL_DEPTH_TEST); // Disable depth testing
//
//    glEnable(GL_BLEND);
//    glBlendFuncSeparate( GL_ONE_MINUS_DST_ALPHA, GL_ONE, GL_ONE_MINUS_DST_ALPHA, GL_ONE);
//    glBlendEquationSeparate(GL_FUNC_ADD,GL_FUNC_ADD);
    m_cam.setSpeed(20.0f);
    ngl::ShaderLib::use(SplatShader);
    ngl::ShaderLib::setUniform("posSampler", 0);
    ngl::ShaderLib::setUniform("colourSampler", 1);
    ngl::ShaderLib::setUniform("scaleSampler", 2);
    ngl::ShaderLib::setUniform("rotationSampler", 3);
    ngl::ShaderLib::setUniform("eye", m_cam.getEye());

    ngl::ShaderLib::setUniform("projection", m_cam.getProjection());
    ngl::ShaderLib::setUniform("view", m_cam.getView() * rot); /// * rot?
    m_splat->renderSplats();
  }
  ngl::ShaderLib::use(ngl::nglColourShader);
  ngl::ShaderLib::setUniform("MVP",m_cam.getProjection()*m_cam.getView()*rot);
  m_splat->drawBB();
}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  m_keysPressed += static_cast<Qt::Key>(_event->key());

  // this method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow
  switch (_event->key())
  {
  // escape key to quite
  case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
  case Qt::Key_Space :
      m_win.spinXFace=0;
      m_win.spinYFace=0;
      m_modelPos.set(ngl::Vec3::zero());
      m_cam.set({0, 2, 10}, {0, 0, 0}, {0, 1, 0});
      break;
    case Qt::Key_P :
      m_drawMode=DrawMode::Points;
      break;
    case Qt::Key_S :
      m_drawMode=DrawMode::Splats;
      break;
  default : break;
  }
  // finally update the GLWindow and re-draw

    update();
}


void NGLScene::keyReleaseEvent(QKeyEvent *_event)
{
  // remove from our key set any keys that have been released
  m_keysPressed -= static_cast<Qt::Key>(_event->key());
}