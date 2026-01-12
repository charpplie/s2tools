#pragma once

#ifndef _S2_APPFW_H__
#define _S2_APPFW_H__

#include <string>
#include <vector>
#include <filesystem>

#include <steampp/steampp.h>

#define HOST "http://176.124.204.212"

#define HASHES_FILENAME_TMP "hashes_tmp"
#define HASHES_FILENAME_HOST "csnd/hashes"

#define MODS_NOT_VERIFIED 0
#define MODS_NOT_INSTALLED 1
#define MODS_VERIFIED_FRESH 2
#define MODS_VERIFIED_OUTOFDATE 3

struct VpkInfo
{
	std::string name;
	std::string hash;
};

struct DownloadResult
{
	enum class Code : uint8_t
	{
		Ok,
		InvalidArgs,
		FileOpenError,
		NetworkError,
		HttpError,
		WriteError,
		RenameError,
		UnknownError
	} code{ Code::UnknownError };

	int httpStatus{ 0 };
	std::string message;
	std::size_t bytesDownloaded{ 0 };
};

class App
{
private:
	static steampp::Steam steam;

public:
	static DownloadResult Download(const std::string& host, const std::string& filename, const std::filesystem::path& outPath, int maxRetries = 3);
	static std::string GetSha256(const char* path);
	static std::vector<VpkInfo> ReadHashes(const char* path);
	static std::string GetAppInstallDir(uint32_t appId);
};

#endif // _S2_APPFW_H__