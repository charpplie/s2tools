#include "Tools.h"

#define S2_GUI
#include "pch.h"

namespace Tabs
{
	void ToolsTab::Draw()
	{
		if (ImGui::BeginTabBar("ToolsTabBar", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("MDL"))
			{
				ImGui::PushID("MDLTab");
				DrawMDLHelpers();
				ImGui::PopID();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Python"))
			{
				ImGui::PushID("PythonTab");
				DrawPythonHelpers();
				ImGui::PopID();
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}

	void ToolsTab::DrawPythonHelpers()
	{
		ImGui::Text("test");
	}

	void ToolsTab::DrawMDLHelpers()
	{
		ImGui::Text("Mdl");
	}
}