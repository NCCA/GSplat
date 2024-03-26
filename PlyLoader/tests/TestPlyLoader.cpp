#include <gtest/gtest.h>
#include "PlyLoader.h"
TEST(PlyLoader,ctor)
{
  PlyLoader p;
  ASSERT_EQ(p.getNumElements(),0);
}

TEST(PlyLoader,testNotPLYFile)
{
  PlyLoader p;
  EXPECT_FALSE(p.load("files/invalidMagicNumber.ply"));
  EXPECT_TRUE(p.load("files/validMagicNumber.ply"));
}

TEST(PlyLoader,loadFull)
{
  PlyLoader p;
  ASSERT_TRUE(p.load("files/validAscii.ply"));
  p.debugPrint();
  ASSERT_TRUE(p.load("files/validBinary.ply"));
  p.debugPrint();

//  ASSERT_TRUE(p.load("/Users/jmacey/teaching/Code/GSplat/bonsai/point_cloud/iteration_30000/point_cloud.ply"));
//  p.debugPrint();



}