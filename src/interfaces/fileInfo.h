#pragma once
#include <Arduino.h> // pour String
#include <cstddef>   // pour size_t

struct FileInfo
{
    String name;
    String path;
    size_t size;
};
using FileCallback = std::function<void(const FileInfo &fileInfo)>;
