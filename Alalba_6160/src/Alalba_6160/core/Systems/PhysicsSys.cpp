#include "PhysicsSys.h"
#include "Alalba_6160/core/Components.h"
#include "Alalba_6160/core/Entities/Entity.h"
#include "Alalba_6160/core/AABB.h"
#include "MapSys.h"
namespace Alalba
{
	Direction checkCollisionY(Entity* solid, TransformComponent& transform, KineticComponent& kinetic) {
    auto& solidTransform = solid->GetComponent<TransformComponent>();
    auto direction = Direction::NONE;

    if (kinetic.speedY >= 0.0f) {
			// Falling
			if (AABBCollision(
				transform.x + TILE_ROUNDNESS / 2,    // Check previous x position
				transform.y + kinetic.speedY,
				transform.w - TILE_ROUNDNESS,
				transform.h,
				solidTransform)) {

				float distanceTop = abs(solidTransform.top() - (transform.bottom() + kinetic.speedY));
				float distanceBottom = abs((transform.top() + kinetic.speedY) - solidTransform.bottom());
				if (distanceTop < distanceBottom) {
					transform.setBottom(solidTransform.top());
					solid->AddComponent<TopCollisionComponent>();
					direction = Direction::BOTTOM;
				}
			}
    } else {
        // Jumping
			if (AABBCollision(
				transform.x + TILE_ROUNDNESS,    // Check previous x position
				transform.y + kinetic.speedY,
				transform.w - (TILE_ROUNDNESS * 2),
				transform.h,
				solidTransform)) {

				float distanceTop = abs(solidTransform.top() - (transform.bottom() + kinetic.speedY));
				float distanceBottom = abs((transform.top() + kinetic.speedY) - solidTransform.bottom());
				if (distanceTop > distanceBottom) {
					transform.setTop(solidTransform.bottom());
					solid->AddComponent<BottomCollisionComponent>();
					direction = Direction::TOP;
				}
			}
    }
  	return direction;
	}
	



	Direction checkCollisionX(Entity* solid, TransformComponent& transform, KineticComponent& kinetic) {
    auto& solidTransform = solid->GetComponent<TransformComponent>();
    auto direction = Direction::NONE;

    // X-AXIS CHECK
    if (AABBCollision(
            transform.x + kinetic.speedX,
            transform.y + kinetic.speedY + 2,   // Check with updated y position
            transform.w,
            transform.h - 8, //Ideally should be -4, but this favours Mario ending up on TOP of a tile.
            solidTransform)) {

        float distanceLeft = abs((transform.left() + kinetic.speedX) - solidTransform.right());
        float distanceRight = abs((transform.right() + kinetic.speedX) - solidTransform.left());
        if (distanceLeft < distanceRight) {
            if (transform.left() < solidTransform.right()) {
                //item is inside block, push out
                transform.x += std::min(.5f, solidTransform.right() - transform.left());
            } else {
                //item about to get inside the block
                transform.setLeft(solidTransform.right());
            }
            solid->AddComponent<RightCollisionComponent>();
            direction = Direction::LEFT;
        } else {
            if (transform.right() > solidTransform.left()) {
                //item is inside block, push out
                transform.x -= std::min(.5f, transform.right() - solidTransform.left());
            } else {
                //item about to get inside the block
                transform.setRight(solidTransform.left());
            }

            solid->AddComponent<LeftCollisionComponent>();
            direction = Direction::RIGHT;
        }
    }
    return direction;
	}







	void PhysicsSys::OnUpdate(Scene& scene){
	
		auto entities = scene.GetEntities();
	
		for(auto entity: entities)
		{
			if(entity->HasComponent<GravityComponent>() && entity->HasComponent<KineticComponent>())
			{
				entity->GetComponent<KineticComponent>().accY += GRAVITY;
			}

			// Checn Kinetic - Tile Collision 
			// player by now
			if(entity->HasComponent<TransformComponent>() && 
						entity->HasComponent<KineticComponent>() && 
						entity->HasComponent<SolidComponent>())
			{
				auto& transform = entity->GetComponent<TransformComponent>();
        auto& kinetic = entity->GetComponent<KineticComponent>();
				
				int futureLeft = (int) (transform.left() + kinetic.speedX) / TILE_SIZE;
				int futureRight = (int) (transform.right() + kinetic.speedX) / TILE_SIZE;
				int futureTop = (int) (transform.top() + kinetic.speedY) / TILE_SIZE;
				int futureBottom = (int) (transform.bottom() + kinetic.speedY) / TILE_SIZE;
				
				std::unordered_set<TileEntity*> coordinates{

					MapSys::Get(futureRight, futureBottom),
					MapSys::Get(futureLeft, futureBottom),
					MapSys::Get(futureLeft, futureTop),
					MapSys::Get(futureRight, futureTop),
        };

				// Check Collision on Y
				for (auto tile : coordinates) {
					if (tile == entity) continue;
					if (!tile) continue;
					if (!(tile->HasComponent<SolidComponent>())) continue;
					switch (checkCollisionY(tile, transform, kinetic)) {
						case Direction::TOP:
								kinetic.accY = std::max(0.0f, kinetic.accY);
								kinetic.speedY = std::max(0.0f, kinetic.speedY);
								entity->AddComponent<TopCollisionComponent>();
								break;
						case Direction::BOTTOM:
								kinetic.accY = std::min(0.0f, kinetic.accY);
								kinetic.speedY = std::min(0.0f, kinetic.speedY);
								entity->AddComponent<BottomCollisionComponent>();
								break;
						default:
								break;
					}
				}



            futureLeft = (int) (transform.left() + kinetic.speedX) / TILE_SIZE;
            futureRight = (int) (transform.right() + kinetic.speedX) / TILE_SIZE;
            futureTop = (int) (transform.top() + 1) / TILE_SIZE;
            futureBottom = (int) (transform.bottom() - 1) / TILE_SIZE;
            coordinates = {
							MapSys::Get(futureRight, futureBottom),
							MapSys::Get(futureLeft, futureBottom),
							MapSys::Get(futureLeft, futureTop),
							MapSys::Get(futureRight, futureTop),
            };

            for (auto tile : coordinates) {
                if (tile == entity) continue;
                if (!tile) continue;
               	if (!(tile->HasComponent<SolidComponent>())) continue;

                switch (checkCollisionX(tile, transform, kinetic)) {
                    case Direction::LEFT:
                        kinetic.accX = std::max(0.0f, kinetic.accX);
                        kinetic.speedX = std::max(0.0f, kinetic.speedX);
                        entity->AddComponent<LeftCollisionComponent>();
                        break;
                    case Direction::RIGHT:
                        kinetic.accX = std::min(0.0f, kinetic.accX);
                        kinetic.speedX = std::min(0.0f, kinetic.speedX);
                        entity->AddComponent<RightCollisionComponent>();
                        break;
                    default:
                        break;
                }
            }



			}

			// Apply force
			if(entity->HasComponent<TransformComponent>() && entity->HasComponent<KineticComponent>())
			{
				auto& transform = entity->GetComponent<TransformComponent>();
        auto& kinematic = entity->GetComponent<KineticComponent>();
        transform.x += kinematic.speedX;
        transform.y += kinematic.speedY;
        kinematic.speedX += kinematic.accX;
        kinematic.speedY += kinematic.accY;

        if (std::abs(kinematic.speedY) < MARIO_ACCELERATION_X) kinematic.speedY = 0;
        if (std::abs(kinematic.speedX) < MARIO_ACCELERATION_X) kinematic.speedX = 0;
        kinematic.speedY *= FRICTION;
        kinematic.speedX *= FRICTION;

        if (kinematic.speedY > MAX_SPEED_Y) kinematic.speedY = MAX_SPEED_Y;
        if (kinematic.speedX > MAX_SPEED_X) kinematic.speedX = MAX_SPEED_X;

        if (kinematic.speedY < -MAX_SPEED_Y) kinematic.speedY = -MAX_SPEED_Y;
        if (kinematic.speedX < -MAX_SPEED_X) kinematic.speedX = -MAX_SPEED_X;
			}
		}
	}

}