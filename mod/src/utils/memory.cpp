/**
 * @file memory.cpp
 * @brief Реализация утилит памяти
 */

#include "mod/memory.h"
#include "mod/logger.h"
#include <Psapi.h>
#include <sstream>
#include <algorithm>

namespace kastol {

std::vector<std::pair<uint8_t, bool>> Memory::PatternToBytes(const char* pattern) {
    std::vector<std::pair<uint8_t, bool>> bytes;
    std::istringstream stream(pattern);
    std::string byteStr;
    
    while (stream >> byteStr) {
        if (byteStr == "??" || byteStr == "?") {
            bytes.push_back({ 0, false }); // Wildcard
        } else {
            uint8_t byte = static_cast<uint8_t>(std::stoul(byteStr, nullptr, 16));
            bytes.push_back({ byte, true });
        }
    }
    
    return bytes;
}

uintptr_t Memory::FindPattern(uintptr_t moduleBase, size_t moduleSize, const char* pattern) {
    auto bytes = PatternToBytes(pattern);
    if (bytes.empty()) return 0;
    
    uint8_t* start = reinterpret_cast<uint8_t*>(moduleBase);
    uint8_t* end = start + moduleSize - bytes.size();
    
    for (uint8_t* addr = start; addr <= end; addr++) {
        bool found = true;
        for (size_t i = 0; i < bytes.size(); i++) {
            if (bytes[i].second && addr[i] != bytes[i].first) {
                found = false;
                break;
            }
        }
        if (found) {
            return reinterpret_cast<uintptr_t>(addr);
        }
    }
    
    return 0;
}

uintptr_t Memory::FindPattern(const char* moduleName, const char* pattern) {
    uintptr_t moduleBase = GetModuleBase(moduleName);
    size_t moduleSize = GetModuleSize(moduleName);
    
    if (moduleBase == 0 || moduleSize == 0) {
        LOG_ERROR(("Модуль не найден: " + std::string(moduleName)).c_str());
        return 0;
    }
    
    return FindPattern(moduleBase, moduleSize, pattern);
}

bool Memory::Read(uintptr_t address, void* buffer, size_t size) {
    if (address == 0 || buffer == nullptr || size == 0) {
        return false;
    }
    
    SIZE_T bytesRead = 0;
    return ReadProcessMemory(GetCurrentProcess(), 
                             reinterpret_cast<LPCVOID>(address), 
                             buffer, size, &bytesRead) && bytesRead == size;
}

bool Memory::Write(uintptr_t address, const void* data, size_t size) {
    if (address == 0 || data == nullptr || size == 0) {
        return false;
    }
    
    DWORD oldProtect = 0;
    if (!VirtualProtect(reinterpret_cast<LPVOID>(address), size, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        return false;
    }
    
    SIZE_T bytesWritten = 0;
    bool result = WriteProcessMemory(GetCurrentProcess(), 
                                      reinterpret_cast<LPVOID>(address), 
                                      data, size, &bytesWritten) && bytesWritten == size;
    
    VirtualProtect(reinterpret_cast<LPVOID>(address), size, oldProtect, &oldProtect);
    return result;
}

uintptr_t Memory::GetModuleBase(const char* moduleName) {
    HMODULE hModule = GetModuleHandleA(moduleName);
    return reinterpret_cast<uintptr_t>(hModule);
}

size_t Memory::GetModuleSize(const char* moduleName) {
    HMODULE hModule = GetModuleHandleA(moduleName);
    if (!hModule) return 0;
    
    MODULEINFO info = {};
    if (GetModuleInformation(GetCurrentProcess(), hModule, &info, sizeof(info))) {
        return info.SizeOfImage;
    }
    return 0;
}

uintptr_t Memory::ResolveRelativeAddress(uintptr_t address, uint32_t offset, uint32_t instructionSize) {
    int32_t relativeOffset = 0;
    if (!Read(address + offset, &relativeOffset, sizeof(relativeOffset))) {
        return 0;
    }
    return address + offset + relativeOffset + instructionSize;
}

bool Memory::ProtectMemory(uintptr_t address, size_t size, DWORD newProtect, DWORD* oldProtect) {
    return VirtualProtect(reinterpret_cast<LPVOID>(address), size, newProtect, oldProtect) != FALSE;
}

} // namespace kastol
