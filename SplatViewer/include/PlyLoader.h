#ifndef PLYLOADER_H
#define PLYLOADER_H
#include <vector>
#include <variant>
#include <unordered_map>
#include <string>
#include <string_view>
#include <fstream>
#include <memory>

enum class PropertyType  {CHAR, UCHAR, SHORT, USHORT, INT, UINT, FLOAT, DOUBLE};



struct property
{
    std::string name;
    PropertyType type;
    std::vector<std::variant <char, unsigned char, short, unsigned short, int, unsigned int, float, double>> value;

};

struct element
{
    std::string name;
    size_t count;
    std::vector<property> properties;
};

class PlyLoader
{
public :
    PlyLoader();
    PlyLoader(std::string_view _fname);
    size_t getNumElements() const ;
    bool load(std::string_view _fname);
    void debugPrint() const ;
    element &getElementByName(std::string_view _name);

private :
    bool checkMagicNumber();
    enum class FileType : bool {ASCII, BINARY} ;
    enum class HeaderType  {ELEMENT,PROPERTY,COMMENT,END_HEADER};
    FileType m_fileType;
    bool parseHeader();
    bool getFormat();
    bool parseAscii();
    bool parseBinary();
    HeaderType getElementType(std::string_view _line);
    PropertyType getType(std::string_view _line);
    std::unordered_map<std::string,element> m_elements;
    size_t m_numElements=0;
    std::ifstream m_infile;
    std::string m_currentElement="";
};


#endif