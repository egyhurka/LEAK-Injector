#include "File.h"

#include <random>
#include <filesystem>

namespace fs = std::filesystem;

std::filesystem::path injector::file::CreateTempDir()
{

	fs::path tempDir = fs::temp_directory_path() / DLL_TEMP_DIRECTORY_NAME;
	if (!fs::exists(tempDir))
	{
		fs::create_directories(tempDir);
	}

	return tempDir;
}

bool injector::file::ClearTempDirectory()
{
	fs::path dir = fs::temp_directory_path() / DLL_TEMP_DIRECTORY_NAME;

	if (!fs::exists(dir))
		return true;

	for (const auto& entry : fs::directory_iterator(dir))
	{
		fs::remove_all(entry.path());
	}

	return true;
}

std::wstring injector::file::CopyFileToTempDir(const std::wstring& filePath)
{
	try
	{
		fs::path srcPath(filePath);
		std::wstring originalName = srcPath.stem().wstring();
		std::wstring extension = srcPath.extension().wstring();

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, 15);


		std::stringstream ss;
		for (int i = 0; i < 4; ++i) {
			ss << std::hex << dis(gen);
		}

		std::string randomStr = ss.str();
		std::wstring tag(randomStr.begin(), randomStr.end());

		std::wstring uniqueFileName = originalName + L"_" + tag + extension;

		fs::path tempDir = CreateTempDir();

		fs::path destPath = tempDir / uniqueFileName;
		fs::copy_file(srcPath, destPath, fs::copy_options::overwrite_existing);

		return destPath.wstring();
	}
	catch (...)
	{
		return L"";
	}
}

SIZE_T injector::file::GetFilePathSize(const std::wstring& filePath)
{
	return (filePath.length() + 1) * sizeof(wchar_t);
}
