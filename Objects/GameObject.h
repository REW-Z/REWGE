#pragma once


//�¼����ಥ����ί�У� 
class Listener
{
public:
	Object* listener;
	std::function<void(void)> handler;
	Listener(Object* newlistener, std::function<void(void)> newhandler)
	{
		listener = newlistener;
		handler = newhandler;
	}
};
class Event
{
public:
	std::list<Listener> listeners;

	void Add(Object* listener, std::function<void(void)> handler)
	{
		listeners.push_back(Listener(listener, handler));
	}
	void RemoveAll(Object* listener)
	{
		auto it = listeners.begin();
		while (it != listeners.end())
		{
			if ((*it).listener == listener)
			{
				it = listeners.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
	void Invoke()
	{
		for (auto it = listeners.begin(); it != listeners.end(); ++it)
		{
			(*it).handler();
		}
	}
};


class GameObject : public Object
{
	OBJECT_TYPE_DECLARE(GameObject)

public:
	GameObject();
	~GameObject();
public:
	int layer;
	bool active;

public:
	//�¼�ϵͳ��������Ϣ��  
	Event onTransformChange;

public:
	//�任
	Transform * transform;
	//����б�
	std::vector<Component *> components;
	


	Component * GetComponent(TypeEnum type);

	Component* AddComponentInternal(Component * c);

	Component* AddComponent(TypeEnum type);
};


class GameObjectUtility
{
public:
	static GameObject& CreateGameObject(std::string name, ...);
};
