#include "LevelStateManager.h"

#include <sstream>

#include <base/MathUtil.h>
#include <base/PlacementHelper.h>

#include "systems/ParticuleSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/MusicSystem.h"
#include "systems/MorphingSystem.h"
#include "systems/ADSRSystem.h"

#include "modes/GameModeManager.h"
#include "TwitchSystem.h"
#include "DepthLayer.h"
#include "CombinationMark.h"
#include "GridSystem.h"
#include "Game.h"

void LevelStateManager::Setup() {
	eGrid = theEntityManager.CreateEntity();
	ADD_COMPONENT(eGrid, ADSR);

	ADSR(eGrid)->idleValue = 0;
	ADSR(eGrid)->attackValue = 1;
	ADSR(eGrid)->attackTiming = 3;
	ADSR(eGrid)->decayTiming = 0.;
	ADSR(eGrid)->sustainValue = 1;
	ADSR(eGrid)->releaseTiming = 0;
	ADSR(eGrid)->active = false;

	eBigLevel = theEntityManager.CreateEntity();
	ADD_COMPONENT(eBigLevel, Transformation);
	TRANSFORM(eBigLevel)->position = Vector2(0, PlacementHelper::GimpYToScreen(846));
	TRANSFORM(eBigLevel)->z = DL_Score;
	ADD_COMPONENT(eBigLevel, TextRendering);
	TEXT_RENDERING(eBigLevel)->color = Color(1, 1, 1);
	TEXT_RENDERING(eBigLevel)->fontName = "gdtypo";
	TEXT_RENDERING(eBigLevel)->charHeight = PlacementHelper::GimpHeightToScreen(288);
	TEXT_RENDERING(eBigLevel)->positioning = TextRenderingComponent::CENTER;
	TEXT_RENDERING(eBigLevel)->isANumber = true;
	ADD_COMPONENT(eBigLevel, Music);
    ADD_COMPONENT(eBigLevel, Morphing);
	MUSIC(eBigLevel)->control = MusicComponent::Stop;

	eSnowEmitter = theEntityManager.CreateEntity();
	ADD_COMPONENT(eSnowEmitter, Transformation);
	TRANSFORM(eSnowEmitter)->size = Vector2(PlacementHelper::GimpWidthToScreen(800), 0.5);
	TransformationSystem::setPosition(TRANSFORM(eSnowEmitter), Vector2(0, PlacementHelper::GimpYToScreen(0)), TransformationSystem::S);
	TRANSFORM(eSnowEmitter)->z = DL_Snow;
	ADD_COMPONENT(eSnowEmitter, Particule);
	PARTICULE(eSnowEmitter)->emissionRate = 0;
	PARTICULE(eSnowEmitter)->texture = theRenderingSystem.loadTextureFile("snow_flake0.png");
	PARTICULE(eSnowEmitter)->lifetime = Interval<float>(5.0f, 6.5f);
	PARTICULE(eSnowEmitter)->initialColor = Interval<Color> (Color(1.0, 1.0, 1.0, 1.0), Color(1.0, 1.0, 1.0, 1.0));
	PARTICULE(eSnowEmitter)->finalColor  = Interval<Color> (Color(1.0, 1.0, 1.0, 1.0), Color(1.0, 1.0, 1.0, 1.0));
	PARTICULE(eSnowEmitter)->initialSize = Interval<float>(PlacementHelper::GimpWidthToScreen(30), PlacementHelper::GimpWidthToScreen(40));
	PARTICULE(eSnowEmitter)->finalSize = Interval<float>(PlacementHelper::GimpWidthToScreen(30), PlacementHelper::GimpWidthToScreen(40));
	PARTICULE(eSnowEmitter)->forceDirection = Interval<float>(0, 0);
	PARTICULE(eSnowEmitter)->forceAmplitude  = Interval<float>(0, 0);
	PARTICULE(eSnowEmitter)->mass = 1;
	
	eSnowBranch = theEntityManager.CreateEntity();
	ADD_COMPONENT(eSnowBranch, Transformation);
	TRANSFORM(eSnowBranch)->size = Vector2(PlacementHelper::GimpWidthToScreen(800), PlacementHelper::GimpHeightToScreen(218));
	TransformationSystem::setPosition(TRANSFORM(eSnowBranch), Vector2(PlacementHelper::GimpXToScreen(0), PlacementHelper::GimpYToScreen(0)), TransformationSystem::NW);
	TRANSFORM(eSnowBranch)->z = DL_SnowBackground;
	ADD_COMPONENT(eSnowBranch, Rendering);
	RENDERING(eSnowBranch)->texture = theRenderingSystem.loadTextureFile("snow_branch.png");

	eSnowGround = theEntityManager.CreateEntity();
	ADD_COMPONENT(eSnowGround, Transformation);
	TRANSFORM(eSnowGround)->size = Vector2(PlacementHelper::GimpWidthToScreen(800), PlacementHelper::GimpHeightToScreen(300));
	TransformationSystem::setPosition(TRANSFORM(eSnowGround), Vector2(PlacementHelper::GimpXToScreen(0), PlacementHelper::GimpYToScreen(1280)), TransformationSystem::SW);
	TRANSFORM(eSnowGround)->z = DL_SnowBackground;
	ADD_COMPONENT(eSnowGround, Rendering);
	RENDERING(eSnowGround)->texture = theRenderingSystem.loadTextureFile("snow_ground.png");
}

void LevelStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);
	std::stringstream a;
	a << currentLevel;
	TEXT_RENDERING(eBigLevel)->text = a.str();
	TEXT_RENDERING(eBigLevel)->hide = false;
	PARTICULE(eSnowEmitter)->emissionRate = 50;
	RENDERING(eSnowBranch)->hide = false;
	RENDERING(eSnowGround)->hide = false;
	MUSIC(eBigLevel)->music = theMusicSystem.loadMusicFile("audio/level_up.ogg");
	MUSIC(eBigLevel)->control = MusicComponent::Start;

	MORPHING(eBigLevel)->timing = 1;
	MORPHING(eBigLevel)->elements.push_back(new TypedMorphElement<float> (&TEXT_RENDERING(eBigLevel)->color.a, 0, 1));
	MORPHING(eBigLevel)->elements.push_back(new TypedMorphElement<float> (&TEXT_RENDERING(smallLevel)->color.a, 1, 0));
	MORPHING(eBigLevel)->elements.push_back(new TypedMorphElement<float> (&RENDERING(eSnowGround)->color.a, 0, 1));
	MORPHING(eBigLevel)->active = true;
	
	TRANSFORM(eBigLevel)->position = Vector2(0, PlacementHelper::GimpYToScreen(846));
	TEXT_RENDERING(eBigLevel)->charHeight = PlacementHelper::GimpHeightToScreen(288);
	TEXT_RENDERING(eBigLevel)->hide = false;
	
	duration = 0;
	
	// desaturate everyone except the branch, mute, pause and text elements
	TextureRef branch = theRenderingSystem.loadTextureFile("branche.png");
	TextureRef pause = theRenderingSystem.loadTextureFile("pause.png");
	TextureRef sound1 = theRenderingSystem.loadTextureFile("sound_on.png");
	TextureRef sound2 = theRenderingSystem.loadTextureFile("sound_off.png");
	std::vector<Entity> text = theTextRenderingSystem.RetrieveAllEntityWithComponent();
	std::vector<Entity> entities = theRenderingSystem.RetrieveAllEntityWithComponent();
	for (int i=0; i<entities.size(); i++) {
		TransformationComponent* tc = TRANSFORM(entities[i]);
		if (tc->parent <= 0 || std::find(text.begin(), text.end(), tc->parent) == text.end()) {
			RenderingComponent* rc = RENDERING(entities[i]);
			if (rc->texture == branch || rc->texture == pause || rc->texture == sound1 || rc->texture == sound2) {
				continue;
			}
			rc->desaturate = true;
		}
	}
	
	entities = theGridSystem.RetrieveAllEntityWithComponent();
	for (int i=0; i<entities.size(); i++) {
		CombinationMark::markCellInCombination(entities[i]);
	}
	
	newLeavesGenerated = false;
}

GameState LevelStateManager::Update(float dt) {
	duration += dt;

	if (duration > 0.15) {
		ADSR(eGrid)->active = true;
		float alpha = 1 - ADSR(eGrid)->value;
		std::vector<Entity> entities = theGridSystem.RetrieveAllEntityWithComponent();
		for (std::vector<Entity>::iterator it = entities.begin(); it != entities.end(); ++it ) {
			RENDERING(*it)->color.a = alpha;
			TWITCH(*it)->speed = alpha * 9;
		}
	}
	
	if (duration > 6) {
		MorphingComponent* mc = MORPHING(eBigLevel);
		for (int i=0; i<mc->elements.size(); i++) {
			delete mc->elements[i];
		}
		mc->elements.clear();
		// move big score to small score
		mc->elements.push_back(new TypedMorphElement<float> (&TEXT_RENDERING(eBigLevel)->charHeight, TEXT_RENDERING(eBigLevel)->charHeight, TEXT_RENDERING(smallLevel)->charHeight));
		mc->elements.push_back(new TypedMorphElement<Vector2> (&TRANSFORM(eBigLevel)->position, TRANSFORM(eBigLevel)->position, TRANSFORM(smallLevel)->position));
		mc->active = true;
		mc->activationTime = 0;
		mc->timing = 0.5;
		
		PARTICULE(eSnowEmitter)->emissionRate = 0;
		
		if (!newLeavesGenerated) {
			newLeavesGenerated = true;
			modeMgr->generateLeaves(0, theGridSystem.Types);
		}
	}
	if (newLeavesGenerated) {
		for (int i=0; i<modeMgr->branchLeaves.size(); i++) {
			TRANSFORM(modeMgr->branchLeaves[i].e)->size = Game::CellSize(8) * Game::CellContentScale() * MathUtil::Min((duration-6) / 4.f, 1.f);
			if (duration < 10) 
				RENDERING(modeMgr->branchLeaves[i].e)->desaturate = true;
			else
				RENDERING(modeMgr->branchLeaves[i].e)->desaturate = false;
		}
	}
	
	
	if (duration > 10) {
		return Spawn;
	}

	return LevelChanged;
}

void LevelStateManager::Exit() {
	theGridSystem.DeleteAll();
	ADSR(eGrid)->active = false;
	feuilles.clear();
	LOGI("%s", __PRETTY_FUNCTION__);
	TEXT_RENDERING(eBigLevel)->hide = true;
	PARTICULE(eSnowEmitter)->emissionRate = 0;
	// RENDERING(eDesaturate)->hide = false;
	RENDERING(eSnowBranch)->hide = true;
	RENDERING(eSnowGround)->hide = true;
	
	MorphingComponent* mc = MORPHING(eBigLevel);
	for (int i=0; i<mc->elements.size(); i++) {
		delete mc->elements[i];
	}
	mc->elements.clear();
	// hide big level
	TEXT_RENDERING(eBigLevel)->hide = true;
	// show small level
	TEXT_RENDERING(smallLevel)->color.a = 1;
	
	std::vector<Entity> ent = theRenderingSystem.RetrieveAllEntityWithComponent();
	for (int i=0; i<ent.size(); i++) {
		RENDERING(ent[i])->desaturate = false;
	}
}
