/**
 * FileName: xamldiagnostics.h
 *
 * Copyright (C) 2022-2023 Maplespe、ALTaleX
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
#include "tapsite.h"

namespace MBox
{
	HRESULT InitializeUWPXamlDiagnostics();
	HRESULT InitializeWinUIXamlDiagnostics();

	void EnableXamlDiagnostics(bool uwp);
	void FreeXamlDiagnostics();

	void RunDiagnostics(bool uwp);

	bool ProcessIsWinUI();
}