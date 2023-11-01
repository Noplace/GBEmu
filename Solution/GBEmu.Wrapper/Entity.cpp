//Entity.cpp
#include "Entity.h"
#include <iostream>
namespace Core
{
  Entity::Entity(const char* name, float xPos, float yPos)
    : m_Name(name), m_XPos(xPos), m_YPos(yPos)
  {
    std::cout << "Created the Entity object!" << std::endl;
  }
  void Entity::Move(float deltaX, float deltaY)
  {
    m_XPos += deltaX;
    m_YPos += deltaY;
    std::cout << "Moved " << m_Name << " to (" << m_XPos << ", " << m_YPos << ")." << std::endl;
  }
}