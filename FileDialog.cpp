#include "FileDialog.h"

std::string FileDialog::openFile()
{
    const char* filters[] = { "*.stl" };

    const char* path = tinyfd_openFileDialog(
        "Open STL file",
        "",
        1,
        filters,
        "STL files",
        0);

    if(path)
        return std::string(path);

    return "";
}
