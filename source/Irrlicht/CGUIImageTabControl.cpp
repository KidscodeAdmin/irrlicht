#include "CGUIImageTabControl.h"

namespace irr
{
namespace gui
{
//! constructor
CGUIImageTab::CGUIImageTab(s32 number, IGUIEnvironment* environment,
	IGUIElement* parent, const core::rect<s32>& rectangle,
	s32 id, video::ITexture *texture, f32 scaling, s32 side)
	: IGUITab(environment, parent, id, rectangle), Number(number),
		BackColor(0,0,0,0), OverrideTextColorEnabled(false), TextColor(255,0,0,0),
		DrawBackground(false), 
		Texture(texture), Scaling(scaling), Side(side), Active(false), Drawn(false),
		DrawnRect(rectangle)
{
	#ifdef _DEBUG
	setDebugName("CGUIImageTab");
	#endif

	const IGUISkin* const skin = environment->getSkin();
	if (skin)
		TextColor = skin->getColor(EGDC_BUTTON_TEXT);
}


//! Returns number of tab in tabcontrol. Can be accessed
//! later IGUITabControl::getTab() by this number.
s32 CGUIImageTab::getNumber() const
{
	return Number;
}


//! Sets the number
void CGUIImageTab::setNumber(s32 n)
{
	Number = n;
}

void CGUIImageTab::refreshSkinColors()
{
	if ( !OverrideTextColorEnabled )
	{
		TextColor = Environment->getSkin()->getColor(EGDC_BUTTON_TEXT);
	}
}

//! draws the element and its children
void CGUIImageTab::draw()
{
	if (!IsVisible)
		return;

	IGUIElement::draw();
}


//! sets if the tab should draw its background
void CGUIImageTab::setDrawBackground(bool draw)
{
	DrawBackground = draw;
}


//! sets the color of the background, if it should be drawn.
void CGUIImageTab::setBackgroundColor(video::SColor c)
{
	BackColor = c;
}


//! sets the color of the text
void CGUIImageTab::setTextColor(video::SColor c)
{
	OverrideTextColorEnabled = true;
	TextColor = c;
}


video::SColor CGUIImageTab::getTextColor() const
{
	return TextColor;
}


//! returns true if the tab is drawing its background, false if not
bool CGUIImageTab::isDrawingBackground() const
{
	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return DrawBackground;
}


//! returns the color of the background
video::SColor CGUIImageTab::getBackgroundColor() const
{
	return BackColor;
}


//! Writes attributes of the element.
void CGUIImageTab::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const
{
	IGUITab::serializeAttributes(out,options);

	out->addInt		("TabNumber",		Number);
	out->addBool	("DrawBackground",	DrawBackground);
	out->addColor	("BackColor",		BackColor);
	out->addBool	("OverrideTextColorEnabled", OverrideTextColorEnabled);
	out->addColor	("TextColor",		TextColor);

}


//! Reads attributes of the element
void CGUIImageTab::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0)
{
	IGUITab::deserializeAttributes(in,options);

	setNumber(in->getAttributeAsInt("TabNumber"));
	setDrawBackground(in->getAttributeAsBool("DrawBackground"));
	setBackgroundColor(in->getAttributeAsColor("BackColor"));
	bool override = in->getAttributeAsBool("OverrideTextColorEnabled");
	setTextColor(in->getAttributeAsColor("TextColor"));
	if ( !override )
	{
		OverrideTextColorEnabled = false;
	}

	if (Parent && Parent->getType() == EGUIET_TAB_CONTROL)
	{
		((CGUIImageTabControl*)Parent)->addTab(this);
		if (isVisible())
			((CGUIImageTabControl*)Parent)->setActiveTab(this);
	}
}


//! Draws the tab image
void CGUIImageTab::drawImage(
	const irr::core::rect<s32>& tabRect
	)
{
	if (Texture)
	{
		f32 margin = 4;
		
		f32 max_width = ( tabRect.LowerRightCorner.X - tabRect.UpperLeftCorner.X - 2 * margin ) * Scaling;
		f32 max_height = ( tabRect.LowerRightCorner.Y - tabRect.UpperLeftCorner.Y - 2 * margin ) * Scaling;
		
		f32 tab_height = max_height;
		f32 tab_width = tab_height * Texture->getSize().Width / Texture->getSize().Height;
		
		if ( tab_width > max_width )
		{
			tab_height *= max_width / tab_width;
			tab_width = max_width;
		}
		
		f32 middle_x = ( tabRect.LowerRightCorner.X + tabRect.UpperLeftCorner.X ) * 0.5f;
		f32 middle_y = ( tabRect.LowerRightCorner.Y + tabRect.UpperLeftCorner.Y ) * 0.5f;
		
		video::IVideoDriver* driver = Environment->getVideoDriver();

		driver->draw2DImage(Texture,
			irr::core::rect<s32>(middle_x - tab_width * 0.5f, middle_y - tab_height * 0.5f, 
				middle_x + tab_width * 0.5f, middle_y + tab_height * 0.5f ), 
			irr::core::rect<s32>(0, 0, Texture->getSize().Width, Texture->getSize().Height), 
			0, 0, true);
	}
}

// ------------------------------------------------------------------
// Tabcontrol
// ------------------------------------------------------------------

//! constructor
CGUIImageTabControl::CGUIImageTabControl(IGUIEnvironment* environment,
	IGUIElement* parent, const core::rect<s32>& rectangle,
	bool show_background, bool show_border, s32 side, s32 id, 
	s32 tab_height, s32 tab_width, s32 tab_padding, s32 tab_spacing,
	s32 width, s32 height, s32 border_width, s32 border_height, s32 border_offset,
	s32 button_width, s32 button_height, 
	s32 button_spacing, s32 button_offset, s32 button_distance, 
	video::ITexture* content_texture, 
	video::ITexture* top_tab_texture, video::ITexture* top_active_tab_texture,
	video::ITexture* bottom_tab_texture, video::ITexture* bottom_active_tab_texture,
	video::ITexture* left_tab_texture, video::ITexture* left_active_tab_texture,
	video::ITexture* right_tab_texture, video::ITexture* right_active_tab_texture,
	video::ITexture* prior_arrow_texture, video::ITexture* prior_arrow_pressed_texture, 
	video::ITexture* next_arrow_texture, video::ITexture* next_arrow_pressed_texture)
	: IGUITabControl(environment, parent, id, rectangle),  
	Tabs(), ShowBackground(show_background), ShowBorder(show_border), Side(side),
	TabHeight(tab_height), TabWidth(tab_width), 
	TabPadding(tab_padding), TabSpacing(tab_spacing),
	Width(width), Height(height),
	BorderWidth(border_width), BorderHeight(border_height), BorderOffset(border_offset),
	ButtonWidth(button_width), ButtonHeight(button_height), 
	ButtonSpacing(button_spacing), ButtonOffset(button_offset), ButtonDistance(button_distance), 
	VerticalAlignment(EGUIA_UPPERLEFT), 
	ScrollControl(false), PriorArrow(0), NextArrow(0), ActiveTabIndex(-1), 
	FirstScrollTabIndex(0), LastScrollTabIndex(-1),	TabContentTexture(content_texture),
	TopTabTexture(top_tab_texture), TopActiveTabTexture(top_active_tab_texture),
	BottomTabTexture(bottom_tab_texture), BottomActiveTabTexture(bottom_active_tab_texture),
	LeftTabTexture(left_tab_texture), LeftActiveTabTexture(left_active_tab_texture),
	RightTabTexture(right_tab_texture), RightActiveTabTexture(right_active_tab_texture),
	PriorArrowTexture(prior_arrow_texture), PriorArrowPressedTexture(prior_arrow_pressed_texture), 
	NextArrowTexture(next_arrow_texture), NextArrowPressedTexture(next_arrow_pressed_texture),
	ContentRect(0, 0, 0, 0)
{
	#ifdef _DEBUG
	setDebugName("CGUIImageTabControl");
	#endif
	
	PriorArrow = Environment->addButton(core::rect<s32>(0,0,10,10), this);

	if ( PriorArrow )
	{
        PriorArrow->setImage(PriorArrowTexture);
        PriorArrow->setPressedImage(PriorArrowPressedTexture);
        PriorArrow->setDrawBorder(false);
        PriorArrow->setScaleImage(true);
		PriorArrow->setUseAlphaChannel(true);
		PriorArrow->setVisible(false);
		PriorArrow->setSubElement(true);
		PriorArrow->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
		PriorArrow->setOverrideFont(Environment->getBuiltInFont());
		PriorArrow->grab();
	}

	NextArrow = Environment->addButton(core::rect<s32>(0,0,10,10), this);

	if ( NextArrow )
	{
        NextArrow->setImage(NextArrowTexture);
        NextArrow->setPressedImage(NextArrowPressedTexture);
        NextArrow->setDrawBorder(false);
        NextArrow->setScaleImage(true);
		NextArrow->setUseAlphaChannel(true);
		NextArrow->setVisible(false);
		NextArrow->setSubElement(true);
		NextArrow->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
		NextArrow->setOverrideFont(Environment->getBuiltInFont());
		NextArrow->grab();
	}

	setTabVerticalAlignment(EGUIA_UPPERLEFT);
	refreshSprites();
}

//! destructor
CGUIImageTabControl::~CGUIImageTabControl()
{
	for (u32 i=0; i<Tabs.size(); ++i)
	{
		if (Tabs[i])
			Tabs[i]->drop();
	}

	if (PriorArrow)
		PriorArrow->drop();

	if (NextArrow)
		NextArrow->drop();
}

void CGUIImageTabControl::refreshSprites()
{
	video::SColor color(255,255,255,255);
	IGUISkin* skin = Environment->getSkin();
	
	if (skin)
	{
		color = skin->getColor(isEnabled() ? EGDC_WINDOW_SYMBOL : EGDC_GRAY_WINDOW_SYMBOL);
	}

	if (PriorArrow)
	{
		PriorArrow->setSprite(EGBS_BUTTON_UP, skin->getIcon(EGDI_CURSOR_LEFT), color);
		PriorArrow->setSprite(EGBS_BUTTON_DOWN, skin->getIcon(EGDI_CURSOR_LEFT), color);
	}

	if (NextArrow)
	{
		NextArrow->setSprite(EGBS_BUTTON_UP, skin->getIcon(EGDI_CURSOR_RIGHT), color);
		NextArrow->setSprite(EGBS_BUTTON_DOWN, skin->getIcon(EGDI_CURSOR_RIGHT), color);
	}
}

//! Adds a tab
IGUITab* CGUIImageTabControl::addTab(const wchar_t* caption, s32 id)
{
	return addImageTab(caption, id, 0);
}

//! Adds an image tab
CGUIImageTab* CGUIImageTabControl::addImageTab(const wchar_t* caption, s32 id, 
	video::ITexture *texture, f32 scaling)
{
	CGUIImageTab* tab = new CGUIImageTab(Tabs.size(), Environment, this, calcRelativeRect(), id, 
		texture, scaling);

	if (!texture)
	{
		tab->setText(caption);
	}
	
	tab->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT);
	tab->setVisible(false);
	Tabs.push_back(tab);

	if (ActiveTabIndex == -1)
	{
		ActiveTabIndex = 0;
		tab->setVisible(true);
	}

	return tab;
}


//! adds a tab which has been created elsewhere
void CGUIImageTabControl::addTab(CGUIImageTab* tab)
{
	if (!tab)
		return;

	// check if its already added
	for (u32 i=0; i < Tabs.size(); ++i)
	{
		if (Tabs[i] == tab)
			return;
	}

	tab->grab();

	if (tab->getNumber() == -1)
		tab->setNumber((s32)Tabs.size());

	while (tab->getNumber() >= (s32)Tabs.size())
		Tabs.push_back(0);

	if (Tabs[tab->getNumber()])
	{
		Tabs.push_back(Tabs[tab->getNumber()]);
		Tabs[Tabs.size()-1]->setNumber(Tabs.size());
	}
	Tabs[tab->getNumber()] = tab;

	if (ActiveTabIndex == -1)
		ActiveTabIndex = tab->getNumber();


	if (tab->getNumber() == ActiveTabIndex)
	{
		setActiveTab(ActiveTabIndex);
	}
}

//! Insert the tab at the given index
IGUITab* CGUIImageTabControl::insertTab(s32 idx, const wchar_t* caption, s32 id)
{
	if ( idx < 0 || idx > (s32)Tabs.size() )	// idx == Tabs.size() is indeed ok here as core::array can handle that
		return NULL;

	CGUIImageTab* tab = new CGUIImageTab(idx, Environment, this, calcRelativeRect(), id);

	tab->setText(caption);
	tab->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT);
	tab->setVisible(false);
	Tabs.insert(tab, (u32)idx);

	if (ActiveTabIndex == -1)
	{
		ActiveTabIndex = 0;
		tab->setVisible(true);
	}

	for ( u32 i=(u32)idx+1; i < Tabs.size(); ++i )
	{
		Tabs[i]->setNumber(i);
	}

	return tab;
}

//! Removes a tab from the tabcontrol
void CGUIImageTabControl::removeTab(s32 idx)
{
	if ( idx < 0 || idx >= (s32)Tabs.size() )
		return;

	Tabs[(u32)idx]->drop();
	Tabs.erase((u32)idx);
	for ( u32 i=(u32)idx; i < Tabs.size(); ++i )
	{
		Tabs[i]->setNumber(i);
	}
}

//! Clears the tabcontrol removing all tabs
void CGUIImageTabControl::clear()
{
	for (u32 i=0; i<Tabs.size(); ++i)
	{
		if (Tabs[i])
			Tabs[i]->drop();
	}
	Tabs.clear();
}

//! Returns amount of tabs in the tabcontrol
s32 CGUIImageTabControl::getTabCount() const
{
	return Tabs.size();
}


//! Returns a tab based on zero based index
IGUITab* CGUIImageTabControl::getTab(s32 idx) const
{
	if ((u32)idx >= Tabs.size())
		return 0;

	return Tabs[idx];
}


//! called if an event happened.
bool CGUIImageTabControl::OnEvent(const SEvent& event)
{
	if (isEnabled())
	{

		switch(event.EventType)
		{
		case EET_GUI_EVENT:
			switch(event.GUIEvent.EventType)
			{
			case EGET_BUTTON_CLICKED:
				if (event.GUIEvent.Caller == PriorArrow)
				{
					scrollLeft();
					return true;
				}
				else if (event.GUIEvent.Caller == NextArrow)
				{
					scrollRight();
					return true;
				}

			break;
			default:
			break;
			}
			break;
		case EET_MOUSE_INPUT_EVENT:
			switch(event.MouseInput.Event)
			{
			case EMIE_LMOUSE_PRESSED_DOWN:
				// todo: dragging tabs around
				return true;
			case EMIE_LMOUSE_LEFT_UP:
			{
				s32 idx = getTabAt(event.MouseInput.X, event.MouseInput.Y);
				if ( idx >= 0 )
				{
					setActiveTab(idx);
					return true;
				}
				break;
			}
			default:
				break;
			}
			break;
		default:
			break;
		}
	}

	return IGUIElement::OnEvent(event);
}


void CGUIImageTabControl::scrollLeft()
{
	if ( ScrollControl
	     && FirstScrollTabIndex > 0 )
	{
		--FirstScrollTabIndex;
	}
}


void CGUIImageTabControl::scrollRight()
{
	if ( ScrollControl
		 && FirstScrollTabIndex < (s32)(Tabs.size()) - 1 )
	{
		++FirstScrollTabIndex;
	}
}


s32 CGUIImageTabControl::calcTabWidth(s32 pos, IGUIFont* font, const wchar_t* text, bool withScrollControl,
	CGUIImageTab* tab) const
{
	if ( !font )
		return 0;

	if ( Side >= 2 )
		return TabWidth;

	s32 len = font->getDimension(text).Width + TabPadding;
	
	if ( tab->Texture )
	{
		len = TabHeight * tab->Scaling * tab->Texture->getSize().Width / tab->Texture->getSize().Height + TabPadding;
	}
			

	// check if we miss the place to draw the tab-button
	if ( withScrollControl && ScrollControl && pos+len > PriorArrow->getAbsolutePosition().UpperLeftCorner.X - 2 )
	{
		s32 tabMinWidth = font->getDimension(L"A").Width;
		
		if ( TabPadding > 0 && tabMinWidth < TabPadding )
			tabMinWidth = TabPadding;
	}
	
	return len;
}


void CGUIImageTabControl::calcTabs()
{	
	if ( !IsVisible )
		return;

	IGUISkin* skin = Environment->getSkin();
	
	if ( !skin )
		return;

	IGUIFont* font = skin->getFont();
	
	if ( !font )
		return;

	if ( FirstScrollTabIndex >= (s32)Tabs.size() )
		FirstScrollTabIndex = ((s32)Tabs.size()) - 1;

	if ( FirstScrollTabIndex < 0 )
		FirstScrollTabIndex = 0;
		
	s32 pos;

	if ( Side < 2 )
	{
		pos = AbsoluteRect.UpperLeftCorner.X + BorderWidth;
	}
	else
	{
		pos = AbsoluteRect.UpperLeftCorner.Y + BorderHeight;
	}
	
	CGUIImageTab* tab;
	
	for (u32 i=FirstScrollTabIndex; i<Tabs.size(); ++i)
	{
		tab = Tabs[i];
		
		if ( tab )
		{
			tab->Active = false;
			tab->Drawn = false;
		}
	}
	
	core::rect<s32> tabRect;
	
	LastScrollTabIndex = -1;

	for (u32 i=FirstScrollTabIndex; i<Tabs.size(); ++i)
	{
		tab = Tabs[i];
		
		if ( tab )
		{
			const wchar_t* text = 0;
		
			text = Tabs[i]->getText();

			// get text length
			s32 len = calcTabWidth(pos, font, text, true, tab);
					
			if ( Side < 2 )
			{
				tabRect.UpperLeftCorner.X = pos;
				pos += len + TabSpacing;
				
				if ( ScrollControl
					 && pos > AbsoluteRect.LowerRightCorner.X - ButtonOffset - 2 * ( ButtonWidth + ButtonSpacing ) - BorderWidth )
				{
					break;		
				}				
				
				if ( pos > AbsoluteRect.LowerRightCorner.X - BorderWidth )
				{
					ScrollControl = true;	
					break;		
				}				
			}
			else
			{
				tabRect.UpperLeftCorner.Y = pos;			
				pos += TabHeight + TabSpacing;
				
				if ( ScrollControl
					 && pos > AbsoluteRect.LowerRightCorner.Y - ButtonOffset - 2 * ( ButtonHeight + ButtonSpacing ) - BorderHeight )
				{
					break;		
				}				
				
				if ( pos > AbsoluteRect.LowerRightCorner.Y - BorderHeight )
				{			
					ScrollControl = true;
					break;		
				}
			}

			if ( Side == 0 )
			{
				tabRect.UpperLeftCorner.Y = AbsoluteRect.UpperLeftCorner.Y;
			}
			else if ( Side == 1 )
			{
				tabRect.UpperLeftCorner.Y = AbsoluteRect.LowerRightCorner.Y - TabHeight;
			}
			else if ( Side == 2 )
			{
				tabRect.UpperLeftCorner.X = AbsoluteRect.UpperLeftCorner.X;
			}
			else
			{
				tabRect.UpperLeftCorner.X = AbsoluteRect.LowerRightCorner.X - TabWidth;
			}
			
			tabRect.LowerRightCorner.X = tabRect.UpperLeftCorner.X + len;
			tabRect.LowerRightCorner.Y = tabRect.UpperLeftCorner.Y + TabHeight;

			if ( i == (u32)ActiveTabIndex )
			{
				tab->Active = true;
			}
			
			tab->Drawn = true;
			tab->DrawnRect = tabRect;
			
			if ( text )
				tab->refreshSkinColors();
				
			LastScrollTabIndex = i;
		}
	}
	
	ContentRect = AbsoluteRect;
	
	if ( Side == 0 )
	{
		ContentRect.UpperLeftCorner.Y += TabHeight;
	}
	else if ( Side == 1 )
	{
		ContentRect.LowerRightCorner.Y -= TabHeight;
	}
	else if ( Side == 2 )
	{
		ContentRect.UpperLeftCorner.X += TabWidth;
	}
	else if ( Side == 3 )
	{
		ContentRect.LowerRightCorner.X -= TabWidth;
	}
}


void CGUIImageTabControl::calcScrollButtons()
{
	core::rect<s32> buttonRect;
	
	if ( Side < 2 )
	{
		buttonRect.UpperLeftCorner.X = AbsoluteRect.getWidth() - ButtonOffset - 2 * ButtonWidth - ButtonSpacing;
		
		if ( Side == 0 )
		{
			buttonRect.UpperLeftCorner.Y = TabHeight - ButtonHeight - ButtonDistance;
		}
		else
		{
			buttonRect.UpperLeftCorner.Y = AbsoluteRect.getHeight() - TabHeight + ButtonDistance;
		}
		
		buttonRect.LowerRightCorner.X = buttonRect.UpperLeftCorner.X + ButtonWidth;
		buttonRect.LowerRightCorner.Y = buttonRect.UpperLeftCorner.Y + ButtonHeight;
		PriorArrow->setRelativePosition(buttonRect);

		buttonRect.UpperLeftCorner.X += ButtonWidth + ButtonSpacing;
		
		buttonRect.LowerRightCorner.X = buttonRect.UpperLeftCorner.X + ButtonWidth;
		buttonRect.LowerRightCorner.Y = buttonRect.UpperLeftCorner.Y + ButtonHeight;
		NextArrow->setRelativePosition(buttonRect);
	}
	else
	{
		buttonRect.UpperLeftCorner.Y = AbsoluteRect.getHeight() - ButtonOffset - 2 * ButtonHeight - ButtonSpacing;
		
		if ( Side == 2 )
		{
			buttonRect.UpperLeftCorner.X = TabWidth - ButtonWidth - ButtonDistance;
		}
		else
		{
			buttonRect.UpperLeftCorner.X = AbsoluteRect.getWidth() - TabWidth + ButtonDistance;
		}
		
		buttonRect.LowerRightCorner.X = buttonRect.UpperLeftCorner.X + ButtonWidth;
		buttonRect.LowerRightCorner.Y = buttonRect.UpperLeftCorner.Y + ButtonHeight;
		PriorArrow->setRelativePosition(buttonRect);

		buttonRect.UpperLeftCorner.Y += ButtonHeight + ButtonSpacing;
		
		buttonRect.LowerRightCorner.X = buttonRect.UpperLeftCorner.X + ButtonWidth;
		buttonRect.LowerRightCorner.Y = buttonRect.UpperLeftCorner.Y + ButtonHeight;
		NextArrow->setRelativePosition(buttonRect);
	}
	
	if (!PriorArrow || !NextArrow)
		return;
		
	PriorArrow->setVisible( ScrollControl );
	NextArrow->setVisible( ScrollControl );

	bringToFront( PriorArrow );
	bringToFront( NextArrow );
}


//! Computes a tab position
core::rect<s32> CGUIImageTabControl::calcRelativeRect()
{
	core::rect<s32> r;
	
	
	r.UpperLeftCorner.X = 0;
	r.UpperLeftCorner.Y = 0;
	r.LowerRightCorner.X = AbsoluteRect.getWidth();	
	r.LowerRightCorner.Y = AbsoluteRect.getHeight();
	
	if ( Side == 0 )
	{
		r.UpperLeftCorner.Y += TabHeight;
	}
	else if ( Side == 1 )
	{
		r.LowerRightCorner.Y -= TabHeight;
	}
	else if ( Side == 2 )
	{
		r.UpperLeftCorner.X += TabWidth;
	}
	else if ( Side == 3 )
	{
		r.LowerRightCorner.X -= TabWidth;
	}

	return r;
}


//! Draws an expanded image
void CGUIImageTabControl::drawExpandedImage(const irr::core::rect<s32>& tab_rect, 
	const video::ITexture *texture, const s32 border_width, const s32 border_height)
{
	if (texture)
	{
		s32 texture_width = texture->getSize().Width;
		s32 texture_height = texture->getSize().Height;
		
		s32 left = tab_rect.UpperLeftCorner.X;
		s32 right = tab_rect.LowerRightCorner.X;
		s32 top = tab_rect.UpperLeftCorner.Y;
		s32 bottom = tab_rect.LowerRightCorner.Y;
		
		video::IVideoDriver* driver = Environment->getVideoDriver();

		driver->draw2DImage(texture,
			irr::core::rect<s32>(left, top, left + border_width, top + border_height), 
			irr::core::rect<s32>(0, 0, border_width, border_height), 
			0, 0, true);

		driver->draw2DImage(texture,
			irr::core::rect<s32>(left + border_width, top, right - border_width, top + border_height), 
			irr::core::rect<s32>(border_width, 0, texture_width - border_width, border_height),  
			0, 0, true);

		driver->draw2DImage(texture,
			irr::core::rect<s32>(right - border_width, top, right, top + border_height), 
			irr::core::rect<s32>(texture_width - border_width, 0, texture_width, border_height),  
			0, 0, true);

		driver->draw2DImage(texture,
			irr::core::rect<s32>(left, top + border_height, left + border_width, bottom - border_height), 
			irr::core::rect<s32>(0, border_height, border_width, texture_height - border_height),  
			0, 0, true);

		driver->draw2DImage(texture,
			irr::core::rect<s32>(left + border_width, top + border_height, right - border_width, bottom - border_height), 
			irr::core::rect<s32>(border_width, border_height, texture_width - border_width, texture_height - border_height),  
			0, 0, true);

		driver->draw2DImage(texture,
			irr::core::rect<s32>(right - border_width, top + border_height, right, bottom - border_height), 
			irr::core::rect<s32>(texture_width - border_width, border_height, texture_width, texture_height - border_height),  
			0, 0, true);

		driver->draw2DImage(texture,
			irr::core::rect<s32>(left, bottom - border_height, left + border_width, bottom), 
			irr::core::rect<s32>(0, texture_height - border_height, border_width, texture_height),  
			0, 0, true);
			
		driver->draw2DImage(texture,
			irr::core::rect<s32>(left + border_width, bottom - border_height, right - border_width, bottom), 
			irr::core::rect<s32>(border_width, texture_height - border_height, texture_width - border_width, texture_height),  
			0, 0, true);
			
		driver->draw2DImage(texture,
			irr::core::rect<s32>(right - border_width, bottom - border_height, right, bottom), 
			irr::core::rect<s32>(texture_width - border_width, texture_height - border_height, texture_width, texture_height),  
			0, 0, true);
	}
}


//! Draws a tab
void CGUIImageTabControl::drawTab(CGUIImageTab* tab, IGUIFont* font)
{
	core::rect<s32> tab_rect(tab->DrawnRect);				
	const wchar_t* text = tab->getText();
	video::ITexture* tab_texture;

	if ( Side == 0 )
	{
		tab_rect.LowerRightCorner.Y += BorderOffset;
		tab_texture = tab->Active ? TopActiveTabTexture : TopTabTexture;
	}
	else if ( Side == 1 )
	{
		tab_rect.UpperLeftCorner.Y -= BorderOffset;
		tab_texture = tab->Active ? BottomActiveTabTexture : BottomTabTexture;
	}
	else if ( Side == 2 )
	{
		tab_rect.LowerRightCorner.X += BorderOffset;
		tab_texture = tab->Active ? LeftActiveTabTexture : LeftTabTexture;
	}
	else
	{
		tab_rect.UpperLeftCorner.X -= BorderOffset;
		tab_texture = tab->Active ? RightActiveTabTexture : RightTabTexture;
	}
	
	drawExpandedImage(tab_rect, tab_texture, BorderWidth, BorderHeight);

	if ( text )
	{
		// draw text
		font->draw(text, tab_rect, tab->getTextColor(),
			true, true, &tab_rect);
	}
		
	tab->drawImage(tab_rect);
}


//! draws the element and its children
void CGUIImageTabControl::draw()
{
	if ( !IsVisible )
		return;

	IGUISkin* skin = Environment->getSkin();
	
	if ( !skin )
		return;

	IGUIFont* font = skin->getFont();

	if ( !font )
		return;
	
	calcTabs();
	calcScrollButtons();
	
	CGUIImageTab* activeTab = 0;
		
	for (s32 i=FirstScrollTabIndex; i<=LastScrollTabIndex; ++i)
	{
		CGUIImageTab* tab = Tabs[i];
		
		if (tab)
		{
			if (tab->Active)
				activeTab = tab;
			else
				drawTab(tab, font);
		}
	}

	drawExpandedImage(ContentRect, TabContentTexture, BorderWidth, BorderHeight);
	
	if (activeTab)
		drawTab(activeTab, font);

	if ( PriorArrow )
		PriorArrow->setEnabled(ScrollControl);
	
	if ( NextArrow )
		NextArrow->setEnabled(ScrollControl);
		
	refreshSprites();

	IGUIElement::draw();
	
	//Environment->getVideoDriver()->draw2DRectangle(video::SColor(32,255,0,0), ContentRect, 0);
}


//! Set the height of the tabs
void CGUIImageTabControl::setTabHeight( s32 height )
{
	if ( height < 0 )
		height = 0;

	TabHeight = height;
}


//! Get the height of the tabs
s32 CGUIImageTabControl::getTabHeight() const
{
	return TabHeight;
}

//! set the maximal width of a tab. Per default width is 0 which means "no width restriction".
void CGUIImageTabControl::setTabWidth(s32 width )
{
	TabWidth = width;
}

//! get the maximal width of a tab
s32 CGUIImageTabControl::getTabWidth() const
{
	return TabWidth;
}


//! Set the extra width added to tabs on each side of the text
void CGUIImageTabControl::setTabPadding( s32 padding )
{
	if ( padding < 0 )
		padding = 0;

	TabPadding = padding;
}


//! Get the extra width added to tabs on each side of the text
s32 CGUIImageTabControl::getTabPadding() const
{
	return TabPadding;
}


//! Set the alignment of the tabs
void CGUIImageTabControl::setTabVerticalAlignment( EGUI_ALIGNMENT alignment )
{
	VerticalAlignment = alignment;

	core::rect<s32> r(calcRelativeRect());
	for ( u32 i=0; i<Tabs.size(); ++i )
	{
		Tabs[i]->setRelativePosition(r);
	}
}

//! Get the alignment of the tabs
EGUI_ALIGNMENT CGUIImageTabControl::getTabVerticalAlignment() const
{
	return VerticalAlignment;
}


s32 CGUIImageTabControl::getTabAt(s32 xpos, s32 ypos) const
{
	core::position2di p(xpos, ypos);

	for (s32 i=FirstScrollTabIndex; i<=LastScrollTabIndex; ++i)
	{
		CGUIImageTab* tab = Tabs[i];
		
		if ( tab )
		{
			if ( tab->Drawn
			     && tab->DrawnRect.isPointInside(p))
			{
				return i;
			}
		}
	}
	return -1;
}

//! Returns which tab is currently active
s32 CGUIImageTabControl::getActiveTab() const
{
	return ActiveTabIndex;
}


//! Brings a tab to front.
bool CGUIImageTabControl::setActiveTab(s32 idx)
{
	if ((u32)idx >= Tabs.size())
		return false;

	bool changed = (ActiveTabIndex != idx);

	ActiveTabIndex = idx;

	for (s32 i=0; i<(s32)Tabs.size(); ++i)
		if (Tabs[i])
			Tabs[i]->setVisible( i == ActiveTabIndex );

	if (changed)
	{
		SEvent event;
		event.EventType = EET_GUI_EVENT;
		event.GUIEvent.Caller = this;
		event.GUIEvent.Element = 0;
		event.GUIEvent.EventType = EGET_TAB_CHANGED;
		Parent->OnEvent(event);
	}

	return true;
}


bool CGUIImageTabControl::setActiveTab(IGUITab *tab)
{
	for (s32 i=0; i<(s32)Tabs.size(); ++i)
		if (Tabs[i] == tab)
			return setActiveTab(i);
	return false;
}


//! Removes a child.
void CGUIImageTabControl::removeChild(IGUIElement* child)
{
	bool isTab = false;

	u32 i=0;
	// check if it is a tab
	while (i<Tabs.size())
	{
		if (Tabs[i] == child)
		{
			Tabs[i]->drop();
			Tabs.erase(i);
			isTab = true;
		}
		else
			++i;
	}

	// reassign numbers
	if (isTab)
	{
		for (i=0; i<Tabs.size(); ++i)
			if (Tabs[i])
				Tabs[i]->setNumber(i);
	}

	// remove real element
	IGUIElement::removeChild(child);
}


//! Update the position of the element, decides scroll button status
void CGUIImageTabControl::updateAbsolutePosition()
{
	IGUIElement::updateAbsolutePosition();
}


//! Writes attributes of the element.
void CGUIImageTabControl::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const
{
	IGUITabControl::serializeAttributes(out,options);

	out->addInt ("ActiveTabIndex",	ActiveTabIndex);
	out->addBool("Border",		ShowBorder);
	out->addBool("FillBackground",	ShowBackground);
	out->addInt ("TabHeight",	TabHeight);
	out->addInt ("TabWidth", TabWidth);
	out->addEnum("TabVerticalAlignment", s32(VerticalAlignment), GUIAlignmentNames);
}


//! Reads attributes of the element
void CGUIImageTabControl::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0)
{
	ShowBorder      = in->getAttributeAsBool("Border");
	ShowBackground  = in->getAttributeAsBool("FillBackground");

	ActiveTabIndex = -1;

	setTabHeight(in->getAttributeAsInt("TabHeight"));
	TabWidth     = in->getAttributeAsInt("TabWidth");

	IGUITabControl::deserializeAttributes(in,options);

	setActiveTab(in->getAttributeAsInt("ActiveTabIndex"));
	setTabVerticalAlignment( static_cast<EGUI_ALIGNMENT>(in->getAttributeAsEnumeration("TabVerticalAlignment" , GUIAlignmentNames)) );
}
} // end namespace irr
} // end namespace gui



