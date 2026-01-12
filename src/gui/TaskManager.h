#pragma once

#include <string>
#include <thread>
#include <mutex>
#include <filesystem>

#define S2_GUI
#include "pch.h"

enum class TaskType : uint8_t
{
	None,
	Verify,
	Update
};

enum class TaskState : uint8_t
{
	Idle,
	Running,
	Success,
	Error
};

struct TaskStatus
{
	TaskType type{ TaskType::None };
	TaskState state{ TaskState::Idle };
	std::string message{};
	std::size_t processed{ 0 };
	std::size_t total{ 0 };
};

class TaskManager
{
public:
	static void StartVerifyTask();
	static void StartUpdateTask();
	static void StopTask();
	static bool IsRunning();
	static TaskStatus GetStatus();
	static void SetStatus(TaskStatus status);

	static bool IsDebugWindowVisible();
	static void SetDebugWindowVisible(bool visible);

	static bool IsProgressRainbowEnabled();
	static float GetProgressRgbSpeed();
	static int GetProgressSegmentWidth();

private:
	static std::jthread taskThread;
	static std::mutex taskMutex;
	static TaskStatus taskStatus;
	static bool show_debug_window;

	static void StartTask(TaskType type, const std::string& dotaPath, const std::filesystem::path& basePath);
};