#ifndef SPLAT_H_
#define SPLAT_H_
#include <string_view>
#include <ngl/Vec3.h>
#include <ngl/Mat3.h>
#include <vector>
#include <ngl/MultiBufferVAO.h>
#include <ngl/BBox.h>
class Splat
{
public :
    Splat(std::string_view _filename);

    void render() const;
    void drawBB() const;
    void processSplatPlyData(std::string_view _filename);
    ~Splat();
    void createVAO();
    void calculateBounds();
    void toggleBounds(){m_drawBounds^=true;}
    ngl::Vec3 getMinBound() const {return m_minBound;}
    ngl::Vec3 getMaxBound() const {return m_maxBound;}
private :
    std::vector<ngl::Vec3> m_points;
    std::vector<ngl::Vec4> m_colours;
    //std::vector<ngl::Vec3> m_scales;
    std::vector<ngl::Mat3> m_rotations;
    std::unique_ptr<ngl::MultiBufferVAO> m_vao;
    std::unique_ptr<ngl::BBox> m_bbox;
    ngl::Vec3 m_minBound;
    ngl::Vec3 m_maxBound;
    bool m_drawBounds=true;

};

#endif