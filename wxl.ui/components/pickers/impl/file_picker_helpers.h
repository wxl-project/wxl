#pragma once

// What stands behind fileType and fileTypeChoice, the types of files a storage picker offers.
//
// Private: the picker arrives as the projection type the wrapper already holds, so this header is
// one only wxl's own sources ever include.

#include <winrt/Microsoft.Windows.Storage.Pickers.h>

#include "FileTypeChoice.h"
#include "hstring_param.h"

namespace wxl::impl {

/// `fileType = u".txt"` on an open picker: one entry appended to the picker's own list.
void add_file_type(winrt::Microsoft::Windows::Storage::Pickers::FileOpenPicker const& picker,
                   hstring_param const& extension);

/// `fileTypeChoice = ...` on a save picker: one entry appended to the picker's own list.
void add_file_type_choice(winrt::Microsoft::Windows::Storage::Pickers::FileSavePicker const& picker,
                          FileTypeChoice const& choice);

}  // namespace wxl::impl
