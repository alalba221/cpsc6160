#include "alalbapch.h"
#include "Entity.h"

namespace Alalba {

	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
	}
	void Entity::OnUpdate(float t)
	{
		if(this->HasComponent<TransformComponent>() && this->HasComponent<Rigidbody2DComponent>())
		{
			auto& transform = this->GetComponent<TransformComponent>();
			
			transform.CenterVelocity += transform.dV;
			transform.AngularVelocity += transform.dW;
			transform.Translation += transform.CenterVelocity * t; 
			transform.Rotation += transform.AngularVelocity * t;

			transform.dV = glm::vec3{0,0,0};
			transform.dW = glm::vec3{0,0,0};
			std::cout<<transform.CenterVelocity.y<<std::endl;
		}
	}
}
