#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include "appframework.h"
#include "pch.h"

#ifdef _WIN32
#define RED 12
#define GREEN 10
#define YELLOW 14
#define CYAN 11
#define WHITE 7
#endif

void printColorizedText(std::string_view text, const uint8_t color);

int main()
{
	printColorizedText("Dotamod Content Autoupdater - https://dotamod.ru\n", YELLOW);

	const std::string dotaPath = App::GetAppInstallDir(570);
	const std::filesystem::path dotaModPath = std::filesystem::path(dotaPath) / "game" / "dota_russian";
	printColorizedText(std::format("Gamedir: {}\n", dotaPath), CYAN);

	std::cout << "Fetching hashes...";

	{
		const DownloadResult dr = App::Download(HOST, HASHES_FILENAME_HOST, HASHES_FILENAME_TMP);
		if (dr.code != DownloadResult::Code::Ok)
		{
			printColorizedText(std::format("Error while fetching hashes: {} (http {})", dr.message, dr.httpStatus).c_str(), RED);
			std::cin.get();
			return -1;
		}
	}

	printColorizedText(" OK!\n", GREEN);

	std::vector<VpkInfo> vi = App::ReadHashes(HASHES_FILENAME_TMP);

	bool isAnythingHappened = false;

	for (size_t i = 0; i < vi.size(); ++i)
	{
		const VpkInfo& _vi = vi.at(i);
		const std::filesystem::path target = dotaModPath / _vi.name;

		if (!std::filesystem::exists(target))
		{
			std::cout << std::format("Downloading {}...", _vi.name);
			const DownloadResult dl = App::Download(HOST, std::format("csnd/{}", _vi.name), std::filesystem::path(std::format("tmp_{}", _vi.name)));
			if (dl.code != DownloadResult::Code::Ok)
			{
				printColorizedText(std::format("Error while downloading vpk file: {} (http {})", dl.message, dl.httpStatus).c_str(), RED);
				std::cin.get();
				return -1;
			}

			isAnythingHappened = true;

			std::error_code ec;
			std::filesystem::rename(std::format("tmp_{}", _vi.name), target, ec);
			if (ec)
			{
				printColorizedText(std::format("Error moving downloaded file: {}", ec.message()).c_str(), RED);
				std::cin.get();
				return -1;
			}

			printColorizedText(" OK!\n", GREEN);
		}
		else if (App::GetSha256(target.string().c_str()) != _vi.hash)
		{
			std::cout << std::format("Updating {}...", _vi.name);
			const DownloadResult dl = App::Download(HOST, std::format("csnd/{}", _vi.name), std::filesystem::path(std::format("tmp_{}", _vi.name)));
			if (dl.code != DownloadResult::Code::Ok)
			{
				printColorizedText(std::format("Error while updating vpk file: {} (http {})", dl.message, dl.httpStatus).c_str(), RED);
				std::cin.get();
				return -1;
			}

			isAnythingHappened = true;

			std::error_code ec;
			std::filesystem::remove(target, ec);
			std::filesystem::rename(std::format("tmp_{}", _vi.name), target, ec);
			if (ec)
			{
				printColorizedText(std::format("Error moving downloaded file: {}", ec.message()).c_str(), RED);
				std::cin.get();
				return -1;
			}
			printColorizedText(" OK!\n", GREEN);
		}

		if (!isAnythingHappened)
		{
			std::cout << "All files are up-to-date. Nothing to update\n";
			break;
		}
	}

	std::filesystem::remove(HASHES_FILENAME_TMP);
	std::cout << "All things are done (or not). Press Enter to exit\n";
	std::cin.get();

	return 0;
}

void printColorizedText(std::string_view text, const uint8_t color)
{
#ifdef _WIN32
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, color);
	std::cout << text;
	SetConsoleTextAttribute(hConsole, WHITE);
#endif
}