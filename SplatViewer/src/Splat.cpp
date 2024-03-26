#include "Splat.h"
#include "PlyLoader.h"
#include <iostream>
#include <ngl/VAOFactory.h>
Splat::Splat(std::string_view _filename)
{
     processSplatPlyData(_filename);
}

void Splat::processSplatPlyData(std::string_view _filename)
{
  std::cout<<"load ply file "<<_filename<<'\n';
  PlyLoader loader(_filename);
  std::cout<<"done now processing \n";
  auto vertex = loader.getElementByName("vertex");
  auto num_verts=vertex.count;
  for (size_t i=0; i< num_verts; ++i)
  {
    auto x=vertex.properties[0].value[i];
    auto y=vertex.properties[1].value[i];
    auto z=vertex.properties[2].value[i];
    m_points.emplace_back(ngl::Vec3(std::get<float>(x),std::get<float>(y),std::get<float>(z)));
  }

}

void Splat::createVAO()
{
  m_vao=ngl::vaoFactoryCast<ngl::MultiBufferVAO>(ngl::VAOFactory::createVAO(ngl::multiBufferVAO,GL_POINTS));
  m_vao->bind();
  // buffer 0 is position data
  m_vao->setData(ngl::MultiBufferVAO::VertexData(m_points.size()*sizeof(ngl::Vec3),m_points[0].m_x));
  m_vao->setVertexAttributePointer(0,3,GL_FLOAT,0,0);
  m_vao->setNumIndices(m_points.size());
  m_vao->unbind();
}




void Splat::render() const
{
  m_vao->bind();
  m_vao->draw();
  m_vao->unbind();
}

Splat::~Splat()
{

}
