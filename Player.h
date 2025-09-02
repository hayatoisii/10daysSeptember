#pragma once
#include "AABB.h"
#include "KamataEngine.h"
#include "Platform.h"
#include "WorldTransformEx.h"

using namespace KamataEngine;

class Player {
public:
	// デストラクタ
	~Player();

	// 初期化
	void Initialize(Model* model, Camera* camera, const Vector3& pos);

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	// 足場との当たり判定処理
	void CheckCollisionWithPlatform(const Platform& platform);

	// 重力を取得するメソッド
	float GetGravity() const;

	// 現在位置を取得
	Vector3 GetPosition() const { return worldTransform_.translation_; }

	// 垂直方向の速度を取得
	float GetVelocityY() const { return velocityY_; }

	// プレイヤーの当たり判定のY方向半分サイズを取得
	float GetHalfSizeY() const { return halfSize_.y; }

	// プレイヤーのAABBを取得
	const AABB& GetAABB() const { return aabb_; }

	// 足場に着地したときの処理（Y座標を足場の上端にセットし、速度リセット）
	void LandOnPlatform(float platformY) {
		worldTransform_.translation_.y = platformY;
		velocityY_ = 0.0f;
		isJumping_ = false;
	}

	// プレイヤーの反転状態を取得
	bool IsInversion() const { return inversion; }

private:
	// 足場のスクロール速度（上方向への移動速度）
	float platformScrollSpeed = 0.2f;

	// プレイヤーの横移動速度
	const float speed = 0.5f;

	// 重力加速度（負の値で下方向）
	float gravity = -0.08f;

	// ジャンプの初速度
	const float jumpPower = 1.2f;

	// 垂直方向の速度
	float velocityY_ = 0.0f;

	// ジャンプ中かどうかのフラグ
	bool isJumping_ = false;

	// プレイヤーの上下反転状態フラグ
	bool inversion = false;

	// プレイヤーのモデル
	Model* model_ = nullptr;

	// 入力管理用
	Input* input_ = nullptr;

	// カメラ参照
	Camera* camera_ = nullptr;

	// ワールド変換（位置・回転・スケール）
	WorldTransform worldTransform_;

	// 当たり判定用AABB
	AABB aabb_;

	// プレイヤーの当たり判定サイズ（半分サイズ）
	Vector3 halfSize_{0.5f, 0.5f, 0.5f};
};
