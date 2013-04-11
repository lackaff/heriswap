/*
	This file is part of Heriswap.

	@author Soupe au Caillou - Pierre-Eric Pelloux-Prayer
	@author Soupe au Caillou - Gautier Pelloux-Prayer

	Heriswap is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, version 3.

	Heriswap is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Heriswap.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "ElitePopupStateManager.h"
#include "../modes/NormalModeManager.h"
#include <sstream>

#include <glm/glm.hpp>

#include <base/PlacementHelper.h>
#include <base/EntityManager.h>

#include "systems/TransformationSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/SoundSystem.h"

#include "DepthLayer.h"

void ElitePopupStateManager::Setup() {
	const Color green("green");
	background = theEntityManager.CreateEntity();
	ADD_COMPONENT(background, Transformation);
	TRANSFORM(background)->size = glm::vec2((float)PlacementHelper::GimpWidthToScreen(712), 
											(float)PlacementHelper::GimpHeightToScreen(450));
	TRANSFORM(background)->z = DL_MainMenuUIBg;
	TransformationSystem::setPosition(TRANSFORM(background), 
									  glm::vec2((float)PlacementHelper::GimpXToScreen(44), 
									  			(float)PlacementHelper::GimpYToScreen(236)), 
									  TransformationSystem::NW);
	ADD_COMPONENT(background, Rendering);
	RENDERING(background)->texture = theRenderingSystem.loadTextureFile("fond_menu_mode");
	RENDERING(background)->color.a = 0.5;

	text = theEntityManager.CreateEntity();
	ADD_COMPONENT(text, Transformation);
	TRANSFORM(text)->position = TRANSFORM(background)->position;
	TRANSFORM(text)->size = TRANSFORM(background)->size;
	TRANSFORM(text)->size.x *= 0.9f;
	TRANSFORM(text)->size.y = (float)PlacementHelper::GimpHeightToScreen(147);
	TransformationSystem::setPosition(TRANSFORM(text), 
									  glm::vec2(0.f, (float)PlacementHelper::GimpYToScreen(236)), 
									  TransformationSystem::N);
	TRANSFORM(text)->z = DL_MainMenuUITxt;
	ADD_COMPONENT(text, TextRendering);
	TEXT_RENDERING(text)->positioning = TextRenderingComponent::LEFT;
	TEXT_RENDERING(text)->color = green;
	TEXT_RENDERING(text)->show = false;
	TEXT_RENDERING(text)->charHeight = PlacementHelper::GimpHeightToScreen(55);
	TEXT_RENDERING(text)->flags |= TextRenderingComponent::MultiLineBit;

	for (int i=0; i<2; i++) {
		eText[i] = theEntityManager.CreateEntity();
		ADD_COMPONENT(eText[i], Transformation);
		ADD_COMPONENT(eText[i], TextRendering);

		TRANSFORM(eText[i])->z = DL_MainMenuUITxt;
		TEXT_RENDERING(eText[i])->show = false;
		TEXT_RENDERING(eText[i])->positioning = TextRenderingComponent::CENTER;
		TEXT_RENDERING(eText[i])->color = green;
		TEXT_RENDERING(eText[i])->charHeight = PlacementHelper::GimpHeightToScreen(75);

	    eButton[i] = theEntityManager.CreateEntity();
	    ADD_COMPONENT(eButton[i], Transformation);
	    TRANSFORM(eButton[i])->size = glm::vec2((float)PlacementHelper::GimpWidthToScreen(708), 
	    										(float)PlacementHelper::GimpHeightToScreen(147));
	    TRANSFORM(eButton[i])->z = DL_MainMenuUIBg;
	    ADD_COMPONENT(eButton[i], Rendering);
	    RENDERING(eButton[i])->texture = theRenderingSystem.loadTextureFile("fond_bouton");
	    RENDERING(eButton[i])->color.a = 0.5;
	    ADD_COMPONENT(eButton[i], Button);
        BUTTON(eButton[i])->enabled = false;

		TRANSFORM(eText[i])->position.x = TRANSFORM(eButton[i])->position.x = 0;
		TRANSFORM(eText[i])->position.y = TRANSFORM(eButton[i])->position.y = PlacementHelper::GimpYToScreen(850+i*183);
	}
	// TODO !
	// TEXT_RENDERING(text)->text = localizeAPI->text("change_difficulty",
	// "You seem really good.\nWould you like to start a new game with increased difficulty ?");
	// TEXT_RENDERING(eText[0])->text = localizeAPI->text("change_difficulty_yes", "Increase difficulty");
	// TEXT_RENDERING(eText[1])->text = localizeAPI->text("change_difficulty_no", "No, keep it easy");
	TEXT_RENDERING(text)->text = localizeAPI->text("change_difficulty");
	TEXT_RENDERING(eText[0])->text = localizeAPI->text("change_difficulty_yes");
	TEXT_RENDERING(eText[1])->text = localizeAPI->text("change_difficulty_no");
}

void ElitePopupStateManager::Enter() {
	RENDERING(background)->show = true;
	TEXT_RENDERING(text)->show = true;
	for (int i=0; i<2; i++) {
		RENDERING(eButton[i])->show = true;
		TEXT_RENDERING(eText[i])->show = true;
		BUTTON(eButton[i])->enabled = true;
	}
}

GameState ElitePopupStateManager::Update(float dt) {
	if (BUTTON(eButton[0])->clicked) {
		LOGW("Change difficulty");
		theGridSystem.setGridFromDifficulty(theGridSystem.nextDifficulty(theGridSystem.sizeToDifficulty()));
		normalGameModeManager->points = 0;
		normalGameModeManager->changeLevel(1);
		return Spawn;
	}
	else if (BUTTON(eButton[1])->clicked)
		return Spawn;
	return ElitePopup;
}

void ElitePopupStateManager::Exit() {
	RENDERING(background)->show = false;
	TEXT_RENDERING(text)->show = false;
	for (int i=0; i<2; i++) {
		RENDERING(eButton[i])->show = false;
        BUTTON(eButton[i])->enabled = false;
		TEXT_RENDERING(eText[i])->show = false;
	}
}
