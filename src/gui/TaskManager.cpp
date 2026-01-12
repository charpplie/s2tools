#include "TaskManager.h"
#include "appframework.h"
#include "AppearanceSettings.h"

std::jthread TaskManager::taskThread;
std::mutex TaskManager::taskMutex;
TaskStatus TaskManager::taskStatus{};
bool TaskManager::show_debug_window = false;

namespace
{
	const std::string& GetDotaPath()
	{
		static std::string path = App::GetAppInstallDir(570);
		return path;
	}

	const std::filesystem::path& GetBasePath()
	{
		static std::filesystem::path path = std::filesystem::path(GetDotaPath()) / "game" / "dota_russian";
		return path;
	}
}

void TaskManager::StartVerifyTask()
{
	StartTask(TaskType::Verify, GetDotaPath(), GetBasePath());
}

void TaskManager::StartUpdateTask()
{
	StartTask(TaskType::Update, GetDotaPath(), GetBasePath());
}

void TaskManager::StopTask()
{
	if (taskThread.joinable())
	{
		taskThread.request_stop();
		TaskStatus cancelling;
		{
			std::lock_guard lock(taskMutex);
			cancelling.type = taskStatus.type;
			cancelling.state = TaskState::Running;
			cancelling.processed = taskStatus.processed;
			cancelling.total = taskStatus.total;
		}
		cancelling.message = "Cancelling...";
		SetStatus(std::move(cancelling));
	}
}

bool TaskManager::IsRunning()
{
	std::lock_guard lock(taskMutex);
	return taskStatus.state == TaskState::Running;
}

TaskStatus TaskManager::GetStatus()
{
	std::lock_guard lock(taskMutex);
	return taskStatus;
}

void TaskManager::SetStatus(TaskStatus status)
{
	std::lock_guard lock(taskMutex);
	taskStatus = std::move(status);
}

bool TaskManager::IsDebugWindowVisible()
{
	return show_debug_window;
}

void TaskManager::SetDebugWindowVisible(bool visible)
{
	show_debug_window = visible;
}

bool TaskManager::IsProgressRainbowEnabled()
{
	return AppearanceSettings::IsProgressRainbowEnabled();
}

float TaskManager::GetProgressRgbSpeed()
{
	return AppearanceSettings::GetProgressRgbSpeed();
}

int TaskManager::GetProgressSegmentWidth()
{
	return AppearanceSettings::GetProgressSegmentWidth();
}

void TaskManager::StartTask(TaskType type, const std::string& dotaPath, const std::filesystem::path& basePath)
{
	if (taskThread.joinable())
	{
		taskThread.join();
	}

	SetStatus({ type, TaskState::Running, "", 0, 0 });

	taskThread = std::jthread([type, dotaPath, basePath](std::stop_token stopToken)
		{
			TaskStatus status;
			status.type = type;
			status.state = TaskState::Running;
			status.processed = 0;
			status.total = 0;
			status.message.clear();

			const DownloadResult downloadResult = App::Download(std::string(HOST), std::string(HASHES_FILENAME_HOST), std::filesystem::path(HASHES_FILENAME_TMP));
			if (downloadResult.code != DownloadResult::Code::Ok)
			{
				status.state = TaskState::Error;
				status.message = std::format("Failed to download hashes: {} (http {})", downloadResult.message, downloadResult.httpStatus);
				TaskManager::SetStatus(status);
				return;
			}

			if (stopToken.stop_requested())
			{
				status.state = TaskState::Error;
				status.message = "Operation cancelled by user.";
				TaskManager::SetStatus(status);
				return;
			}

			std::vector<VpkInfo> hashes = App::ReadHashes(HASHES_FILENAME_TMP);
			std::filesystem::remove(HASHES_FILENAME_TMP);

			if (hashes.empty())
			{
				status.state = TaskState::Error;
				status.message = "Hashes list is empty or invalid";
				TaskManager::SetStatus(status);
				return;
			}

			status.total = hashes.size();

			std::string progressLog;

			for (const VpkInfo& vi : hashes)
			{
				if (stopToken.stop_requested())
				{
					status.state = TaskState::Error;
					status.message = "Operation cancelled by user.";
					TaskManager::SetStatus(status);
					return;
				}

				const std::filesystem::path target = basePath / vi.name;
				const std::string tempName = std::format("tmp_{}", vi.name);

				if (type == TaskType::Update)
				{
					const bool exists = std::filesystem::exists(target);
					const bool upToDate = exists && App::GetSha256(target.string().c_str()) == vi.hash;

					if (!exists || !upToDate)
					{
						const DownloadResult dl = App::Download(std::string(HOST), std::format("csnd/{}", vi.name), std::filesystem::path(tempName));
						if (dl.code != DownloadResult::Code::Ok)
						{
							status.state = TaskState::Error;
							status.message = std::format("Failed to download {}: {} (http {})", vi.name, dl.message, dl.httpStatus);
							TaskManager::SetStatus(status);
							return;
						}

						if (stopToken.stop_requested())
						{
							std::error_code ec;
							std::filesystem::remove(tempName, ec);
							status.state = TaskState::Error;
							status.message = "Operation cancelled by user.";
							TaskManager::SetStatus(status);
							return;
						}

						if (exists)
						{
							std::error_code ec;
							std::filesystem::remove(target, ec);
						}

						std::error_code renameEc;
						std::filesystem::rename(tempName, target, renameEc);
						if (renameEc)
						{
							status.state = TaskState::Error;
							status.message = std::format("Failed to move downloaded file {} -> {}: {}", tempName, target.string(), renameEc.message());
							TaskManager::SetStatus(status);
							return;
						}

						progressLog += std::format("\nUpdated {}", vi.name);
					}
					else
					{
						progressLog += std::format("\nAlready up-to-date: {}", vi.name);
					}
				}
				else if (type == TaskType::Verify)
				{
					const bool exists = std::filesystem::exists(target);
					if (!exists)
					{
						progressLog += std::format("\nMissing locally: {}", vi.name);
					}
					else if (App::GetSha256(target.string().c_str()) != vi.hash)
					{
						progressLog += std::format("\nOut-of-Date: {}", vi.name);
					}
					else
					{
						progressLog += std::format("\nUp-to-Date: {}", vi.name);
					}
				}

				status.processed++;
				status.message = progressLog;
				TaskManager::SetStatus(status);
			}

			status.state = TaskState::Success;
			if (progressLog.empty())
			{
				status.message = "All files are up-to-date. Nothing to do.";
			}
			else
			{
				status.message = progressLog;
			}
			TaskManager::SetStatus(status);
		});
}