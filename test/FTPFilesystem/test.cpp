#include <unity.h>
#include <SPIFFS.h>
#include <SD.h>
#include "FTPFilesystem.h"

void get_path_without_fs()
{
	String path = "/SD/foo/bar";
	String path_without = FTPFilesystem::getPathWithoutFS(path);
	TEST_ASSERT_EQUAL_STRING("/foo/bar", path_without.c_str());
}

void get_fs()
{
	FTPFilesystem f;
	f.addFilesystem("SD", &SD);
	FS * sd = f.getFilesystem("/SD/foo");
	TEST_ASSERT_EQUAL_PTR(&SD, sd);
}

void get_multiple_fs()
{
	FTPFilesystem f;
	f.addFilesystem("SD", &SD);
	f.addFilesystem("SPIFFS", &SPIFFS);
	FS * sd = f.getFilesystem("/SD/foo");
	TEST_ASSERT_EQUAL_PTR(&SD, sd);
	FS * spiffs = f.getFilesystem("/SPIFFS/foo");
	TEST_ASSERT_EQUAL_PTR(&SPIFFS, spiffs);
}


void setup()
{
	UNITY_BEGIN();
	RUN_TEST(get_path_without_fs);
	RUN_TEST(get_fs);
	RUN_TEST(get_multiple_fs);
	UNITY_END();
}

void loop()
{
}
