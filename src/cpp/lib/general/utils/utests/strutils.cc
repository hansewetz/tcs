#include "general/utils/strutils.h"
#include "gtest/gtest.h"
#include <iostream>

using namespace std;
using namespace tcs;

// ---- first (collection based) version of strcat
// strcat - basic case
TEST(strutils,strcat_1){
  vector<string>v{"one","two","3","4"};
  string res=strcat(0,v);
  ASSERT_EQ(res,"one0two0304");
}
// strcat - boundary case with one element
TEST(strutils,strcat_2){
  vector<string>v{"one"};
  string res=strcat(", ",v);
  ASSERT_EQ(res,"one");
}
// strcat - boundary case with no elements
TEST(strutils,strcat_3){
  vector<string>v;
  string res=strcat(", ",v);
  ASSERT_EQ(res,"");
}
// ---- second (argument pack based) version of strcat
// strcat - basic case
TEST(strutils,strcat_10){
  string res=strcat(0,"one","two",3,4);
  ASSERT_EQ(res,"one0two0304");
}
// strcat - boundary case with one element
TEST(strutils,strcat_11){
  string res=strcat(0,"one");
  ASSERT_EQ(res,"one");
}
// --- string case
// upper case
TEST(strutils,toupper){
  ASSERT_EQ("HELLO",toupper("hello"));
}
// lower case
TEST(strutils,tolower){
  ASSERT_EQ("hello",tolower("HELLO"));
}
