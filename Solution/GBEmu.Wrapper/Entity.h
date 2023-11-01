#pragma once
namespace Core
{
  class Entity
  {
  public:
    const char* m_Name;
  private:
    float m_XPos, m_YPos;
  public:
    Entity(const char* name, float xPos, float yPos);

    void Move(float deltaX, float deltaY);
    inline float GetXPosition() const { return m_XPos; };
    inline float GetYPosition() const { return m_YPos; };
  };
}