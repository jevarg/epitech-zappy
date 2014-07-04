#include "FontText.hpp"

FontText::FontText(const sf::Vector2i &pos, const std::string &msg, int size, sf::Color color) : _pos(pos)
{
  _font.loadFromFile(FONT_PATH);
  _text.setFont(_font);
  _text.setString(msg);
  _text.setCharacterSize(size);
  _text.setColor(color);
  _text.setPosition(pos.x, pos.y);
}

FontText::~FontText()
{
  // TODO Auto-generated destructor stub
}

void FontText::draw(sf::RenderWindow &window) const
{
  window.draw(_text);
}

const sf::Vector2i &FontText::getPos() const
{
  return _pos;
}