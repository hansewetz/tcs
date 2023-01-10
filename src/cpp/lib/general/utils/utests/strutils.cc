#include "general/utils/strutils.h"
#include "gtest/gtest.h"
#include <iostream>

using namespace std;
using namespace tcs;

// ---- first (collection based) version of strcat
// strcat - basic case
TEST(strutils,strcat_10){
  vector<string>v{"one","two","3","4"};
  string res=strcat(0,v);
  ASSERT_EQ(res,"one0two0304");
}
// strcat - boundary case with one element
TEST(strutils,strcat_11){
  vector<string>v{"one"};
  string res=strcat(", ",v);
  ASSERT_EQ(res,"one");
}
// strcat - boundary case with no elements
TEST(strutils,strcat_12){
  vector<string>v;
  string res=strcat(", ",v);
  ASSERT_EQ(res,"");
}
// ---- second (argument pack based) version of strcat
// strcat - basic case
TEST(strutils,strcat_20){
  string res=strcat(0,"one","two",3,4);
  ASSERT_EQ(res,"one0two0304");
}
// strcat - boundary case with one element
TEST(strutils,strcat_21){
  string res=strcat(0,"one");
  ASSERT_EQ(res,"one");
}
// ---- third (tuple based) version of strcat
// 'normal' case with multiple elements in tuple
TEST(strutils,strcat_30){
  auto tup=make_tuple("1","2",3,4,'5');
  string res=strcat(0,tup);
  ASSERT_EQ(res,"102030405");
}
// boundary case with no elements in tuple
TEST(strutils,strcat_31){
  auto tup=make_tuple();
  string res=strcat(", "s,tup);
  ASSERT_EQ(res,"");
} 
// boundary case witgh one element in tuple
TEST(strutil,strcat_32){
  auto tup=make_tuple(1);
  string res=strcat(", "s,tup);
  ASSERT_EQ(res,"1");
}

// NOTE! not yet done

// --- string case
// upper case
TEST(strutils,toupper){
  ASSERT_EQ("HELLO",toupper("hello"));
}
// lower case
TEST(strutils,tolower){
  ASSERT_EQ("hello",tolower("HELLO"));
}
