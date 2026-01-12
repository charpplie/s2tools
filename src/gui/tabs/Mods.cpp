#include "Mods.h"
#include "appframework.h"
#include "../TaskManager.h"

#define S2_GUI
#include "pch.h"

namespace Tabs
{
	void ModsTab::Draw()
	{
		const bool taskRunning = TaskManager::IsRunning();

		ImGui::BeginDisabled(taskRunning);
		if (ImGui::Button("Verify"))
		{
			TaskManager::StartVerifyTask();
		}
		ImGui::EndDisabled();

		ImGui::SameLine();

		ImGui::BeginDisabled(taskRunning);
		if (ImGui::Button("Update"))
		{
			TaskManager::StartUpdateTask();
		}
		ImGui::EndDisabled();

		ImGui::SameLine();

		ImGui::BeginDisabled(!taskRunning);
		if (ImGui::Button("Stop"))
		{
			TaskManager::StopTask();
		}
		ImGui::EndDisabled();

		TaskStatus snapshot = TaskManager::GetStatus();

		if (snapshot.state == TaskState::Running || snapshot.state == TaskState::Success)
		{
			const float progress = snapshot.state == TaskState::Success
				? 1.0f
				: (snapshot.total == 0 ? 0.0f : static_cast<float>(snapshot.processed) / static_cast<float>(snapshot.total));

			// draw custom progress bar so we can render rainbow gradient animation
			const ImVec2 cursor = ImGui::GetCursorScreenPos();
			const float avail_x = ImGui::GetContentRegionAvail().x;
			const float bar_h = ImGui::GetFrameHeight();
			const ImVec2 bb_min = cursor;
			const ImVec2 bb_max = ImVec2(cursor.x + avail_x, cursor.y + bar_h);
			ImDrawList* dl = ImGui::GetWindowDrawList();

			// background
			const ImU32 bg_col = ImGui::GetColorU32(ImGuiCol_FrameBg);
			dl->AddRectFilled(bb_min, bb_max, bg_col, ImGui::GetStyle().FrameRounding);

			// filled portion
			const float filled_w = avail_x * progress;
			if (filled_w > 0.0f)
			{
				if (TaskManager::IsProgressRainbowEnabled())
				{
					// base hue cycles with time
					const float baseHue = std::fmod(static_cast<float>(ImGui::GetTime()) * TaskManager::GetProgressRgbSpeed(), 1.0f);
					const int segW = (std::max)(1, TaskManager::GetProgressSegmentWidth());
					const int steps = (std::max)(1, static_cast<int>(std::ceil(filled_w / float(segW))));
					for (int i = 0; i < steps; ++i)
					{
						const float x0 = float(i * segW);
						const float x1 = (std::min)(filled_w, x0 + float(segW));
						if (x1 <= x0) break;
						// hue along the progress creates full rainbow along the bar
						const float t = (filled_w > 0.0f) ? (x0 / filled_w) : 0.0f;
						float hue = baseHue + t;
						hue = hue - std::floor(hue); // wrap 0..1
						float r, g, b;
						ImGui::ColorConvertHSVtoRGB(hue, 1.0f, 1.0f, r, g, b);
						const ImU32 c = ImGui::GetColorU32(ImVec4(r, g, b, 1.0f));
						dl->AddRectFilled(ImVec2(bb_min.x + x0, bb_min.y), ImVec2(bb_min.x + x1, bb_max.y), c, ImGui::GetStyle().FrameRounding);
					}
				}
				else
				{
					// non-rainbow: green on success, default accent while running
					ImU32 fill_col;
					if (snapshot.state == TaskState::Success)
						fill_col = ImGui::GetColorU32(ImVec4(0.0f, 200.0f / 255.0f, 0.0f, 1.0f));
					else
						fill_col = ImGui::GetColorU32(ImGuiCol_PlotHistogram);

					dl->AddRectFilled(bb_min, ImVec2(bb_min.x + filled_w, bb_max.y), fill_col, ImGui::GetStyle().FrameRounding);
				}
			}

			// render a frame around the bar
			const ImU32 border_u32 = ImGui::GetColorU32(ImGuiCol_Border);
			dl->AddRect(bb_min, bb_max, border_u32, ImGui::GetStyle().FrameRounding, 0, ImGui::GetStyle().FrameBorderSize);
			{
				std::string opTitle;
				if (snapshot.type == TaskType::Update)
				{
					opTitle = (snapshot.state == TaskState::Success) ? "Updating mods (Done)" : "Updating mods";
				}
				else if (snapshot.type == TaskType::Verify)
				{
					opTitle = (snapshot.state == TaskState::Success) ? "Verifying mods (Done)" : "Verifying mods";
				}

				if (!opTitle.empty())
				{
					const ImVec2 textSize = ImGui::CalcTextSize(opTitle.c_str());
					const float text_x = bb_min.x + (avail_x - textSize.x) * 0.5f;
					const float text_y = bb_min.y + (bar_h - textSize.y) * 0.5f;
					const ImU32 text_col = ImGui::GetColorU32(ImGuiCol_Text);
					// subtle shadow for readability over colorful fills
					dl->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(text_x + 1.0f, text_y + 1.0f), ImGui::GetColorU32(ImVec4(0, 0, 0, 0.5f)), opTitle.c_str());
					dl->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(text_x, text_y), text_col, opTitle.c_str());
				}
			}

			// consume layout space
			ImGui::Dummy(ImVec2(avail_x, bar_h));
		}

		if (!snapshot.message.empty())
		{
			ImGui::BeginChild("ModsLog", ImVec2(0, 0), false);
			ImGui::TextWrapped("%s", snapshot.message.c_str());
			ImGui::EndChild();
		}
	}
}