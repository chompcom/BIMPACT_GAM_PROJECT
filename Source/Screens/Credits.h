
/*!***************************************************************************
\file       Credits.h
\author		Quah Ming Jun, m.quah
\par        m.quah@digipen.edu
\brief
			This header file declares the lifecycle functions for the Credits Scene.

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
***************************************************************************/


#pragma once

void CreditsLoad();				// Load
void CreditsInit();				// Init
void CreditsUpdate(float dt);	// Update
void CreditsDraw();				// Render
void CreditsFree();				// Free
void CreditsUnload();			// Unload
