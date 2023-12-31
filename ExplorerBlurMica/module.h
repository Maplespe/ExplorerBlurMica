#pragma once
#include "framework.h"

namespace MBox
{
	// {887D3A6A-502E-4AF5-9CE6-D515E12AFE89}
	static constexpr CLSID CLSID_FolderExtension = { 0x887d3a6a, 0x502e, 0x4af5, { 0x9c, 0xe6, 0xd5, 0x15, 0xe1, 0x2a, 0xfe, 0x89 } };
	static constexpr auto CLSID_FolderExtension_STR = L"{887D3A6A-502E-4AF5-9CE6-D515E12AFE89}";
	static constexpr std::wstring_view regPath = LR"(SOFTWARE\Classes\Drive\shellex\FolderExtensions\{887D3A6A-502E-4AF5-9CE6-D515E12AFE89})";

	BOOL OnDllMain(HMODULE hModule, DWORD ul_reason_for_call);
}