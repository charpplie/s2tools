#pragma once

#include <string>
#include <filesystem>

namespace Tabs
{
	class SettingsTab
	{
	public:
		static void Draw();

	private:
		static void DrawGeneralSettings();
		static void DrawAppearanceSettings();
	};
}