#include "AppearanceSettings.h"
#include <algorithm>
#include <cmath>

ImVec4 AppearanceSettings::borderColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
bool AppearanceSettings::borderColorEditorRgb = true;
bool AppearanceSettings::borderRgbMode = false;
bool AppearanceSettings::borderRgbFlow = false;
float AppearanceSettings::borderRgbSpeed = 0.1f;
int AppearanceSettings::borderRgbSegmentWidth = 4;

bool AppearanceSettings::progressRgbMode = false;
float AppearanceSettings::progressRgbSpeed = 0.25f;
int AppearanceSettings::progressRgbSegmentWidth = 4;

// Default theme: dark
bool AppearanceSettings::darkTheme = true;

ImVec4 AppearanceSettings::GetBorderColor()
{
	return borderColor;
}

void AppearanceSettings::SetBorderColor(const ImVec4& color)
{
	borderColor = color;
}

bool AppearanceSettings::IsBorderColorEditorRgb()
{
	return borderColorEditorRgb;
}

void AppearanceSettings::SetBorderColorEditorRgb(bool isRgb)
{
	borderColorEditorRgb = isRgb;
}

bool AppearanceSettings::IsBorderAnimated()
{
	return borderRgbMode;
}

void AppearanceSettings::SetBorderAnimated(bool animated)
{
	borderRgbMode = animated;
}

bool AppearanceSettings::IsBorderFlowing()
{
	return borderRgbFlow;
}

void AppearanceSettings::SetBorderFlowing(bool flowing)
{
	borderRgbFlow = flowing;
}

float AppearanceSettings::GetBorderRgbSpeed()
{
	return borderRgbSpeed;
}

void AppearanceSettings::SetBorderRgbSpeed(float speed)
{
	borderRgbSpeed = speed;
}

int AppearanceSettings::GetBorderSegmentWidth()
{
	return borderRgbSegmentWidth;
}

void AppearanceSettings::SetBorderSegmentWidth(int width)
{
	borderRgbSegmentWidth = width;
}

bool AppearanceSettings::IsProgressRainbowEnabled()
{
	return progressRgbMode;
}

void AppearanceSettings::SetProgressRainbowEnabled(bool enabled)
{
	progressRgbMode = enabled;
}

float AppearanceSettings::GetProgressRgbSpeed()
{
	return progressRgbSpeed;
}

void AppearanceSettings::SetProgressRgbSpeed(float speed)
{
	progressRgbSpeed = speed;
}

int AppearanceSettings::GetProgressSegmentWidth()
{
	return progressRgbSegmentWidth;
}

void AppearanceSettings::SetProgressSegmentWidth(int width)
{
	progressRgbSegmentWidth = width;
}

bool AppearanceSettings::IsDarkTheme()
{
	return darkTheme;
}

void AppearanceSettings::SetDarkTheme(bool dark)
{
	darkTheme = dark;
}

ImVec4 AppearanceSettings::GetActiveBorderColor()
{
	ImVec4 activeBorder = borderColor;
	
	if (borderRgbMode && !borderRgbFlow)
	{
		// Single-color hue cycle: compute RGB from HSV(hue(t), 1, 1) and preserve alpha
		const float hue = std::fmod(static_cast<float>(ImGui::GetTime()) * borderRgbSpeed, 1.0f);
		float r, g, b;
		ImGui::ColorConvertHSVtoRGB(hue, 1.0f, 1.0f, r, g, b);
		activeBorder.x = r;
		activeBorder.y = g;
		activeBorder.z = b;
		activeBorder.w = borderColor.w;
	}
	
	return activeBorder;
}

void AppearanceSettings::DrawFlowingBorder(const ImVec2& windowPos, const ImVec2& windowSize)
{
	if (!borderRgbMode || !borderRgbFlow)
		return;

	ImDrawList* dl = ImGui::GetWindowDrawList();
	const float thickness = (std::max)(1.0f, ImGui::GetStyle().FrameBorderSize * 2.0f);
	const float segW = static_cast<float>((std::max)(1, borderRgbSegmentWidth));
	const float baseHue = std::fmod(static_cast<float>(ImGui::GetTime()) * borderRgbSpeed, 1.0f);

	const float w = windowSize.x;
	const float h = windowSize.y;
	const float topLen = w;
	const float rightLen = h;
	const float bottomLen = w;
	const float leftLen = h;
	const float perimeter = topLen + rightLen + bottomLen + leftLen;

	auto drawSideSegments = [&](float startX, float startY, float len, bool horizontal, float offset)
	{
		const int steps = (std::max)(1, static_cast<int>(std::ceil(len / segW)));
		for (int i = 0; i < steps; ++i)
		{
			const float segStart = (std::min)(len, i * segW);
			const float segEnd = (std::min)(len, segStart + segW);
			if (segEnd <= segStart) continue;
			
			const float t = (offset + segStart) / perimeter;
			float hue = baseHue + t;
			hue = hue - std::floor(hue);
			float r, g, b;
			ImGui::ColorConvertHSVtoRGB(hue, 1.0f, 1.0f, r, g, b);
			const ImU32 c = ImGui::GetColorU32(ImVec4(r, g, b, borderColor.w));

			if (horizontal)
			{
				const float x0 = windowPos.x + startX + segStart;
				const float x1 = windowPos.x + startX + segEnd;
				const float y0 = windowPos.y + startY;
				const float y1 = windowPos.y + startY + thickness;
				dl->AddRectFilled(ImVec2(x0, y0), ImVec2(x1, y1), c, 0.0f);
			}
			else
			{
				const float x0 = windowPos.x + startX;
				const float x1 = windowPos.x + startX + thickness;
				const float y0 = windowPos.y + startY + segStart;
				const float y1 = windowPos.y + startY + segEnd;
				dl->AddRectFilled(ImVec2(x0, y0), ImVec2(x1, y1), c, 0.0f);
			}
		}
	};

	drawSideSegments(0.0f, -thickness, topLen, true, 0.0f);
	drawSideSegments(w, 0.0f, rightLen, false, topLen);
	drawSideSegments(0.0f, h, bottomLen, true, topLen + rightLen);
	drawSideSegments(-thickness, 0.0f, leftLen, false, topLen + rightLen + bottomLen);
}