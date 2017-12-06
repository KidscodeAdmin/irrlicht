// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_TEXT_SCENE_NODE_H_INCLUDED__
#define __C_TEXT_SCENE_NODE_H_INCLUDED__

#include "ITextSceneNode.h"
#include "IBillboardTextSceneNode.h"
#include "IGUIFont.h"
#include "IGUIFontBitmap.h"
#include "ISceneCollisionManager.h"
#include "SMesh.h"
#include <string> // :PATCH:

namespace irr
{
namespace scene
{


	class CTextSceneNode : public ITextSceneNode
	{
	public:

		//! constructor
		CTextSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
			gui::IGUIFont* font, scene::ISceneCollisionManager* coll,
			const core::vector3df& position = core::vector3df(0,0,0), const wchar_t* text=0,
			video::SColor color=video::SColor(100,0,0,0));

		//! destructor
		virtual ~CTextSceneNode();

		virtual void OnRegisterSceneNode();

		//! renders the node.
		virtual void render();

		//! returns the axis aligned bounding box of this node
		virtual const core::aabbox3d<f32>& getBoundingBox() const;

		//! sets the text string
		virtual void setText(const wchar_t* text);

		//! sets the color of the text
		virtual void setTextColor(video::SColor color);

		//! Returns type of the scene node
		virtual ESCENE_NODE_TYPE getType() const { return ESNT_TEXT; }

	private:

		core::stringw Text;
		video::SColor Color;
		gui::IGUIFont* Font;
		scene::ISceneCollisionManager* Coll;
		core::aabbox3d<f32> Box;
	};

	class CBillboardTextSceneNode : public IBillboardTextSceneNode
	{
	public:

		CBillboardTextSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
			gui::IGUIFont* font,const wchar_t* text,
			const core::vector3df& position, const core::dimension2d<f32>& size,
			const video::SColor colorTop, const video::SColor colorBottom,
			const bool background, const video::SColor & backgroundColor, 
			const video::SColor & borderColor, const f32 border,
			const f32 xPadding, const f32 yPadding,
			const f32 xOffset, const f32 yOffset); // :PATCH:

		//! destructor
		virtual ~CBillboardTextSceneNode();

		//! sets the vertex positions etc
		virtual void OnAnimate(u32 timeMs);

		//! registers the node into the transparent pass
		virtual void OnRegisterSceneNode();

		//! renders the background.
		virtual void renderBackground();
		
		//! renders the node.
		virtual void render();

		//! returns the axis aligned bounding box of this node
		virtual const core::aabbox3d<f32>& getBoundingBox() const;

		//! sets the text string
		virtual void setText(const wchar_t* text);

		//! sets the color of the text
		virtual void setTextColor(video::SColor color);

		//! sets the size of the billboard
		virtual void setSize(const core::dimension2d<f32>& size);
		
		//! resize the billboard
		virtual void resize();

		//! gets the size of the billboard
		virtual const core::dimension2d<f32>& getSize() const;

		virtual video::SMaterial& getMaterial(u32 i);

		//! returns amount of materials used by this scene node.
		virtual u32 getMaterialCount() const;

		//! Returns type of the scene node
		virtual ESCENE_NODE_TYPE getType() const { return ESNT_TEXT; }

		//! Set the color of all vertices of the billboard
		//! \param overallColor: the color to set
		virtual void setColor(const video::SColor & overallColor);

		//! Set the color of the top and bottom vertices of the billboard
		//! \param topColor: the color to set the top vertices
		//! \param bottomColor: the color to set the bottom vertices
		virtual void setColor(const video::SColor & topColor, const video::SColor & bottomColor);

		//! Gets the color of the top and bottom vertices of the billboard
		//! \param topColor: stores the color of the top vertices
		//! \param bottomColor: stores the color of the bottom vertices
		virtual void getColor(video::SColor & topColor, video::SColor & bottomColor) const;

		//! Sets the size
		virtual void setSize(f32 height, f32 bottomEdgeWidth, f32 topEdgeWidth)
		{
			setSize(core::dimension2df(bottomEdgeWidth, height));
		}

		//! Gets the size
		virtual void getSize(f32& height, f32& bottomEdgeWidth, f32& topEdgeWidth) const
		{
			height = Size.Height;
			bottomEdgeWidth = Size.Width;
			topEdgeWidth = Size.Width;
		}

	private:
		
		//! Parses an hexadecimal color
		void parseColor(const std::string& color_string,
						video::SColor& color, f32& scaling); // :PATCH:
		
		core::stringw OldText; // :PATCH:
		core::stringw Text;
		f32 LineCount;
		gui::IGUIFontBitmap* Font;
		core::dimension2d<f32> Size;
		core::aabbox3d<f32> BBox;
		video::SMaterial Material;

		video::SColor TopColor;
		video::SColor BottomColor;
		
		bool Background;
		video::SColor BackgroundColor;
		video::SColor BorderColor;
		float Border;
		float XPadding;		
		float YPadding;		
		float XOffset;
		float YOffset;
		
		struct SSymbolInfo
		{
			u32 bufNo;
			f32 Width;
			f32 Height;
			f32 Kerning;
			u32 firstInd;
			u32 firstVert;
			f32 VerticalStep; // :PATCH:
			video::SColor TopColor; // :PATCH:
			video::SColor BottomColor; // :PATCH:
			f32 Scaling; // :PATCH:
		};

		core::array < SSymbolInfo > Symbol;

		SMesh *Mesh;
	};

} // end namespace scene
} // end namespace irr

#endif

