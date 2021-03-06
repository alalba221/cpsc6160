#pragma once
#include "entt.hpp"
#include "Alalba_6160/core/Components.h"
namespace Alalba
{
	class Entity;
	class Scene
	{
	public:
		Scene();
		~Scene();

		entt::registry& Reg() { return m_Registry; }

		// Entity* CreateEntity(const std::string& name = std::string());
		void Init();
		void OnUpdate();
		std::list<Entity*> GetEntities(){return m_Entities;}
		void AddEntity(Entity* entity,const std::string& name = std::string());
	private:
		entt::registry m_Registry;
		std::list<Entity*> m_Entities;
		friend class Entity;
	};
}