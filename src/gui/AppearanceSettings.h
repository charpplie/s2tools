#pragma once

#define S2_GUI
#include "pch.h"

class AppearanceSettings
{
public:
	// Border color settings
	static ImVec4 GetBorderColor();
	static void SetBorderColor(const ImVec4& color);
	
	static bool IsBorderColorEditorRgb();
	static void SetBorderColorEditorRgb(bool isRgb);
	
	static bool IsBorderAnimated();
	static void SetBorderAnimated(bool animated);
	
	static bool IsBorderFlowing();
	static void SetBorderFlowing(bool flowing);
	
	static float GetBorderRgbSpeed();
	static void SetBorderRgbSpeed(float speed);
	
	static int GetBorderSegmentWidth();
	static void SetBorderSegmentWidth(int width);

	// Progress rainbow settings
	static bool IsProgressRainbowEnabled();
	static void SetProgressRainbowEnabled(bool enabled);
	
	static float GetProgressRgbSpeed();
	static void SetProgressRgbSpeed(float speed);
	
	static int GetProgressSegmentWidth();
	static void SetProgressSegmentWidth(int width);

	// Theme
	static bool IsDarkTheme();
	static void SetDarkTheme(bool dark);

	// Apply current border style
	static ImVec4 GetActiveBorderColor();
	static void DrawFlowingBorder(const ImVec2& windowPos, const ImVec2& windowSize);

private:
	static ImVec4 borderColor;
	static bool borderColorEditorRgb;
	static bool borderRgbMode;
	static bool borderRgbFlow;
	static float borderRgbSpeed;
	static int borderRgbSegmentWidth;

	static bool progressRgbMode;
	static float progressRgbSpeed;
	static int progressRgbSegmentWidth;

	static bool darkTheme;
};