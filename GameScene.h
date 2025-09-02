#pragma once
#include "KamataEngine.h"
#include "Platform.h"
#include "Player.h"
#include <random>

class GameScene {
public:
	// デストラクタ
	~GameScene();

	// 初期化処理
	void Initialize();

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

private:
	// プラットフォーム生成タイマー（経過時間）
	float platformSpawnTimer = 0.0f;

	// プラットフォーム生成間隔（秒）
	const float platformSpawnInterval = 0.7f;

	// プレイヤーオブジェクトへのポインタ
	Player* player_ = nullptr;

	// プレイヤーモデル
	KamataEngine::Model* modelPlayer_ = nullptr;

	// プラットフォームモデル
	KamataEngine::Model* modelPlatform_ = nullptr;

	// カメラ
	KamataEngine::Camera camera_;

	// ワールド変換（汎用）
	WorldTransform worldTransform;

	// 動的に生成されるプラットフォームのコンテナ
	std::vector<Platform*> platforms_;

	// 乱数生成エンジン（メルセンヌ・ツイスタ）
	std::mt19937 randomEngine_;
};
