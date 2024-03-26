#include "PlyLoader.h"
#include <iostream>
#include <fmt/format.h>
#include <range/v3/all.hpp>

PlyLoader::PlyLoader()
{

}

PlyLoader::PlyLoader(std::string_view _fname)
{
  load(_fname);
}

size_t PlyLoader::getNumElements() const
{
  return m_elements.size();
}

bool PlyLoader::checkMagicNumber()
{
  // grab first line of file and check it contains ply
  std::string line;
  getline(m_infile,line,'\n');
  return line.find("ply") != std::string::npos;
}


bool PlyLoader::getFormat()
{
  bool valid=false;
  using namespace ranges;
  std::string line;
  getline(m_infile,line,'\n');
  // split line into
  auto split_line = line | views::split(' ') | to<std::vector<std::string>>();
  // line should be  format [type] 1.0 so check which type and set
  if (split_line.size() > 2 && split_line[0] == "format")
  {
    static const std::unordered_map<std::string_view, FileType> formatMap = {
            {"ascii", FileType::ASCII},
            {"binary_little_endian", FileType::BINARY},
            {"binary_big_endian", FileType::BINARY}
    };

    auto it = formatMap.find(split_line[1]);
    if (it != formatMap.end())
    {
      m_fileType = it->second;
      valid = true;
    }
  }
  return valid;
}

PropertyType PlyLoader::getType(std::string_view _line)
{
  static const std::unordered_map<std::string_view, PropertyType> typeMap = {
          {"char", PropertyType::CHAR},
          {"uchar", PropertyType::UCHAR},
          {"short", PropertyType::SHORT},
          {"ushort", PropertyType::USHORT},
          {"int", PropertyType::INT},
          {"uint", PropertyType::UINT},
          {"float", PropertyType::FLOAT},
          {"double", PropertyType::DOUBLE}
  };
  auto it = typeMap.find(_line);
  if (it != typeMap.end())
  {
    return it->second;
  }
  return PropertyType::CHAR;
}

bool PlyLoader::parseHeader()
{
  bool loaded= false;
  loaded=getFormat();
  if(loaded)
  {
    std::string line;
    while (getline(m_infile, line, '\n'))
    {
      auto split_line= line | ranges::views::split(' ') | ranges::to<std::vector<std::string>>();
      if(split_line.size() == 0)
      {
        continue;
      }
      auto type = getElementType(split_line[0]);
      switch (type)
      {
        case HeaderType::ELEMENT:
        {

          element e;
          e.count = std::stoi(split_line[2]);
          e.properties.reserve(e.count);
          m_elements[split_line[1]]= e;
          m_currentElement = split_line[1];
          m_numElements++;
        }
        break;

        case HeaderType::PROPERTY:
        {
          property p;
          p.name = split_line[2];
          p.type = getType(split_line[1]);
          m_elements[m_currentElement].properties.push_back(p);
        }
        break;

        case HeaderType::END_HEADER:
            return true;
          break;
        case HeaderType::COMMENT:
          break;
      }
    }
  }
  return loaded;
}

PlyLoader::HeaderType PlyLoader::getElementType(std::string_view _line)
{
  static const std::unordered_map<std::string_view, HeaderType> elementMap = {
          {"element", HeaderType::ELEMENT},
          {"property", HeaderType::PROPERTY},
          {"comment", HeaderType::COMMENT},
          {"end_header", HeaderType::END_HEADER}
  };
  auto it = elementMap.find(_line);
  if (it != elementMap.end())
  {
    return it->second;
  }
  return HeaderType::COMMENT;
}

element& PlyLoader::getElementByName(std::string_view _name)
{
  auto it = m_elements.find(_name.data());
  if (it != m_elements.end())
  {
    return {it->second};
  }
}


bool PlyLoader::parseAscii()
{
  for (auto &element : m_elements)
  {
    for (size_t j = 0; j < element.second.count; ++j)
    {
      std::string line;
      getline(m_infile, line, '\n');
      auto split_line = line | ranges::views::split(' ') | ranges::to<std::vector<std::string>>();
      for (size_t k = 0; k < element.second.properties.size(); ++k)
      {
        switch (element.second.properties[k].type)
        {
          case PropertyType::CHAR:
            element.second.properties[k].value.push_back(std::stoi(split_line[k]));
            break;
          case PropertyType::UCHAR:
            element.second.properties[k].value.push_back( std::stoi(split_line[k]));
            break;
          case PropertyType::SHORT:
            element.second.properties[k].value.push_back(std::stoi(split_line[k]));
            break;
          case PropertyType::USHORT:
            element.second.properties[k].value.push_back(std::stoi(split_line[k]));
            break;
          case PropertyType::INT:
            element.second.properties[k].value.push_back(std::stoi(split_line[k]));
            break;
          case PropertyType::UINT:
            element.second.properties[k].value.push_back(std::stoi(split_line[k]));
            break;
          case PropertyType::FLOAT:
            element.second.properties[k].value.push_back(std::stof(split_line[k]));
            break;
          case PropertyType::DOUBLE:
            element.second.properties[k].value.push_back(std::stod(split_line[k]));
            break;
        }
      }
    }

  }
return true;
}


bool PlyLoader::parseBinary()
{
  for (auto &element : m_elements)
  {
    for (size_t j = 0; j < element.second.count; ++j)
    {
      for (size_t k = 0; k < element.second.properties.size(); ++k)
      {
        switch (element.second.properties[k].type)
        {
          case PropertyType::CHAR:
          {
            char c;
            m_infile.read(reinterpret_cast<char *>(&c), sizeof(char));
            element.second.properties[k].value.push_back(c);
          }
          break;
          case PropertyType::UCHAR:
          {
            unsigned char c;
            m_infile.read(reinterpret_cast<char *>(&c), sizeof(unsigned char));
            element.second.properties[k].value.push_back(c);
          }
          break;
          case PropertyType::SHORT:
          {
            short s;
            m_infile.read(reinterpret_cast<char *>(&s), sizeof(short));
            element.second.properties[k].value.push_back(s);
          }
          break;
          case PropertyType::USHORT:
          {
            unsigned short s;
            m_infile.read(reinterpret_cast<char *>(&s), sizeof(unsigned short));
            element.second.properties[k].value.push_back(s);
          }
          break;
          case PropertyType::INT:
          {
            int i;
            m_infile.read(reinterpret_cast<char *>(&i), sizeof(int));
            element.second.properties[k].value.push_back(i);
          }
          break;
          case PropertyType::UINT:
          {
            unsigned int i;
            m_infile.read(reinterpret_cast<char *>(&i), sizeof(unsigned int));
            element.second.properties[k].value.push_back(i);
          }
          break;
          case PropertyType::FLOAT:
          {
            float f;
            m_infile.read(reinterpret_cast<char *>(&f), sizeof(float));
            element.second.properties[k].value.push_back(f);

          }
          break;
          case PropertyType::DOUBLE:
          {
            double d;
            m_infile.read(reinterpret_cast<char *>(&d), sizeof(double));
            element.second.properties[k].value.push_back(d);
          }
          break;
        }
      }
    }
  }
  return true;
}

bool PlyLoader::load(std::string_view _filename)
{
  bool loaded = false;
  m_elements.clear();
  m_currentElement="";
  m_numElements=0;
  m_infile.open(_filename.data());
  if (!m_infile.is_open())
  {
    std::cout<<fmt::format(" file {0} not found  ", _filename.data());
    return false;
  }
  if( checkMagicNumber() )
  {
    // valid ply file parse the rest
    loaded=parseHeader();
  }
  if(loaded)
  {
    // now we have the header / format we can read in, we have either binary or ascii
    if(m_fileType == FileType::ASCII)
    {
      // read in ascii
      loaded=parseAscii();
    }
    else
    {
      // read in binary
      loaded=parseBinary();
    }
  }
  m_infile.close();
  return loaded;
}

void PlyLoader::debugPrint() const
{
  for (const auto &element : m_elements)
  {
    std::cout << fmt::format("Element {0} has {1} properties\n", element.first, element.second.count);
    for (const auto &property : element.second.properties)
    {
      std::cout << fmt::format("Property {0} has type {1}\n", property.name, static_cast<int>(property.type));
    }
    // print out the actual data
    for (size_t i = 0; i < element.second.count; ++i)
    {
//      for (const auto &property : element.second.properties)
//      {
//        switch (property.type)
//        {
//          case PropertyType::CHAR:
//            std::cout << fmt::format(" {0} ", std::get<char>(property.value));
//            break;
//          case PropertyType::UCHAR:
//            std::cout << fmt::format(" {0} ", std::get<unsigned char>(property.value));
//            break;
//          case PropertyType::SHORT:
//            std::cout << fmt::format(" {0} ", std::get<short>(property.value));
//            break;
//          case PropertyType::USHORT:
//            std::cout << fmt::format(" {0} ", std::get<unsigned short>(property.value));
//            break;
//          case PropertyType::INT:
//            std::cout << fmt::format(" {0} ", std::get<int>(property.value));
//            break;
//          case PropertyType::UINT:
//            std::cout << fmt::format(" {0} ", std::get<unsigned int>(property.value));
//            break;
//          case PropertyType::FLOAT:
//            std::cout << fmt::format(" {0} ", std::get<float>(property.value));
//            break;
//          case PropertyType::DOUBLE:
//            std::cout << fmt::format(" {0} ", std::get<double>(property.value));
//            break;
//        }
//      }
      std::cout << "\n";
    }

  }
}