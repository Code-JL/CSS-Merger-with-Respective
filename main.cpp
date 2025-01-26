#include <iostream>
#include <Windows.h>
#include <shobjidl.h> 
#include "CSSParser.h"

std::string OpenFileDialog() {
    std::string filename;
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr)) {
        IFileOpenDialog* pFileOpen;
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
            IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

        if (SUCCEEDED(hr)) {
            // Get current directory
            wchar_t buffer[MAX_PATH];
            GetCurrentDirectory(MAX_PATH, buffer);

            // Create shell item for current directory
            IShellItem* pItem;
            hr = SHCreateItemFromParsingName(buffer, NULL, IID_PPV_ARGS(&pItem));
            if (SUCCEEDED(hr)) {
                pFileOpen->SetFolder(pItem);
                pItem->Release();
            }

            // Set file type filter
            COMDLG_FILTERSPEC fileTypes[] = {
                { L"CSS Files", L"*.css" },
                { L"All Files", L"*.*" }
            };
            pFileOpen->SetFileTypes(2, fileTypes);

            hr = pFileOpen->Show(NULL);
            if (SUCCEEDED(hr)) {
                IShellItem* pResult;
                hr = pFileOpen->GetResult(&pResult);
                if (SUCCEEDED(hr)) {
                    PWSTR filePath;
                    hr = pResult->GetDisplayName(SIGDN_FILESYSPATH, &filePath);
                    if (SUCCEEDED(hr)) {
                        std::wstring ws(filePath);
                        filename = std::string(ws.begin(), ws.end());
                        CoTaskMemFree(filePath);
                    }
                    pResult->Release();
                }
            }
            pFileOpen->Release();
        }
        CoUninitialize();
    }
    return filename;
}

std::string SaveFileDialog() {
    std::string filename;
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr)) {
        IFileSaveDialog* pFileSave;
        hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL,
            IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

        if (SUCCEEDED(hr)) {
            // Get current directory
            wchar_t buffer[MAX_PATH];
            GetCurrentDirectory(MAX_PATH, buffer);

            // Create shell item for current directory
            IShellItem* pItem;
            hr = SHCreateItemFromParsingName(buffer, NULL, IID_PPV_ARGS(&pItem));
            if (SUCCEEDED(hr)) {
                pFileSave->SetFolder(pItem);
                pItem->Release();
            }

            // Set default file name and extension
            pFileSave->SetDefaultExtension(L"css");
            pFileSave->SetFileName(L"merged.css");

            // Set file type filter
            COMDLG_FILTERSPEC fileTypes[] = {
                { L"CSS Files", L"*.css" },
                { L"All Files", L"*.*" }
            };
            pFileSave->SetFileTypes(2, fileTypes);

            hr = pFileSave->Show(NULL);
            if (SUCCEEDED(hr)) {
                IShellItem* pResult;
                hr = pFileSave->GetResult(&pResult);
                if (SUCCEEDED(hr)) {
                    PWSTR filePath;
                    hr = pResult->GetDisplayName(SIGDN_FILESYSPATH, &filePath);
                    if (SUCCEEDED(hr)) {
                        std::wstring ws(filePath);
                        filename = std::string(ws.begin(), ws.end());
                        CoTaskMemFree(filePath);
                    }
                    pResult->Release();
                }
            }
            pFileSave->Release();
        }
        CoUninitialize();
    }
    return filename;
}


int main() {
    std::cout << "CSS Merger Tool\n";
    std::cout << "Please select the first CSS file...\n";

    std::string file1 = OpenFileDialog();
    if (file1.empty()) {
        std::cout << "No file selected. Exiting...\n";
        return 1;
    }

    std::cout << "Please select the second CSS file...\n";
    std::string file2 = OpenFileDialog();
    if (file2.empty()) {
        std::cout << "No file selected. Exiting...\n";
        return 1;
    }

    std::cout << "Select where to save the merged CSS file...\n";
    std::string outputFile = SaveFileDialog();
    if (outputFile.empty()) {
        std::cout << "No output location selected. Exiting...\n";
        return 1;
    }

    try {
        // Parse input files
        auto rules1 = CSSParser::parseFile(file1);
        auto rules2 = CSSParser::parseFile(file2);

        // Merge rules (simple version - rules2 takes precedence)
        auto mergedRules = rules1;
        for (const auto& rule : rules2) {
            mergedRules[rule.first] = rule.second;
        }

        // Write to output file
        CSSParser::writeToFile(outputFile, mergedRules);

        std::cout << "Successfully parsed " << rules1.size() << " rules from first file\n";
        std::cout << "Successfully parsed " << rules2.size() << " rules from second file\n";
        std::cout << "Merged CSS has been saved to: " << outputFile << "\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

