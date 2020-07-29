#include <unity.h>
#include "FTPPath.h"

void split_path_test_1()
{
	String p = "/";
	std::list<String> splited = FTPPath::splitPath(p);
	TEST_ASSERT_EQUAL(0, splited.size());
}

void split_path_test_2()
{
	String p = "/SD/";
	std::list<String> splited = FTPPath::splitPath(p);
	TEST_ASSERT_EQUAL(1, splited.size());
}

void split_path_test_3()
{
	String p = "/SD/abc/";
	std::list<String> splited = FTPPath::splitPath(p);
	TEST_ASSERT_EQUAL(2, splited.size());
}

void split_path_test_4()
{
	String p = "/SD/abc.txt";
	std::list<String> splited = FTPPath::splitPath(p);
	TEST_ASSERT_EQUAL(2, splited.size());
}

void split_path_test_5()
{
	String p = "/SD/abc/abc.txt";
	std::list<String> splited = FTPPath::splitPath(p);
	TEST_ASSERT_EQUAL(3, splited.size());
}

void set_dir()
{
	FTPPath path("/");
	TEST_ASSERT_EQUAL_STRING("/", path.getPath().c_str());
}

void change_dir_1()
{
	FTPPath path("/");
	path.changePath("foo");
	TEST_ASSERT_EQUAL_STRING("/foo", path.getPath().c_str());
}

void change_dir_2()
{
	FTPPath path("/");
	path.changePath("foo");
	path.changePath("bar");
	TEST_ASSERT_EQUAL_STRING("/foo/bar", path.getPath().c_str());
}

void get_file_1()
{
	FTPPath path("/");
	path.changePath("foo");
	path.changePath("bar");
	TEST_ASSERT_EQUAL_STRING("/foo/bar/foo.txt", path.getFilePath("foo.txt").c_str());
}

void go_dir_up()
{
	FTPPath path("/");
	path.changePath("foo");
	path.changePath("bar");
	path.goPathUp();
	TEST_ASSERT_EQUAL_STRING("/foo", path.getPath().c_str());

	TEST_ASSERT_EQUAL_STRING("/foo/bar.txt", path.getFilePath("bar.txt").c_str());
}

void get_file_2()
{
	FTPPath path("/");
	path.changePath("foo");
	path.changePath("bar");
	path.goPathUp();
	TEST_ASSERT_EQUAL_STRING("/foo/bar.txt", path.getFilePath("bar.txt").c_str());
}

void setup()
{
	UNITY_BEGIN();
	RUN_TEST(split_path_test_1);
	RUN_TEST(split_path_test_2);
	RUN_TEST(split_path_test_3);
	RUN_TEST(split_path_test_4);
	RUN_TEST(split_path_test_5);
	RUN_TEST(set_dir);
	RUN_TEST(change_dir_1);
	RUN_TEST(change_dir_2);
	RUN_TEST(get_file_1);
	RUN_TEST(go_dir_up);
	RUN_TEST(get_file_1);
	UNITY_END();
}

void loop()
{
}
