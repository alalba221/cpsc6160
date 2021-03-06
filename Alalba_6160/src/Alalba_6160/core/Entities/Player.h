#pragma once

#include "Entity.h"
#include "Alalba_6160/core/Events/Event.h"
#include "Alalba_6160/core/Components.h"
namespace Alalba {

	enum class ANIMATION_STATE {
		BLINKING,
		RUNNING,
		STANDING,
		DRIFTING,
		DUCKING,
		JUMPING
	};
  
	const int SMALL_MARIO_COLLIDER_HEIGHT = TILE_SIZE - 2;
	const int SUPER_MARIO_COLLIDER_HEIGHT = (TILE_SIZE * 2) - 4;
	constexpr int RUNNING_ANIMATION_SPEED = 4;
	
	class Player : public Entity
	{
	public:
		Player() = default;
		Player(entt::entity handle, Scene* scene);
		Player(const Player& other) = default;
	private:
		int dirX = 0;
		bool jump = false;
		bool duck = false;
		bool sprint = false;
		int left = 0;
		int right = 0;
		int lookingLeft = 0;
		ANIMATION_STATE currentState = ANIMATION_STATE::STANDING;
		CameraComponent* m_Camera;
	private:
		void setAnimation(ANIMATION_STATE animationState);
	public:
		virtual void OnEvent(Event& event) override;
		virtual void OnUpdate() override;
	};
}