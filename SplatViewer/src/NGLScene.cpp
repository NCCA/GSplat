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
  setTitle("Blank NGL");

}


NGLScene::~NGLScene()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}



void NGLScene::resizeGL(int _w , int _h)
{
  m_win.width  = static_cast<int>( _w * devicePixelRatio() );
  m_win.height = static_cast<int>( _h * devicePixelRatio() );
  m_project = ngl::perspective(45.0f, static_cast<float>(_w) / _h, 0.1f, 200.0f);

}


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
  ngl::ShaderLib::loadShader("SplatShader","shaders/SplatVertex.glsl","shaders/SplatFragment.glsl");
  // colour shader for debug
  ngl::ShaderLib::use(ngl::nglColourShader);
  ngl::ShaderLib::setUniform("Colour",1.0f,0.0f,0.0f,1.0f);
  // We now create our view matrix for a static camera
  m_splat = std::make_unique<Splat>(m_filename);
  m_splat->createVAO();
  auto from =m_splat->getMaxBound()+ngl::Vec3(0.0f,0.0f,2.0f);
  //ngl::Vec3 from{0.0f, 2.0f, 2.0f};
  ngl::Vec3 to{0.0f, 0.0f, 0.0f};
  ngl::Vec3 up{0.0f, 1.0f, 0.0f};
  // now load to our new camera
  m_view = ngl::lookAt(from, to, up);
  m_project=ngl::perspective(45.0f,1024.0f/720.0f,0.1f,200.0f);

  ngl::ShaderLib::loadShader("PointSplatShader","shaders/PointSplatVertex.glsl","shaders/PointSplatFragment.glsl");
  ngl::ShaderLib::use("PointSplatShader");
}



void NGLScene::paintGL()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_win.width,m_win.height);
  // Rotation based on the mouse position for our global transform
  auto rotX = ngl::Mat4::rotateX(m_win.spinXFace);
  auto rotY = ngl::Mat4::rotateY(m_win.spinYFace);

  // multiply the rotations
  m_mouseGlobalTX = rotX * rotY;
  // add the translations
  m_mouseGlobalTX.m_m[3][0] = m_modelPos.m_x;
  m_mouseGlobalTX.m_m[3][1] = m_modelPos.m_y;
  m_mouseGlobalTX.m_m[3][2] = m_modelPos.m_z;
  ngl::ShaderLib::use("PointSplatShader");
  ngl::ShaderLib::setUniform("MVP",m_project*m_view*m_mouseGlobalTX);
  m_splat->render();
  ngl::ShaderLib::use("nglColourShader");
  m_splat->drawBB();
}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
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

  break;
  default : break;
  }
  // finally update the GLWindow and re-draw

    update();
}
