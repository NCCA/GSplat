#include "Splat.h"
#include "PlyLoader.h"
#include <iostream>
#include <ngl/VAOFactory.h>
#include <ngl/NGLStream.h>
#include <climits>
Splat::Splat(std::string_view _filename)
{
    createTextureBuffers();
     processSplatPlyData(_filename);
}

void Splat::createTextureBuffers()
{
  constexpr size_t s_num_buffers=5;
  GLuint targets[s_num_buffers];
  glGenTextures(s_num_buffers, &targets[0]);
  m_texturebuffers["position"]=targets[0];
  m_texturebuffers["normal"]=targets[1];
  m_texturebuffers["scale"]=targets[2];
  m_texturebuffers["colour"]=targets[3];
  m_texturebuffers["rotation"]=targets[4];
}

void Splat::processSplatPlyData(std::string_view _filename)
{
  std::cout<<"load ply file "<<_filename<<'\n';
  PlyLoader loader(_filename);
  std::cout<<"done now processing \n";
  auto vertex = loader.getElementByName("vertex");
  auto num_verts=vertex.count;
  m_numPoints=vertex.count;
  std::vector <ngl::Vec3> points(num_verts);
  for (size_t i=0; i< num_verts; ++i)
  {
    auto x=vertex.properties[0].value[i];
    auto y=vertex.properties[1].value[i];
    auto z=vertex.properties[2].value[i];
    points[i]=ngl::Vec3(std::get<float>(x),std::get<float>(y),std::get<float>(z));
    //points.emplace_back(ngl::Vec3(std::get<float>(x),std::get<float>(y),std::get<float>(z)));
  }
  // now upload to a texture buffer

  calculateBounds(points);
  GLuint target;
  glGenBuffers(1,&target);
  glBindBuffer(GL_TEXTURE_BUFFER,target);
  glBufferData(GL_TEXTURE_BUFFER,points.size()*sizeof(ngl::Vec3),&points[0].m_x,GL_STATIC_DRAW);

  glActiveTexture(GL_TEXTURE0);
  auto pos=m_texturebuffers.find("position")->second;
  glBindTexture(GL_TEXTURE_BUFFER,pos);
  glTexBuffer(GL_TEXTURE_BUFFER,GL_RGB32F,target);


  auto find_property_index = [&vertex](std::string_view _name)
  {
    for(size_t i=0; i<vertex.properties.size(); ++i)
    {
      if(vertex.properties[i].name==_name)
        return i;
    }
    //return 0;
  };
  // process the colours

  auto r_index = find_property_index("f_dc_0");
  auto g_index = find_property_index("f_dc_1");
  auto b_index = find_property_index("f_dc_2");
  auto a_index = find_property_index("opacity");
  std::vector<ngl::Vec4> colours;
  for (size_t i=0; i< num_verts; ++i)
  {
    auto r=vertex.properties[r_index].value[i];
    auto g=vertex.properties[g_index].value[i];
    auto b=vertex.properties[b_index].value[i];
    auto a=vertex.properties[a_index].value[i];
    float red=std::get<float>(r);
    float green=std::get<float>(g);
    float blue=std::get<float>(b);

//    float red=(0.5f + colour_coef * std::get<float>(r)) * 255;
//    float green=(0.5f + colour_coef * std::get<float>(g)) * 255;
//    float blue=(0.5f + colour_coef * std::get<float>(b)) * 255;
    float alpha= (1.0 / (1.0 + std::exp(-std::get<float>(a)))) * 255;
    //std::cout<<red<<' ' << green <<' '<<blue<<' '<<alpha<<'\n';
    colours.emplace_back(ngl::Vec4(red,green,blue,alpha));
  }
  glGenBuffers(1,&target);
  glBindBuffer(GL_TEXTURE_BUFFER,target);
  glBufferData(GL_TEXTURE_BUFFER,points.size()*sizeof(ngl::Vec4),&colours[0].m_x,GL_STATIC_DRAW);

  glActiveTexture(GL_TEXTURE1);
  auto colour=m_texturebuffers.find("colour")->second;
  glBindTexture(GL_TEXTURE_BUFFER,colour);
  glTexBuffer(GL_TEXTURE_BUFFER,GL_RGBA32F,target);


  // process the scales
  auto scale_index_x = find_property_index("scale_0");
  auto scale_index_y = find_property_index("scale_1");
  auto scale_index_z = find_property_index("scale_2");
  std::vector<ngl::Vec3> scales(num_verts);
  for (size_t i=0; i< num_verts; ++i)
  {
    auto sx = std::get<float>(vertex.properties[scale_index_x].value[i]);
    auto sy = std::get<float>(vertex.properties[scale_index_y].value[i]);
    auto sz = std::get<float>(vertex.properties[scale_index_z].value[i]);
    scales[i].set(sx,sy,sz);
  }
  glGenBuffers(1,&target);
  glBindBuffer(GL_TEXTURE_BUFFER,target);
  glBufferData(GL_TEXTURE_BUFFER,scales.size()*sizeof(ngl::Vec3),&scales[0].m_x,GL_STATIC_DRAW);
  glActiveTexture(GL_TEXTURE2);
  auto scale=m_texturebuffers.find("scale")->second;
  glBindTexture(GL_TEXTURE_BUFFER,scale);
  glTexBuffer(GL_TEXTURE_BUFFER,GL_RGB32F,target);


  auto rot_index_0 = find_property_index("rot_0");
  auto rot_index_1 = find_property_index("rot_1");
  auto rot_index_2 = find_property_index("rot_2");
  auto rot_index_3 = find_property_index("rot_3");

  std::vector<ngl::Vec4> rotations(num_verts);
  for (size_t i=0; i< num_verts; ++i)
  {
    auto rx = std::get<float>(vertex.properties[rot_index_0].value[i]);
    auto ry = std::get<float>(vertex.properties[rot_index_1].value[i]);
    auto rz = std::get<float>(vertex.properties[rot_index_2].value[i]);
    auto rw = std::get<float>(vertex.properties[rot_index_3].value[i]);

    rotations[i].set(rx,ry,rz,rw);
  }
  glGenBuffers(1,&target);
  glBindBuffer(GL_TEXTURE_BUFFER,target);
  glBufferData(GL_TEXTURE_BUFFER,rotations.size()*sizeof(ngl::Vec4),&rotations[0].m_x,GL_STATIC_DRAW);
  glActiveTexture(GL_TEXTURE3);
  auto rot=m_texturebuffers.find("rotation")->second;
  glBindTexture(GL_TEXTURE_BUFFER,rot);
  glTexBuffer(GL_TEXTURE_BUFFER,GL_RGB32F,target);





  /*
  for (size_t i=0; i< num_verts; ++i)
  {
    auto sx=std::get<float>(vertex.properties[scale_index_x].value[i]);
    auto sy=std::get<float>(vertex.properties[scale_index_y].value[i]);
    auto sz=std::get<float>(vertex.properties[scale_index_z].value[i]);
    auto rot_0=std::get<float>(vertex.properties[rot_index_0].value[i]);
    auto rot_1=std::get<float>(vertex.properties[rot_index_1].value[i]);
    auto rot_2=std::get<float>(vertex.properties[rot_index_2].value[i]);
    auto rot_3=std::get<float>(vertex.properties[rot_index_3].value[i]);
    ngl::Mat3 rot(
    1.0f- 2.0f * (rot_2 * rot_2 + rot_3 * rot_3),
            2.0f * (rot_1 * rot_2 + rot_0 * rot_3),
            2.0f * (rot_1 * rot_3 - rot_0 * rot_2),
            2.0f * (rot_1 * rot_2 - rot_0 * rot_3),
            1.0f - 2.0f * (rot_1 * rot_1 + rot_3 * rot_3),
            2.0f* (rot_2 * rot_3 + rot_0 * rot_1),
            2.0f * (rot_1 * rot_3 + rot_0 * rot_2),
            2.0f * (rot_2 * rot_3 - rot_0 * rot_1),
            1.0f - 2.0f * (rot_1 * rot_1 + rot_2 * rot_2));
    rot *=ngl::Mat3::scale(sx,sy,sz);
    m_rotations.emplace_back(rot);
    //m_scales.emplace_back(ngl::Vec3(std::get<float>(sx),std::get<float>(sy),std::get<float>(sz)));
  }
*/

  std::cout<<"loaded "<<m_numPoints<<" points\n";

}

void Splat::createVAO()
{
  /*
  m_vao=ngl::vaoFactoryCast<ngl::MultiBufferVAO>(ngl::VAOFactory::createVAO(ngl::multiBufferVAO,GL_POINTS));
  m_vao->bind();
  // buffer 0 is position data
  m_vao->setData(ngl::MultiBufferVAO::VertexData(m_points.size()*sizeof(ngl::Vec3),m_points[0].m_x));
  m_vao->setVertexAttributePointer(0,3,GL_FLOAT,0,0);
  // buffer 1 is colour data
  m_vao->setData(ngl::MultiBufferVAO::VertexData(m_colours.size()*sizeof(ngl::Vec4),m_colours[0].m_x));
  m_vao->setVertexAttributePointer(1,4,GL_FLOAT,0,0);
  m_vao->setNumIndices(m_points.size());
  // buffer 2,3,4 is rotations as a mat4
  m_vao->setData(ngl::MultiBufferVAO::VertexData(m_rotations.size()*sizeof(ngl::Mat3),m_rotations[0].m_00));
  m_vao->setVertexAttributePointer(2,3,GL_FLOAT,0,0);
  m_vao->setVertexAttributePointer(3,3,GL_FLOAT,0,0);
  m_vao->setVertexAttributePointer(4,3,GL_FLOAT,0,0);

  // number of points to draw
  m_vao->setNumIndices(m_points.size());

  m_vao->unbind();
*/
  m_bbox=std::make_unique<ngl::BBox>(m_minBound.m_x,m_maxBound.m_x,m_minBound.m_y,m_maxBound.m_y,m_minBound.m_z,m_maxBound.m_z);
  glGenVertexArrays(1, &m_pointVAO);

}


void Splat::renderPoints() const
{

  glActiveTexture(GL_TEXTURE0);
  auto pos=m_texturebuffers.find("position")->second;
  glBindTexture(GL_TEXTURE_BUFFER,pos);

  glActiveTexture(GL_TEXTURE1);
  auto colour=m_texturebuffers.find("colour")->second;
  glBindTexture(GL_TEXTURE_BUFFER,colour);
  glBindVertexArray(m_pointVAO);
  glDrawArrays(GL_POINTS,0,m_numPoints);
  glBindVertexArray(0);

}

void Splat::renderSplats() const
{
  glActiveTexture(GL_TEXTURE0);
  auto pos=m_texturebuffers.find("position")->second;
  glBindTexture(GL_TEXTURE_BUFFER,pos);

  glActiveTexture(GL_TEXTURE1);
  auto colour=m_texturebuffers.find("colour")->second;
  glBindTexture(GL_TEXTURE_BUFFER,colour);

  glActiveTexture(GL_TEXTURE2);
  auto scale=m_texturebuffers.find("scale")->second;
  glBindTexture(GL_TEXTURE_BUFFER,scale);

  glActiveTexture(GL_TEXTURE3);
  auto rotation=m_texturebuffers.find("rotation")->second;
  glBindTexture(GL_TEXTURE_BUFFER,rotation);


  glBindVertexArray(m_pointVAO);
  glDrawArrays(GL_POINTS,0,m_numPoints);
  glBindVertexArray(0);

}

void Splat::drawBB() const
{
  if(m_drawBounds)
  {
    m_bbox->draw();
  }
}

Splat::~Splat()
{

}

void Splat::calculateBounds(const std::vector<ngl::Vec3> &_points)
{
  auto maxfloat = std::numeric_limits<float>::max();
  auto minfloat = std::numeric_limits<float>::min();
  ngl::Vec3 min(maxfloat,maxfloat,maxfloat);
  ngl::Vec3 max(minfloat,minfloat,minfloat);
  for(auto p : _points)
  {
    if(p.m_x<min.m_x)
      min.m_x=p.m_x;
    if(p.m_y<min.m_y)
      min.m_y=p.m_y;
    if(p.m_z<min.m_z)
      min.m_z=p.m_z;
    if(p.m_x>max.m_x)
      max.m_x=p.m_x;
    if(p.m_y>max.m_y)
      max.m_y=p.m_y;
    if(p.m_z>max.m_z)
      max.m_z=p.m_z;
  }
  m_minBound=min;
  m_maxBound=max;
}