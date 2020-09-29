//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"

namespace GBEmu_Frontend_UWP
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();
		void OpenRomFile();
	private:
		emulation::gb::Emu emu;
		emulation::gb::CartridgeHeader header;
	};
}
