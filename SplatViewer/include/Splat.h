#ifndef SPLAT_H_
#define SPLAT_H_
#include <string_view>
#include <ngl/Vec3.h>
#include <vector>
#include <ngl/MultiBufferVAO.h>
class Splat
{
public :
    Splat(std::string_view _filename);

    void render() const;
    void processSplatPlyData(std::string_view _filename);
    ~Splat();
    void createVAO();
private :
    std::vector<ngl::Vec3> m_points;
    std::unique_ptr<ngl::MultiBufferVAO> m_vao;

};

#endif