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
	const std::string dotaModPath = std::format("{}\\game\\dota_russian\\", dotaPath);
	printColorizedText(std::format("Gamedir: {}\n", dotaPath), CYAN);

	std::cout << "Fetching hashes...";

	if (App::Download(HOST, HASHES_FILENAME_HOST, HASHES_FILENAME_TMP) != 0)
	{
		printColorizedText("Error while fetching hashes. EXITING", RED);
		std::cin.get();
		return -1;
	}

	printColorizedText(" OK!\n", GREEN);

	std::vector<VpkInfo> vi = App::ReadHashes(HASHES_FILENAME_TMP);

	bool isAnythingHappended = false;

	for (int i = 0; i < vi.size(); i++)
	{
		const VpkInfo _vi = vi.at(i);

		if (!std::filesystem::exists(std::format("{}\\{}", dotaModPath, _vi.name).c_str()))
		{
			std::cout << std::format("Downloading {}...", _vi.name);
			if (App::Download(HOST, std::format("csnd/{}", _vi.name).c_str(), std::format("tmp_{}", _vi.name).c_str()) != 0)
			{
				printColorizedText("Error while downloading vpk file. EXITING", RED);
				std::cin.get();
				return -1;
			}

			isAnythingHappended = true;

			std::filesystem::rename(std::format("tmp_{}", _vi.name), std::format("{}\\{}", dotaModPath, _vi.name));

			printColorizedText(" OK!\n", GREEN);
		}
		else if (App::GetSha256(std::format("{}\\{}", dotaModPath, _vi.name).c_str()) != _vi.hash)
		{
			std::cout << std::format("Updating {}...", _vi.name);
			if (App::Download(HOST, std::format("csnd/{}", _vi.name).c_str(), std::format("tmp_{}", _vi.name).c_str()) != 0)
			{
				printColorizedText("Error while updating vpk file. EXITING", RED);
				std::cin.get();
				return -1;
			}

			isAnythingHappended = true;

			std::filesystem::remove(std::format("{}\\{}", dotaModPath, _vi.name));
			std::filesystem::rename(std::format("tmp_{}", _vi.name), std::format("{}\\{}", dotaModPath, _vi.name));
			printColorizedText(" OK!\n", GREEN);
		}

		if (!isAnythingHappended)
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