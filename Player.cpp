#include "Player.h"
#include <cassert>
#define NOMINMAX
#include <algorithm>

Player::~Player() {}

void Player::Initialize(Model* model, Camera* camera, const Vector3& pos) {
	assert(model);
	model_ = model;
	camera_ = camera;
	worldTransform_.translation_ = pos;
	input_ = KamataEngine::Input::GetInstance();
	worldTransform_.Initialize();

	// 初期AABB（プレイヤーの当たり判定用ボックス）を設定
	aabb_.Set(pos - halfSize_, pos + halfSize_);
}

float Player::GetGravity() const { return gravity; }

void Player::Update() {
	// 左右移動
	if (input_->PushKey(DIK_A)) {
		worldTransform_.translation_.x -= speed;
	}
	if (input_->PushKey(DIK_D)) {
		worldTransform_.translation_.x += speed;
	}

	// ジャンプ入力（反転状態によりジャンプ方向を切り替える）
	if (!isJumping_ && input_->TriggerKey(DIK_SPACE)) {
		velocityY_ = inversion ? -jumpPower : jumpPower; // inversion=trueなら上方向にジャンプ
		isJumping_ = true;
	}

	// 重力を適用
	velocityY_ += gravity;
	worldTransform_.translation_.y += velocityY_;

	// 地面判定と反転処理
	if (!inversion) {
		// 通常状態（地面はy = -20）
		if (worldTransform_.translation_.y <= -20.0f) {
			worldTransform_.translation_.y = -20.0f;
			velocityY_ = 0.0f;
			isJumping_ = false;
			inversion = true; // 反転状態へ

			gravity = -gravity;                               // 重力を反転
			platformScrollSpeed = -fabs(platformScrollSpeed); // 足場のスクロール速度も反転
		}
	} else {
		// 反転状態（地面はy = 18）
		if (worldTransform_.translation_.y >= 18.0f) {
			worldTransform_.translation_.y = 18.0f;
			velocityY_ = 0.0f;
			isJumping_ = false;
			inversion = false; // 通常状態へ

			gravity = -gravity;                              // 重力を元に戻す
			platformScrollSpeed = fabs(platformScrollSpeed); // 足場のスクロール速度も元に戻す
		}
	}

	// AABBを現在の位置に合わせて更新
	Vector3 pos = worldTransform_.translation_;
	aabb_.Set(pos - halfSize_, pos + halfSize_);

	worldTransform_.UpdateMatarix();
}

void Player::Draw() {
	if (model_) {
		model_->Draw(worldTransform_, *camera_);
	}
}

void Player::CheckCollisionWithPlatform(const Platform& platform) {
	const AABB& platformAABB = platform.GetAABB();

	if (!aabb_.IsColliding(platformAABB))
		return;

	// プレイヤーと足場のそれぞれの最小・最大座標を取得
	Vector3 playerMin = aabb_.GetMin();
	Vector3 playerMax = aabb_.GetMax();
	Vector3 platformMin = platformAABB.GetMin();
	Vector3 platformMax = platformAABB.GetMax();

	// 衝突量をX軸・Y軸方向で計算
	float overlapLeft = playerMax.x - platformMin.x;
	float overlapRight = platformMax.x - playerMin.x;
	float overlapBottom = playerMax.y - platformMin.y;
	float overlapTop = platformMax.y - playerMin.y;

	// X軸・Y軸どちらの衝突が浅いか判定し、補正方向を決定
	float minOverlapX = std::fmin(overlapLeft, overlapRight);
	float minOverlapY = std::fmin(overlapBottom, overlapTop);

	if (minOverlapX < minOverlapY) {
		// 横方向の補正（X軸方向のめり込み回避）
		if (overlapLeft < overlapRight) {
			worldTransform_.translation_.x -= overlapLeft;
		} else {
			worldTransform_.translation_.x += overlapRight;
		}
	} else {
		// 縦方向の補正（Y軸方向のめり込み回避）

		// 重力反転状態とY速度を考慮し着地・天井判定を行う
		if ((!inversion && velocityY_ <= 0.0f && playerMax.y > platformMax.y) || (inversion && velocityY_ >= 0.0f && playerMin.y < platformMin.y)) {
			// 上から着地（通常時は下方向移動中、反転時は上方向移動中）
			worldTransform_.translation_.y = platformMax.y + halfSize_.y;
			velocityY_ = 0.0f;
			isJumping_ = false;
		} else if ((!inversion && velocityY_ > 0.0f && playerMin.y < platformMin.y) || (inversion && velocityY_ < 0.0f && playerMax.y > platformMax.y)) {
			// 下からぶつかる（通常時は上方向移動中、反転時は下方向移動中）
			worldTransform_.translation_.y = platformMin.y - halfSize_.y;
			velocityY_ = 0.0f;
			// isJumping_は維持（天井にぶつかっただけ）
		}
	}

	// AABB再計算
	aabb_.Set(worldTransform_.translation_ - halfSize_, worldTransform_.translation_ + halfSize_);
}