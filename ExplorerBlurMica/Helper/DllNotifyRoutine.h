/**
 * FileName: DllNotifyRoutine.h
 *
 * Copyright (C) 2022-2024 Maplespe、ALTaleX
 *
 * This file is part of MToolBox and ExplorerBlurMica.
 * ExplorerBlurMica is free software: you can redistribute it and/or modify it under the terms of the
 * GNU Lesser General Public License as published by the Free Software Foundation, either version 3
 * of the License, or any later version.
 *
 * ExplorerBlurMica is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with Foobar.
 * If not, see <https://www.gnu.org/licenses/lgpl-3.0.html>.
*/
#pragma once
#include "../framework.h"
#include <winternl.h>

namespace MBox
{
	class DllNotifyRoutine
	{
	public:
		static DllNotifyRoutine& GetInstance();
		DllNotifyRoutine();
		~DllNotifyRoutine() noexcept;
		DllNotifyRoutine(const DllNotifyRoutine&) = delete;
		DllNotifyRoutine& operator=(const DllNotifyRoutine&) = delete;

		struct DllInfo
		{
			PCUNICODE_STRING FullDllName;   // The full path name of the DLL module.
			PCUNICODE_STRING BaseDllName;   // The base file name of the DLL module.
			PVOID DllBase;                  // A pointer to the base address for the DLL in memory.
			ULONG SizeOfImage;              // The size of the DLL image, in bytes.
		};
		using Callback = std::function<void(bool load, DllInfo info)>;

		void AddCallback(Callback callback);
		void DeleteCallback(Callback callback);
	private:
		typedef struct _LDR_DLL_LOADED_NOTIFICATION_DATA
		{
			ULONG Flags;                    // Reserved.
			PCUNICODE_STRING FullDllName;   // The full path name of the DLL module.
			PCUNICODE_STRING BaseDllName;   // The base file name of the DLL module.
			PVOID DllBase;                  // A pointer to the base address for the DLL in memory.
			ULONG SizeOfImage;              // The size of the DLL image, in bytes.
		} LDR_DLL_LOADED_NOTIFICATION_DATA, * PLDR_DLL_LOADED_NOTIFICATION_DATA;
		typedef struct _LDR_DLL_UNLOADED_NOTIFICATION_DATA
		{
			ULONG Flags;                    // Reserved.
			PCUNICODE_STRING FullDllName;   // The full path name of the DLL module.
			PCUNICODE_STRING BaseDllName;   // The base file name of the DLL module.
			PVOID DllBase;                  // A pointer to the base address for the DLL in memory.
			ULONG SizeOfImage;              // The size of the DLL image, in bytes.
		} LDR_DLL_UNLOADED_NOTIFICATION_DATA, * PLDR_DLL_UNLOADED_NOTIFICATION_DATA;
		typedef union _LDR_DLL_NOTIFICATION_DATA
		{
			LDR_DLL_LOADED_NOTIFICATION_DATA Loaded;
			LDR_DLL_UNLOADED_NOTIFICATION_DATA Unloaded;
		} LDR_DLL_NOTIFICATION_DATA, * PLDR_DLL_NOTIFICATION_DATA;
		typedef LDR_DLL_NOTIFICATION_DATA* PCLDR_DLL_NOTIFICATION_DATA;
		typedef VOID(CALLBACK* PLDR_DLL_NOTIFICATION_FUNCTION)(
			ULONG NotificationReason,
			PCLDR_DLL_NOTIFICATION_DATA NotificationData,
			PVOID Context
			);
		static VOID CALLBACK LdrDllNotification(
			ULONG NotificationReason,
			PCLDR_DLL_NOTIFICATION_DATA NotificationData,
			PVOID Context
		);
		static constexpr UINT LDR_DLL_NOTIFICATION_REASON_LOADED{ 1 };
		static constexpr UINT LDR_DLL_NOTIFICATION_REASON_UNLOADED{ 2 };

		bool m_internalError{ false };
		PVOID m_cookie{ nullptr };
		NTSTATUS(NTAPI* m_actualLdrRegisterDllNotification)(ULONG, PLDR_DLL_NOTIFICATION_FUNCTION, PVOID, PVOID*);
		NTSTATUS(NTAPI* m_actualLdrUnregisterDllNotification)(PVOID);
		std::vector<Callback> m_callbackList{};
	};
}