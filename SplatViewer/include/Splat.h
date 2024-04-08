#ifndef SPLAT_H_
#define SPLAT_H_
#include <string_view>
#include <ngl/Vec3.h>
#include <ngl/Mat3.h>
#include <vector>
#include <ngl/MultiBufferVAO.h>
#include <ngl/BBox.h>

#include <unordered_map>

class Splat
{
public :
    Splat(std::string_view _filename);

    void render() const;
    void renderPoints() const;
    void renderSplats() const;
    void drawBB() const;
    void processSplatPlyData(std::string_view _filename);
    ~Splat();
    void createVAO();
    void createTextureBuffers();
    void generateIndexBuffer();
    void toggleBounds(){m_drawBounds^=true;}
    ngl::Vec3 getMinBound() const {return m_minBound;}
    ngl::Vec3 getMaxBound() const {return m_maxBound;}
    size_t getNumPoints() const {return m_numPoints;}
    size_t getNumSplatPoints() const {return m_numSplatPoints;}
private :
    void calculateBounds(const std::vector<ngl::Vec3> &_points);

    size_t m_numPoints=0;
    size_t m_numSplatPoints=0;
    GLuint m_indexBufferID=0;
    GLuint m_indexBufferStorage=0;
    std::unique_ptr<ngl::BBox> m_bbox;
    ngl::Vec3 m_minBound;
    ngl::Vec3 m_maxBound;
    bool m_drawBounds=true;
    std::unordered_map<std::string,GLuint> m_texturebuffers;
    GLuint m_pointVAO;

};

#endif