#include <cstdlib>
#include <cerrno>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <cstring>
#include <filesystem>

#include <openssl/sha.h>

#include <httplib.h>

#include "appframework.h"

steampp::Steam App::steam;

int App::Download(std::string host, std::string filename, std::string out)
{
	httplib::Client client(host);
	client.set_connection_timeout(10);
	client.set_read_timeout(30);

	std::ofstream ofs(out, std::ios::binary);
	if (!ofs)
	{
		return -1;
	}

	auto res = client.Get(std::format("/{}", filename), [&](const char* data, size_t len)
		{
			ofs.write(data, len);
			return true;
		});

	if (!res)
	{
		return -2;
	}

	return 0;
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
	std::ifstream ifs(path, std::ios::in);
	std::string str;

	std::vector<VpkInfo> vecVi;

	while (std::getline(ifs, str))
	{
		VpkInfo vi = {};
		const size_t separatorIndex = str.find(' ');
		vi.name = str.substr(0, separatorIndex);
		vi.hash = str.substr(separatorIndex + 1, str.length());
		vecVi.push_back({ vi });
	}

	return vecVi;
}

std::string App::GetAppInstallDir(uint32_t appId)
{
	return steam.getAppInstallDir(appId);
}