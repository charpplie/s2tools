#include <cstdlib>
#include <cerrno>
#include <cctype>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <cstring>
#include <filesystem>
#include <thread>
#include <chrono>
#include <system_error>

#include <openssl/sha.h>

#include <httplib.h>

#include "appframework.h"

steampp::Steam App::steam;

DownloadResult App::Download(const std::string& host, const std::string& filename, const std::filesystem::path& outPath, int maxRetries)
{
	DownloadResult result;
	if (host.empty() || filename.empty() || outPath.empty())
	{
		result.code = DownloadResult::Code::InvalidArgs;
		result.message = "Invalid arguments";
		return result;
	}

	const std::string route = std::format("/{}", filename);
	
	// Resolve the actual output directory
	std::filesystem::path actualOutPath = outPath;
	if (!actualOutPath.is_absolute())
	{
		actualOutPath = std::filesystem::current_path() / actualOutPath;
	}

	std::filesystem::path tmpPath = actualOutPath.parent_path() / std::format("{}.tmp", actualOutPath.filename().string());

	constexpr std::chrono::milliseconds initialBackoff{ 500 };

	int attempt = 0;
	std::chrono::milliseconds backoff = initialBackoff;

	while (attempt <= maxRetries)
	{
		attempt++;

		try
		{
			httplib::Client client(host);
			client.set_connection_timeout(10);
			client.set_read_timeout(30);

			// Create destination directory if needed
			std::error_code ec;
			std::filesystem::create_directories(actualOutPath.parent_path(), ec);
			if (ec)
			{
				result.code = DownloadResult::Code::WriteError;
				result.message = std::format("Failed to create destination directory '{}': {}", actualOutPath.parent_path().string(), ec.message());
				return result;
			}

			// Open temp file for writing
			std::ofstream ofs(tmpPath, std::ios::binary);
			if (!ofs.is_open())
			{
				result.code = DownloadResult::Code::FileOpenError;
				result.message = std::format("Failed to open temporary file '{}'", tmpPath.string());
				return result;
			}

			std::size_t bytesWritten = 0;
			auto res = client.Get(route.c_str(), [&](const char* data, size_t len) {
				if (!ofs.write(data, static_cast<std::streamsize>(len)))
				{
					// write error
					return false;
				}
				bytesWritten += len;
				return true;
			});

			ofs.close();

			if (!res)
			{
				// network / connection error
				result.code = DownloadResult::Code::NetworkError;
				result.httpStatus = 0;
				result.message = std::format("Network error on attempt {}/{}", attempt, maxRetries + 1);
				result.bytesDownloaded = bytesWritten;

				// retryable network error: if attempts left, backoff and retry
				if (attempt <= maxRetries)
				{
					std::error_code ignore;
					std::filesystem::remove(tmpPath, ignore);
					std::this_thread::sleep_for(backoff);
					backoff *= 2;
					continue;
				}

				std::error_code ignore;
				std::filesystem::remove(tmpPath, ignore);
				return result;
			}

			// We have an HTTP response
			result.httpStatus = res->status;
			if (res->status != 200)
			{
				result.bytesDownloaded = bytesWritten;
				result.message = std::format("HTTP error: status {}", res->status);
				std::error_code ignore;
				std::filesystem::remove(tmpPath, ignore);
				
				// Retry on 5xx
				if (res->status >= 500 && res->status < 600 && attempt <= maxRetries)
				{
					result.code = DownloadResult::Code::HttpError;
					std::this_thread::sleep_for(backoff);
					backoff *= 2;
					continue;
				}
				// Non-retryable HTTP error
				result.code = DownloadResult::Code::HttpError;
				return result;
			}

			// Success: move tmp -> actualOutPath
			std::error_code renameEc;
			std::filesystem::rename(tmpPath, actualOutPath, renameEc);
			if (renameEc)
			{
				std::error_code copyEc;
				std::error_code ignore;
				std::filesystem::copy_file(tmpPath, actualOutPath, std::filesystem::copy_options::overwrite_existing, copyEc);
				if (copyEc)
				{
					result.code = DownloadResult::Code::RenameError;
					result.message = std::format("Failed to move temporary file to destination: {}", copyEc.message());
					std::filesystem::remove(tmpPath, ignore);
					return result;
				}
				else
				{
					std::filesystem::remove(tmpPath, ignore);
				}
			}

			result.code = DownloadResult::Code::Ok;
			result.bytesDownloaded = bytesWritten;
			result.message = "OK";
			return result;
		}
		catch (const std::exception& ex)
		{
			result.code = DownloadResult::Code::UnknownError;
			result.message = std::format("Exception: {}", ex.what());
			// retry on exception if attempts left
			if (attempt <= maxRetries)
			{
				std::this_thread::sleep_for(backoff);
				backoff *= 2;
				continue;
			}
			return result;
		}
	}

	// fallback
	result.code = DownloadResult::Code::UnknownError;
	result.message = "Unresolved error";
	return result;
}

std::string App::GetSha256(const char* path)
{
	std::ifstream ifs(path, std::ios::in | std::ios::binary);

	constexpr const std::size_t bufSize{ 1 << 12 };
	char buffer[bufSize];

	unsigned char hash[SHA256_DIGEST_LENGTH] = { 0 };

	SHA256_CTX ctx;
	SHA256_Init(&ctx);

	while (ifs.good())
	{
		ifs.read(buffer, bufSize);
		SHA256_Update(&ctx, buffer, ifs.gcount());
	}

	SHA256_Final(hash, &ctx);
	ifs.close();

	std::ostringstream os;
	os << std::hex << std::setfill('0');

	for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
	{
		os << std::setw(2) << static_cast<unsigned int>(hash[i]);
	}

	return os.str();
}

std::vector<VpkInfo> App::ReadHashes(const char* path)
{
	std::vector<VpkInfo> vecVi;

	if (path == nullptr || *path == '\0')
	{
		std::cerr << "ReadHashes: path is empty\n";
		return vecVi;
	}

	std::ifstream ifs(path, std::ios::in);
	if (!ifs.is_open())
	{
		std::cerr << "ReadHashes: failed to open '" << path << "': " << std::strerror(errno) << '\n';
		return vecVi;
	}

	auto trim = [](std::string& s)
	{
		while (!s.empty() && std::isspace(static_cast<unsigned char>(s.front())))
		{
			s.erase(s.begin());
		}
		while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back())))
		{
			s.pop_back();
		}
	};

	std::string line;
	std::size_t lineNo = 0;

	while (std::getline(ifs, line))
	{
		++lineNo;
		trim(line);

		if (line.empty())
		{
			continue;
		}

		const std::size_t separatorIndex = line.find(' ');
		if (separatorIndex == std::string::npos)
		{
			std::cerr << "ReadHashes: invalid line " << lineNo << " (no delimiter)\n";
			continue;
		}

		std::string name = line.substr(0, separatorIndex);
		std::string hash = line.substr(separatorIndex + 1);

		trim(name);	
		trim(hash);

		if (name.empty() || hash.empty())
		{
			std::cerr << "ReadHashes: invalid line " << lineNo << " (missing name/hash)\n";
			continue;
		}

		vecVi.push_back({ std::move(name), std::move(hash) });
	}

	return vecVi;
}

std::string App::GetAppInstallDir(uint32_t appId)
{
	return steam.getAppInstallDir(appId);
}