#include <string>
#include <fstream>
#include <istream>
#include <vector>
//#include <algorithm>
#include <filesystem>
//#include <thread>

#include <steampp/steampp.h>

#include <ShObjIdl_core.h>

#include "appframework.h"
#include "UI.h"

#define WIDTH 500
#define HEIGHT 150

#define W_MODS 500
#define H_MODS 250

LPCSTR UI::lpWindowName = "Dotamod Toolkit";
ImVec2 UI::vWindowSize = { WIDTH, HEIGHT };
ImGuiWindowFlags UI::WindowFlags = 0;
bool UI::show_main_window = true;
bool UI::show_mods_window = false;
bool UI::show_debug_window = false;

const COMDLG_FILTERSPEC vpkSpec[1] = { {L"Valve PacK (*.vpk)", L"*.vpk"} };

int screenWidth = GetSystemMetrics(SM_CXSCREEN);
int screenHeight = GetSystemMetrics(SM_CYSCREEN);

float x = (screenWidth - WIDTH) / 2;
float y = (screenHeight - HEIGHT) / 2;

bool UI::isActive()
{
	return show_main_window == true;
}

void UI::Draw()
{
	static std::string dotaPath = App::GetAppInstallDir(570);
	static std::string basePath = std::format("{}\\game\\dota_russian\\", dotaPath);

	if (isActive())
	{
		ImGui::SetNextWindowSize(vWindowSize, ImGuiCond_Once);
		ImGui::SetNextWindowBgAlpha(1.0f);
		ImGui::SetNextWindowPos({ x, y }, ImGuiCond_Once);
		ImGui::Begin(lpWindowName, &show_main_window, WindowFlags);

		{
			ImGui::Checkbox("Mods", &show_mods_window);

			if (show_mods_window)
			{
			mods_start:
				static std::string lastMsg = "";

				static bool verifying = FALSE;
				static uint8_t verified = MODS_NOT_VERIFIED;

				static bool updating = FALSE;
				static bool updatedFiles = 0;

				ImGui::SetNextWindowSize({ W_MODS, H_MODS }, ImGuiCond_Appearing);
				ImGui::Begin("Mods", &show_mods_window);

				if (ImGui::Button("Verify"))
				{
					verifying = TRUE;
					verified = MODS_NOT_VERIFIED;
				}

				ImGui::SameLine();

				if (ImGui::Button("Update"))
				{
					updating = TRUE;
				}

				ImGui::Text(std::format("Game dir: {}", dotaPath).c_str());
				ImGui::Button("Change");
				
				if (lastMsg != "")
				{
					ImGui::Text(lastMsg.c_str());
				}

				if (updating)
				{
					updating = FALSE;
					updatedFiles = 0;
					lastMsg = "";

					if (App::Download(HOST, HASHES_FILENAME_HOST, HASHES_FILENAME_TMP) != 0)
					{
						EXIT_IMGUI("Error while downloading hashes", mods_start)
					}

					std::vector<VpkInfo> vi = App::ReadHashes(HASHES_FILENAME_TMP);

					std::string status = "";

					//constexpr const char* basePath = "D:\\SteamLibrary\\steamapps\\common\\dota 2 beta\\game\\dota_russian\\";

					for (int i = 0; i < vi.size(); i++)
					{
						ImGui::Text(status.c_str());
						const VpkInfo _vi = vi.at(i);

						//int res = 0;

						//std::jthread _thread([&_vi, &res]()
						//{
						//	res = App::Download(HOST, std::format("csnd/{}", _vi.name).c_str(), std::format("tmp_{}", _vi.name).c_str());
						//});

						if (!std::filesystem::exists(std::format("{}\\{}", basePath, _vi.name).c_str()))
						{
							//std::string _status = std::format("\nDownloading {} (missing locally)", _vi.name);
							//ImGui::Text(_status.c_str());
							//status += _status;

							if (App::Download(HOST, std::format("csnd/{}", _vi.name).c_str(), std::format("tmp_{}", _vi.name).c_str()) != 0)
							{
								EXIT_IMGUI(std::format("Error while downloading {}", _vi.name), mods_start)
							}

							//std::filesystem::remove(std::format("{}\\{}", basePath, _vi.name));
							std::filesystem::rename(std::format("tmp_{}", _vi.name), std::format("{}\\{}", basePath, _vi.name));

							//_status = std::format("\nDownloaded {}", _vi.name);
							//ImGui::Text(_status.c_str());
							status += std::format("\nDownloaded {}", _vi.name);
						}
						else if (App::GetSha256(std::format("{}\\{}", basePath, _vi.name).c_str()) != _vi.hash)
						{
							//std::string _status = std::format("\nDownloading {} (missing locally)", _vi.name);
							//ImGui::Text(_status.c_str());
							//status += std::format("\nDownloading {} (out-of-date)", _vi.name);

							//static std::thread _t(App::Download, HOST, std::format("csnd/{}", _vi.name).c_str(), std::format("tmp_{}", _vi.name).c_str());

							if (App::Download(HOST, std::format("csnd/{}", _vi.name).c_str(), std::format("tmp_{}", _vi.name).c_str()) != 0)
							{
								EXIT_IMGUI("some error", mods_start)
							}

							//_t.join();

							std::filesystem::remove(std::format("{}\\{}", basePath, _vi.name));
							std::filesystem::rename(std::format("tmp_{}", _vi.name), std::format("{}\\{}", basePath, _vi.name));
							status += std::format("\nUpdated {}", _vi.name);
						}
					}

					lastMsg = status == "" ? "All files are up-to-date. Nothing to update" : status;
				}

				if (verifying)
				{
					verifying = FALSE;
					lastMsg = "";

					ImGui::Text("Fetching hashes...");

					if (App::Download(HOST, HASHES_FILENAME_HOST, HASHES_FILENAME_TMP) != 0)
					{
						EXIT_IMGUI("Some error lol didnt care lmao go cry kiddy", mods_start)
					}

					ImGui::Text("Verifying hashes...");

					std::vector<VpkInfo> vi = App::ReadHashes(HASHES_FILENAME_TMP);

					std::remove(HASHES_FILENAME_TMP);

					std::string status = "";
					std::string missing = "";
					std::string outdated = "";
					std::string fresh = "";

					//constexpr const char* basePath = "D:\\SteamLibrary\\steamapps\\common\\dota 2 beta\\game\\dota_russian\\";

					for (int i = 0; i < vi.size(); i++)
					{
						const VpkInfo _vi = vi.at(i);
						if (!std::filesystem::exists(std::format("{}\\{}", basePath, _vi.name).c_str()))
						{
							missing += std::format("\nMissing locally: {}", _vi.name);
						}
						else if (App::GetSha256(std::format("{}\\{}", basePath, _vi.name).c_str()) != _vi.hash)
						{
							outdated += std::format("\nOut-of-Date: {}", _vi.name);
						}
						else
						{
							fresh += std::format("\nUp-to-Date: {}", _vi.name);
						}
					}

					status = std::format("{}{}{}", missing, outdated, fresh);

					lastMsg = status;
				}

				switch (verified)
				{
					case MODS_NOT_VERIFIED:
						break;
					case MODS_NOT_INSTALLED:
						ImGui::Text("Mods not installed");
						break;
					case MODS_VERIFIED_FRESH:
						ImGui::Text("Ok");
						break;
					case MODS_VERIFIED_OUTOFDATE:
						ImGui::Text("Out-of-Date");
						break;
					default:
						std::unreachable();
				}

				ImGui::End();
			}

			ImGui::Checkbox("Debug", &show_debug_window);

			if (show_debug_window)
			{
				ImGui::Begin("Debug", &show_debug_window);

				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

				ImGui::End();
			}
		}

		ImGui::End();
	}
}

std::string UI::FileOpenDialog(HWND hWnd)
{
	IFileOpenDialog* pfd = NULL;
	HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));

	pfd->SetFileTypes(ARRAYSIZE(vpkSpec), vpkSpec);
	hr = pfd->Show(hWnd);

	std::string ret = "";

	if (SUCCEEDED(hr))
	{
		IShellItem* psiResult = NULL;
		hr = pfd->GetResult(&psiResult);

		if (SUCCEEDED(hr))
		{
			PWSTR pszFilePath = NULL;
			hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

			if (SUCCEEDED(hr))
			{
				size_t convertedChars = 0;
				size_t origSize = wcslen(pszFilePath) + 1;
				const size_t newSize = origSize * 2;
				char* nstring = new char[newSize];
				wcstombs_s(&convertedChars, nstring, newSize, pszFilePath, _TRUNCATE);

				ret = nstring;

				delete[] nstring;
				CoTaskMemFree(pszFilePath);
			}

			psiResult->Release();
		}
	}

	return ret;
}