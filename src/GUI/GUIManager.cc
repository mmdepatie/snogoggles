#include <SFML/Window/Event.hpp>

#include <Viewer/GUIManager.hh>
#include <Viewer/UIEvent.hh>
#include <Viewer/GUI.hh>
using namespace Viewer;

GUIManager::~GUIManager()
{
  for( unsigned int uGUI = 0; uGUI < fGUIObjects.size(); uGUI++ )
    delete fGUIObjects[uGUI];
  fGUIObjects.clear();
}

GUIReturn 
GUIManager::NewEvent( UIEvent& event )
{
  GUIReturn retEvent; // Returned event
  int oldFocus = fFocus;
  switch( event.Type )
    {
// First events that go straight through to Focus
    case sf::Event::KeyPressed:
    case sf::Event::KeyReleased:
      if( fFocus >= 0 && fFocus < fGUIObjects.size() )
	retEvent = fGUIObjects[fFocus]->NewEvent( event );
      break;

// Now events that go straight through to Focus and change the focus   
    case sf::Event::MouseButtonReleased:
      if( fFocus >= 0 && fFocus < fGUIObjects.size() )
	retEvent = fGUIObjects[fFocus]->NewEvent( event );
      fFocus = FindGUI( event.GetLocalCoord() );
      break;
    case sf::Event::LostFocus:
      if( fFocus >= 0 && fFocus < fGUIObjects.size() )
	retEvent = fGUIObjects[fFocus]->NewEvent( event );
      fFocus = -1;
      break;

// Now events that change the focus   
    case sf::Event::MouseMoved:
    case sf::Event::MouseButtonPressed:
      fFocus = FindGUI( event.GetLocalCoord() );   
      if( fFocus >= 0 && fFocus < fGUIObjects.size() )
	retEvent = fGUIObjects[fFocus]->NewEvent( event );
      break;
    }
// Focus change
  if( oldFocus != fFocus && oldFocus != -1 ) 
    {
      event.Type = sf::Event::LostFocus;
      fGUIObjects[oldFocus]->NewEvent( event );
    }
  return retEvent;
}
  
void 
GUIManager::Render( RWWrapper& windowApp )
{
  for( unsigned int uGUI = 0; uGUI < fGUIObjects.size(); uGUI++ )
    if( !fGUIObjects[uGUI]->Hidden() )
      fGUIObjects[uGUI]->Render( windowApp );
}
  

GUI* 
GUIManager::GetGUI( unsigned int guiID )
{
  return fGUIObjects[guiID];
}

int
GUIManager::FindGUI( sf::Vector2<double> localCoord )
{
  for( unsigned int uGUI = 0; uGUI < fGUIObjects.size(); uGUI++ )
    if( !fGUIObjects[uGUI]->Hidden() && fGUIObjects[uGUI]->ContainsPoint( localCoord ) )
      return uGUI;
  return -1;
}
