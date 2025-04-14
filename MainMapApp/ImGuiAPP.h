#pragma once



class ImGuiApp
{
	protected:
		char MainWindowsTitle[100];
		ImGuiWindowFlags mainWinFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus;// | ImGuiWindowFlags_NoSavedSettings);
public:
	virtual void InitializeApp() {};
	virtual void ShutDownApp() {};
	virtual void UpdateApp() {};


};

