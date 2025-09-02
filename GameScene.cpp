#include "GameScene.h"
#include <random>

GameScene::~GameScene() {
	// 動的確保したオブジェクトの解放
	delete player_;
	for (auto platform : platforms_) {
		delete platform;
	}
}

void GameScene::Initialize() {
	// 乱数エンジンの初期化
	std::random_device rd;
	randomEngine_ = std::mt19937(rd());

	// モデルの読み込み
	modelPlayer_ = KamataEngine::Model::CreateFromOBJ("cube", true);
	modelPlatform_ = KamataEngine::Model::CreateFromOBJ("platform", true);

	// カメラ初期化
	camera_.Initialize();

	// プレイヤー初期化
	Vector3 playerPos = {0, 0, 0};
	player_ = new Player();
	player_->Initialize(modelPlayer_, &camera_, playerPos);

	// 足場をランダムに初期生成
	std::uniform_real_distribution<float> posX(-20.0f, 20.0f);
	std::uniform_real_distribution<float> posY(-10.0f, 40.0f);

	const int platformCount = 10; // 足場の数
	for (int i = 0; i < platformCount; i++) {
		Vector3 pos = {posX(randomEngine_), posY(randomEngine_), 0.0f};
		Vector3 scale = {1.5f, 1.2f, 1.0f};

		Platform* platform = new Platform();
		platform->Initialize(pos, scale, modelPlatform_, &camera_);
		platforms_.push_back(platform);
	}

	// ワールドトランスフォームの初期化
	worldTransform.Initialize();
}

void GameScene::Update() {
	// 足場の生成タイミング管理
	platformSpawnTimer += 1.0f / 60.0f; // 60FPS想定
	if (platformSpawnTimer >= platformSpawnInterval) {
		platformSpawnTimer = 0.0f;

		// 新しい足場のX座標を乱数で決定
		std::uniform_real_distribution<float> posX(-15.0f, 15.0f);

		// プレイヤーの反転状態によって足場の出現Y座標を切り替え
		Vector3 pos;
		if (player_->IsInversion()) {
			pos = {posX(randomEngine_), 21.0f, 0.0f}; // 反転中は画面上部付近に生成
		} else {
			pos = {posX(randomEngine_), -20.0f, 0.0f}; // 通常は画面下部付近に生成
		}

		Vector3 scale = {1.5f, 1.2f, 1.0f};
		Platform* platform = new Platform();
		platform->Initialize(pos, scale, modelPlatform_, &camera_);
		platforms_.push_back(platform);
	}

	// プレイヤーの重力を取得し、足場のスクロール速度を設定
	float gravity = player_->GetGravity();
	float scrollSpeed = (gravity > 0.0f) ? -0.1f : 0.1f;

	// 足場にスクロール速度をセットし、更新処理を呼ぶ
	for (auto platform : platforms_) {
		platform->SetScrollSpeed(scrollSpeed);
		platform->Update();
	}

// 画面外の足場を削除（反転状態で判定を切り替え）
	for (auto it = platforms_.begin(); it != platforms_.end();) {
		Vector3 pos = (*it)->GetWorldPosition();

		if (player_->IsInversion()) {
			// 反転中は上端を超えたら削除
			if (pos.y > 22.0f) {
				delete *it;
				it = platforms_.erase(it);
				continue;
			}
		} else {
			// 通常は下端を下回ったら削除
			if (pos.y < -22.0f) {
				delete *it;
				it = platforms_.erase(it);
				continue;
			}
		}
		++it;
	}

	// プレイヤーの当たり判定と物理計算
	for (auto platform : platforms_) {
		const AABB& platformAABB = platform->GetAABB();

		// プレイヤーと足場が衝突したら着地判定
		if (player_->GetAABB().IsColliding(platformAABB)) {
			if (player_->GetVelocityY() <= 0.0f) {
				player_->LandOnPlatform(platformAABB.GetMax().y + player_->GetHalfSizeY());
			} else {
				player_->LandOnPlatform(platformAABB.GetMin().y - player_->GetHalfSizeY());
			}
			break; // １回着地したら判定終了
		}
	}

	// プレイヤーの更新処理
	player_->Update();
}

void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 2D描画前準備
	Sprite::PreDraw(dxCommon->GetCommandList());
	// 2D描画（必要ならここに描画処理を書く）
	Sprite::PostDraw();

	// 深度バッファクリア
	dxCommon->ClearDepthBuffer();

	// 3D描画前準備
	Model::PreDraw(dxCommon->GetCommandList());

	// 足場を描画
	for (auto platform : platforms_) {
		platform->Draw();
	}
	// プレイヤーを描画
	player_->Draw();

	// 3D描画終了処理
	Model::PostDraw();
}