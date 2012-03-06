#pragma once
#include "GridSystem.h"

class ScoreStorage;

class Game {
	public:

		void init(ScoreStorage* score, int windowW, int windowH, const uint8_t* in = 0, int size = 0);
		void tick(float dt);
		void togglePause(bool activate);
		void toggleShowCombi(bool forcedesactivate);

		static Vector2 GridCoordsToPosition(int i, int j);
		static float CellSize();
		static float CellContentScale();

		int saveState(uint8_t** out);
	private:
		void loadState(const uint8_t* in, int size);

	class Data;
	Data* datas;
};
