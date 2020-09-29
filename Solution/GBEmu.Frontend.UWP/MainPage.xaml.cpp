//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
using namespace Windows::Storage;
using namespace Concurrency;
using namespace Windows::Storage::Pickers;

using namespace GBEmu_Frontend_UWP;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
{
	InitializeComponent();
  emu.set_on_render([]() {

    });

  emu.Initialize(emulation::gb::default_gb_hz);
  OpenRomFile();
 // emu.cartridge()->LoadFile("C:\\Users\\U11111\\Documents\\GitHub\\GBEmu\\test\\games\\Mission Impossible (USA) (En,Fr,Es).gbc", &header);
  //if (emu.cartridge()->cart_loaded() == true)
  //  emu.Run();
}

void MainPage::OpenRomFile() {
  FileOpenPicker ^ openPicker = ref new FileOpenPicker();
  openPicker->ViewMode = PickerViewMode::Thumbnail;
  openPicker->SuggestedStartLocation = PickerLocationId::DocumentsLibrary;
  openPicker->FileTypeFilter->Append(".gb");
  openPicker->FileTypeFilter->Append(".gbc");


  
  create_task(openPicker->PickSingleFileAsync()).then([this](StorageFile^ file) {
    if (file) {
      create_task(file->CopyAsync((Windows::Storage::ApplicationData::Current)->TemporaryFolder, file->Name, Windows::Storage::NameCollisionOption::ReplaceExisting)).then([this](StorageFile^ file2) {
        if (file2) {
          OutputTextBlock->Text = "Copied File to: " + file2->Path;
          auto ls = file2->Path->Data();
          FILE* pFile;
          HRESULT hr = _wfopen_s(&pFile, ls, L"rb");
          //fread_s code was here
          fclose(pFile);
        }
        });
    } else {
      //do some stuff
    }
    });

  /*if (file != null) {
    // Application now has read/write access to the picked file
    OutputTextBlock.Text = "Picked photo: " + file.Name;
  } else {
    OutputTextBlock.Text = "Operation cancelled.";
  }*/
}