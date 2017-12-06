// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CTextSceneNode.h"
#include "ISceneManager.h"
#include "IVideoDriver.h"
#include "ICameraSceneNode.h"
#include "IGUISpriteBank.h"
#include "SMeshBuffer.h"
#include "os.h"
#include <map>
#include <stdlib.h>

namespace irr
{
namespace scene
{


//! constructor
CTextSceneNode::CTextSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
			gui::IGUIFont* font, scene::ISceneCollisionManager* coll,
			const core::vector3df& position, const wchar_t* text,
			video::SColor color)
	: ITextSceneNode(parent, mgr, id, position), Text(text), Color(color),
		Font(font), Coll(coll)

{
	#ifdef _DEBUG
	setDebugName("CTextSceneNode");
	#endif

	if (Font)
		Font->grab();

	setAutomaticCulling(scene::EAC_OFF);
}

//! destructor
CTextSceneNode::~CTextSceneNode()
{
	if (Font)
		Font->drop();
}

void CTextSceneNode::OnRegisterSceneNode()
{
	if (IsVisible)
		SceneManager->registerNodeForRendering(this, ESNRP_TRANSPARENT);

	ISceneNode::OnRegisterSceneNode();
}

//! renders the node.
void CTextSceneNode::render()
{
	if (!Font || !Coll)
		return;

	core::position2d<s32> pos = Coll->getScreenCoordinatesFrom3DPosition(getAbsolutePosition(),
		SceneManager->getActiveCamera());

	core::rect<s32> r(pos, core::dimension2d<s32>(1,1));
	Font->draw(Text.c_str(), r, Color, true, true);
}


//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CTextSceneNode::getBoundingBox() const
{
	return Box;
}

//! sets the text string
void CTextSceneNode::setText(const wchar_t* text)
{
	Text = text;
}


//! sets the color of the text
void CTextSceneNode::setTextColor(video::SColor color)
{
	Color = color;
}


//!--------------------------------- CBillboardTextSceneNode ----------------------------------------------


//! constructor
CBillboardTextSceneNode::CBillboardTextSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
		gui::IGUIFont* font,const wchar_t* text,
		const core::vector3df& position, const core::dimension2d<f32>& size,
		const video::SColor colorTop, const video::SColor bottomColor,
		const bool background, const video::SColor & backgroundColor, 
		const video::SColor & borderColor, const f32 border,
		const f32 xPadding, const f32 yPadding,
		const f32 xOffset, const f32 yOffset)
	: IBillboardTextSceneNode(parent, mgr, id, position),
		LineCount(1), Font(0), TopColor(colorTop), BottomColor(bottomColor), 
		Background(background), BackgroundColor(backgroundColor), BorderColor(borderColor), 
		Border(border), XPadding(xPadding), YPadding(yPadding), 
		XOffset(xOffset), YOffset(yOffset), Mesh(0) // :PATCH:
{
	#ifdef _DEBUG
	setDebugName("CBillboardTextSceneNode");
	#endif

	Material.MaterialType = video::EMT_TRANSPARENT_ALPHA_CHANNEL;
	Material.MaterialTypeParam = 1.f / 255.f;
	Material.BackfaceCulling = false;
	Material.Lighting = false;
	Material.ZBuffer = video::ECFN_LESSEQUAL;
	Material.ZWriteEnable = false;

	if (font)
	{
		// doesn't support other font types
		if (font->getType() == gui::EGFT_BITMAP)
		{
			Font = (gui::IGUIFontBitmap*)font;
			Font->grab();

			// mesh with one buffer per texture
			Mesh = new SMesh();
			for (u32 i=0; i<Font->getSpriteBank()->getTextureCount(); ++i)
			{
				SMeshBuffer *mb = new SMeshBuffer();
				mb->Material = Material;
				mb->Material.setTexture(0, Font->getSpriteBank()->getTexture(i));
				Mesh->addMeshBuffer(mb);
				mb->drop();
			}
		}
		else
		{
			os::Printer::log("Sorry, CBillboardTextSceneNode does not support this font type", ELL_INFORMATION);
		}
	}

	setSize(size);
	setText(text);

	setAutomaticCulling ( scene::EAC_BOX );
}



CBillboardTextSceneNode::~CBillboardTextSceneNode()
{
	if (Font)
		Font->drop();

	if (Mesh)
		Mesh->drop();
}


//! sets the text string
void CBillboardTextSceneNode::setText(const wchar_t* text)
{
	if ( !Mesh )
		return;

	if (OldText == text)
		return;

	OldText = text;
	Text = "";
	core::array<s32> charLineBreaks;
	core::array<video::SColor> charTopColors;
	core::array<video::SColor> charBottomColors;
	core::array<f32> charScalings;
	LineCount = 1.0f;
	f32 lineBreaks = 0.0f;
	video::SColor topColor = TopColor;
	video::SColor bottomColor = BottomColor;
	f32 scaling = 1.0f;

	for (const wchar_t* c=text; *c; ++c)
	{
		if (*c == L'\n')
		{
			LineCount += 1.0f;
			lineBreaks += 1.0f;
		}
		else
		{
			if (*c == L'{')
			{
				++c;
				std::string color_string;
				while (*c)
				{
					if (*c == L'}')
						break;
					color_string += char(*c);
					++c;
				}
				std::size_t slash = color_string.find('/');
				if (slash == std::string::npos)
				{
					parseColor(color_string, topColor, scaling);
					parseColor(color_string, bottomColor, scaling);
				}
				else
				{
					parseColor(color_string.substr(0,slash), topColor, scaling);
					parseColor(color_string.substr(slash + 1), bottomColor, scaling);
				}
				continue;
			}
			else if (*c == L'\\' && c[1])
			{
				++c;
			}
			Text += *c;
			charLineBreaks.push_back( lineBreaks );
			charTopColors.push_back( topColor );
			charBottomColors.push_back( bottomColor );
			charScalings.push_back( scaling);
			lineBreaks = 0.0f;
		}
	}
	
	Symbol.clear();

	// clear mesh
	for (u32 j=0; j < Mesh->getMeshBufferCount(); ++j)
	{
		((SMeshBuffer*)Mesh->getMeshBuffer(j))->Indices.clear();
		((SMeshBuffer*)Mesh->getMeshBuffer(j))->Vertices.clear();
	}

	if (!Font)
		return;

	const core::array< core::rect<s32> > &sourceRects = Font->getSpriteBank()->getPositions();
	const core::array< gui::SGUISprite > &sprites = Font->getSpriteBank()->getSprites();

	f32 dim[2];
	f32 tex[4];

	u32 i;
	for ( i = 0; i != Text.size (); ++i )
	{
		SSymbolInfo info;

		u32 spriteno = Font->getSpriteNoFromChar( &Text.c_str()[i] );
		u32 rectno = sprites[spriteno].Frames[0].rectNumber;
		u32 texno = sprites[spriteno].Frames[0].textureNumber;

		dim[0] = core::reciprocal ( (f32) Font->getSpriteBank()->getTexture(texno)->getSize().Width );
		dim[1] = core::reciprocal ( (f32) Font->getSpriteBank()->getTexture(texno)->getSize().Height );

		const core::rect<s32>& s = sourceRects[rectno];

		// add space for letter to buffer
		SMeshBuffer* buf = (SMeshBuffer*)Mesh->getMeshBuffer(texno);
		u32 firstInd = buf->Indices.size();
		u32 firstVert = buf->Vertices.size();
		buf->Indices.set_used(firstInd + 6);
		buf->Vertices.set_used(firstVert + 4);

		tex[0] = (s.LowerRightCorner.X * dim[0]) + 0.5f*dim[0]; // half pixel
		tex[1] = (s.LowerRightCorner.Y * dim[1]) + 0.5f*dim[1];
		tex[2] = (s.UpperLeftCorner.Y  * dim[1]) - 0.5f*dim[1];
		tex[3] = (s.UpperLeftCorner.X  * dim[0]) - 0.5f*dim[0];

		buf->Vertices[firstVert+0].TCoords.set(tex[0], tex[1]);
		buf->Vertices[firstVert+1].TCoords.set(tex[0], tex[2]);
		buf->Vertices[firstVert+2].TCoords.set(tex[3], tex[2]);
		buf->Vertices[firstVert+3].TCoords.set(tex[3], tex[1]);

		buf->Vertices[firstVert+0].Color = charBottomColors[i];
		buf->Vertices[firstVert+3].Color = charBottomColors[i];
		buf->Vertices[firstVert+1].Color = charTopColors[i];
		buf->Vertices[firstVert+2].Color = charTopColors[i];

		buf->Indices[firstInd+0] = (u16)firstVert+0;
		buf->Indices[firstInd+1] = (u16)firstVert+2;
		buf->Indices[firstInd+2] = (u16)firstVert+1;
		buf->Indices[firstInd+3] = (u16)firstVert+0;
		buf->Indices[firstInd+4] = (u16)firstVert+3;
		buf->Indices[firstInd+5] = (u16)firstVert+2;

		wchar_t *tp = 0;
		if (i>0)
			tp = &Text[i-1];

		info.Width = (f32)s.getWidth();
		info.Height = (f32)s.getHeight();
		info.bufNo = texno;
		info.Kerning = (f32)Font->getKerningWidth(&Text[i], tp);
		info.firstInd = firstInd;
		info.firstVert = firstVert;
		info.VerticalStep = charLineBreaks[i];
		info.TopColor = charTopColors[i];
		info.BottomColor = charBottomColors[i];
		info.Scaling = charScalings[i];

		Symbol.push_back(info);
	}
	
	resize();
}

//! resize the billboard
void CBillboardTextSceneNode::resize()
{
	if (!IsVisible || !Font || !Mesh)
		return;

	ICameraSceneNode* camera = SceneManager->getActiveCamera();
	if (!camera)
		return;

	// get text width
	f32 textLength = 0.0f;
	f32 maxTextLength = 0.0f;
	f32 charHeight = 0.0f;
	
	u32 i;
	for(i=0; i!=Symbol.size(); ++i)
	{
		SSymbolInfo &info = Symbol[i];
		
		if ( info.VerticalStep > 0.0f )
			textLength = 0.0f;
			
		textLength += info.Kerning + info.Width;
		
		if ( textLength > maxTextLength )
			maxTextLength = textLength;
			
		if ( info.Height > charHeight )
			charHeight = info.Height;
	}
	
	textLength = maxTextLength;
	
	if ( textLength <= 0.0f )
		textLength = 1.0f;
		
	if ( charHeight <= 0.0f )
		charHeight = 1.0f;
		
	Size.Width = (Size.Height / LineCount) * textLength / charHeight;
		
	//const core::matrix4 &m = camera->getViewFrustum()->Matrices[ video::ETS_VIEW ];

	// make billboard look to camera
	core::vector3df line_pos = getAbsolutePosition();

	core::vector3df campos = camera->getAbsolutePosition();
	core::vector3df target = camera->getTarget();
	core::vector3df up = camera->getUpVector();
	core::vector3df view = target - campos;
	view.normalize();

	core::vector3df horizontal = up.crossProduct(view);
	if ( horizontal.getLength() == 0 )
	{
		horizontal.set(up.Y,up.X,up.Z);
	}

	horizontal.normalize();
	
	core::vector3df line_horizontal = horizontal;
	
	horizontal *= 0.5f * Size.Width;

	core::vector3df vertical = horizontal.crossProduct(view);
	vertical.normalize();
	
	core::vector3df line_vertical = vertical;
	line_vertical *= 0.5f * Size.Height / LineCount;
	
	vertical *= 0.5f * Size.Height;
	
	view *= -1.0f;

	// center text
	line_pos += line_horizontal * (Size.Width * -0.5f);
	
	if ( LineCount > 1 )
		line_pos += line_vertical * - (LineCount - 1.0f);
	
	line_pos += horizontal * (XOffset * charHeight / textLength);
	line_pos += vertical * (2.0 * YOffset / LineCount);
	
	core::vector3df pos = line_pos;

	for ( i = 0; i!= Symbol.size(); ++i )
	{
		SSymbolInfo &info = Symbol[i];
		
		if ( info.VerticalStep > 0.0f )
		{
			line_pos += 2.0f * line_vertical * info.VerticalStep;
			pos = line_pos;
		}
		
		f32 infw = info.Width / textLength;
		f32 infk = info.Kerning / textLength;
		f32 w = (Size.Width * infw * 0.5f);
		pos += line_horizontal * w;

		SMeshBuffer* buf = (SMeshBuffer*)Mesh->getMeshBuffer(info.bufNo);

		buf->Vertices[info.firstVert+0].Normal = view;
		buf->Vertices[info.firstVert+1].Normal = view;
		buf->Vertices[info.firstVert+2].Normal = view;
		buf->Vertices[info.firstVert+3].Normal = view;

		buf->Vertices[info.firstVert+0].Pos = pos + (line_horizontal * w) + line_vertical;
		buf->Vertices[info.firstVert+1].Pos = pos + (line_horizontal * w) - line_vertical;
		buf->Vertices[info.firstVert+2].Pos = pos - (line_horizontal * w) - line_vertical;
		buf->Vertices[info.firstVert+3].Pos = pos - (line_horizontal * w) + line_vertical;
		
		buf->Vertices[info.firstVert+0].Color = info.BottomColor;
		buf->Vertices[info.firstVert+3].Color = info.BottomColor;
		buf->Vertices[info.firstVert+1].Color = info.TopColor;
		buf->Vertices[info.firstVert+2].Color = info.TopColor;

		pos += line_horizontal * (Size.Width*infk + w);
	}

	// make bounding box

	for (i=0; i< Mesh->getMeshBufferCount() ; ++i)
		Mesh->getMeshBuffer(i)->recalculateBoundingBox();
	Mesh->recalculateBoundingBox();

	BBox = Mesh->getBoundingBox();
	core::matrix4 mat( getAbsoluteTransformation(), core::matrix4::EM4CONST_INVERSE );
	mat.transformBoxEx(BBox);
}

//! pre render event
void CBillboardTextSceneNode::OnAnimate(u32 timeMs)
{
	ISceneNode::OnAnimate(timeMs);

	resize();
}


void CBillboardTextSceneNode::OnRegisterSceneNode()
{
	SceneManager->registerNodeForRendering(this, ESNRP_TRANSPARENT);
	ISceneNode::OnRegisterSceneNode();
}


//! render background
void CBillboardTextSceneNode::renderBackground()
{
	video::IVideoDriver* driver = SceneManager->getVideoDriver();
	ICameraSceneNode* camera = SceneManager->getActiveCamera();

	if (!camera || !driver)
		return;
		
	video::S3DVertex vertices[4];
	u16 indices[6];
	f32 border = Size.Height / LineCount * Border;
	f32 x_padding = Size.Height / LineCount * XPadding;
	f32 y_padding = Size.Height / LineCount * YPadding;
	f32 width = Size.Width + x_padding * 2;
	f32 height = Size.Height + y_padding * 2;
	
	indices[0] = 0;
	indices[1] = 2;
	indices[2] = 1;
	indices[3] = 0;
	indices[4] = 3;
	indices[5] = 2;

	vertices[0].TCoords.set(1.0f, 1.0f);
	vertices[0].Color = BackgroundColor;

	vertices[1].TCoords.set(1.0f, 0.0f);
	vertices[1].Color = BackgroundColor;

	vertices[2].TCoords.set(0.0f, 0.0f);
	vertices[2].Color = BackgroundColor;

	vertices[3].TCoords.set(0.0f, 1.0f);
	vertices[3].Color = BackgroundColor;
	
	// make billboard look to camera

	core::vector3df pos = getAbsolutePosition();

	core::vector3df campos = camera->getAbsolutePosition();
	core::vector3df target = camera->getTarget();
	core::vector3df up = camera->getUpVector();
	core::vector3df view = target - campos;
	view.normalize();

	core::vector3df horizontal = up.crossProduct(view);
	if ( horizontal.getLength() == 0 )
	{
		horizontal.set(up.Y,up.X,up.Z);
	}
	horizontal.normalize();
	core::vector3df borderHorizontal = horizontal * border;
	horizontal *= 0.5f * width;

	// pointing down!
	core::vector3df vertical = horizontal.crossProduct(view);
	vertical.normalize();
	core::vector3df borderVertical = vertical * border;
	vertical *= 0.5f * height;

	view *= -1.0f;

	for (s32 i=0; i<4; ++i)
		vertices[i].Normal = view;

	/* Vertices are:
	2--1
	|\ |
	| \|
	3--0
	*/
	
	driver->setTransform(video::ETS_WORLD, core::IdentityMatrix);
	driver->setMaterial(Material);
	
	// background
	vertices[2].Pos = pos - horizontal - vertical;
	vertices[1].Pos = pos + horizontal - vertical;
	vertices[3].Pos = pos - horizontal + vertical;
	vertices[0].Pos = pos + horizontal + vertical;
	driver->drawIndexedTriangleList(vertices, 4, indices, 2);
	
	vertices[0].Color = BorderColor;
	vertices[1].Color = BorderColor;
	vertices[2].Color = BorderColor;
	vertices[3].Color = BorderColor;
	
	// top border
	vertices[2].Pos = pos - horizontal - borderHorizontal - vertical - borderVertical;
	vertices[1].Pos = pos + horizontal + borderHorizontal - vertical - borderVertical;
	vertices[3].Pos = pos - horizontal - borderHorizontal - vertical;
	vertices[0].Pos = pos + horizontal + borderHorizontal - vertical;
	driver->drawIndexedTriangleList(vertices, 4, indices, 2);
	
	// bottom border
	vertices[2].Pos = pos - horizontal - borderHorizontal + vertical;
	vertices[1].Pos = pos + horizontal + borderHorizontal + vertical;
	vertices[3].Pos = pos - horizontal - borderHorizontal + vertical + borderVertical;
	vertices[0].Pos = pos + horizontal + borderHorizontal + vertical + borderVertical;
	driver->drawIndexedTriangleList(vertices, 4, indices, 2);
	
	// left border
	vertices[2].Pos = pos - horizontal - borderHorizontal - vertical;
	vertices[1].Pos = pos - horizontal - vertical;
	vertices[3].Pos = pos - horizontal - borderHorizontal + vertical;
	vertices[0].Pos = pos - horizontal + vertical;
	driver->drawIndexedTriangleList(vertices, 4, indices, 2);
	
	// right border
	vertices[2].Pos = pos + horizontal - vertical;
	vertices[1].Pos = pos + horizontal + borderHorizontal - vertical;
	vertices[3].Pos = pos + horizontal + vertical;
	vertices[0].Pos = pos + horizontal + borderHorizontal + vertical;
	driver->drawIndexedTriangleList(vertices, 4, indices, 2);
}


//! render
void CBillboardTextSceneNode::render()
{
	if ( !Mesh )
		return;

	if (Background)
		renderBackground();
	
	video::IVideoDriver* driver = SceneManager->getVideoDriver();

	// draw
	core::matrix4 mat;
	driver->setTransform(video::ETS_WORLD, mat);

	for (u32 i = 0; i < Mesh->getMeshBufferCount(); ++i)
	{
		driver->setMaterial(Mesh->getMeshBuffer(i)->getMaterial());
		driver->drawMeshBuffer(Mesh->getMeshBuffer(i));
	}

	if ( DebugDataVisible & scene::EDS_BBOX )
	{
		driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
		video::SMaterial m;
		m.Lighting = false;
		driver->setMaterial(m);
		driver->draw3DBox(BBox, video::SColor(0,208,195,152));
	}
}


//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CBillboardTextSceneNode::getBoundingBox() const
{
	return BBox;
}


//! sets the size of the billboard
void CBillboardTextSceneNode::setSize(const core::dimension2d<f32>& size)
{
	Size = size;

	if (Size.Width == 0.0f)
		Size.Width = 1.0f;

	if (Size.Height == 0.0f )
		Size.Height = 1.0f;

	//f32 avg = (size.Width + size.Height)/6;
	//BBox.MinEdge.set(-avg,-avg,-avg);
	//BBox.MaxEdge.set(avg,avg,avg);
}


video::SMaterial& CBillboardTextSceneNode::getMaterial(u32 i)
{
	if (Mesh && Mesh->getMeshBufferCount() > i )
		return Mesh->getMeshBuffer(i)->getMaterial();
	else
		return Material;
}


//! returns amount of materials used by this scene node.
u32 CBillboardTextSceneNode::getMaterialCount() const
{
	if (Mesh)
		return Mesh->getMeshBufferCount();
	else
		return 0;
}


//! gets the size of the billboard
const core::dimension2d<f32>& CBillboardTextSceneNode::getSize() const
{
	return Size;
}


//! sets the color of the text
void CBillboardTextSceneNode::setTextColor(video::SColor color)
{
	TopColor = color;
	BottomColor = color;
}

//! Set the color of all vertices of the billboard
//! \param overallColor: the color to set
void CBillboardTextSceneNode::setColor(const video::SColor & overallColor)
{
	if ( !Mesh )
		return;

	for ( u32 i = 0; i != Text.size (); ++i )
	{
		const SSymbolInfo &info = Symbol[i];
		SMeshBuffer* buf = (SMeshBuffer*)Mesh->getMeshBuffer(info.bufNo);
		buf->Vertices[info.firstVert+0].Color = overallColor;
		buf->Vertices[info.firstVert+1].Color = overallColor;
		buf->Vertices[info.firstVert+2].Color = overallColor;
		buf->Vertices[info.firstVert+3].Color = overallColor;
	}
}


//! Set the color of the top and bottom vertices of the billboard
//! \param topColor: the color to set the top vertices
//! \param bottomColor: the color to set the bottom vertices
void CBillboardTextSceneNode::setColor(const video::SColor & topColor, const video::SColor & bottomColor)
{
	if ( !Mesh )
		return;

	BottomColor = bottomColor;
	TopColor = topColor;
	for ( u32 i = 0; i != Text.size (); ++i )
	{
		const SSymbolInfo &info = Symbol[i];
		SMeshBuffer* buf = (SMeshBuffer*)Mesh->getMeshBuffer(info.bufNo);
		buf->Vertices[info.firstVert+0].Color = BottomColor;
		buf->Vertices[info.firstVert+3].Color = BottomColor;
		buf->Vertices[info.firstVert+1].Color = TopColor;
		buf->Vertices[info.firstVert+2].Color = TopColor;
	}
}


//! Gets the color of the top and bottom vertices of the billboard
//! \param topColor: stores the color of the top vertices
//! \param bottomColor: stores the color of the bottom vertices
void CBillboardTextSceneNode::getColor(video::SColor & topColor, video::SColor & bottomColor) const
{
	topColor = TopColor;
	bottomColor = BottomColor;
}

//! Parses an hexadecimal color
void CBillboardTextSceneNode::parseColor(const std::string& color_string,
	video::SColor& color, f32& scaling) // :PATCH:
{
	static std::map<std::string, u32> colors;
	
	if (colors.size() == 0)
	{
		colors["aliceblue"]              = 0xf0f8ff;
		colors["antiquewhite"]           = 0xfaebd7;
		colors["aqua"]                   = 0x00ffff;
		colors["aquamarine"]             = 0x7fffd4;
		colors["azure"]                  = 0xf0ffff;
		colors["beige"]                  = 0xf5f5dc;
		colors["bisque"]                 = 0xffe4c4;
		colors["black"]                  = 00000000;
		colors["blanchedalmond"]         = 0xffebcd;
		colors["blue"]                   = 0x0000ff;
		colors["blueviolet"]             = 0x8a2be2;
		colors["brown"]                  = 0xa52a2a;
		colors["burlywood"]              = 0xdeb887;
		colors["cadetblue"]              = 0x5f9ea0;
		colors["chartreuse"]             = 0x7fff00;
		colors["chocolate"]              = 0xd2691e;
		colors["coral"]                  = 0xff7f50;
		colors["cornflowerblue"]         = 0x6495ed;
		colors["cornsilk"]               = 0xfff8dc;
		colors["crimson"]                = 0xdc143c;
		colors["cyan"]                   = 0x00ffff;
		colors["darkblue"]               = 0x00008b;
		colors["darkcyan"]               = 0x008b8b;
		colors["darkgoldenrod"]          = 0xb8860b;
		colors["darkgray"]               = 0xa9a9a9;
		colors["darkgreen"]              = 0x006400;
		colors["darkgrey"]               = 0xa9a9a9;
		colors["darkkhaki"]              = 0xbdb76b;
		colors["darkmagenta"]            = 0x8b008b;
		colors["darkolivegreen"]         = 0x556b2f;
		colors["darkorange"]             = 0xff8c00;
		colors["darkorchid"]             = 0x9932cc;
		colors["darkred"]                = 0x8b0000;
		colors["darksalmon"]             = 0xe9967a;
		colors["darkseagreen"]           = 0x8fbc8f;
		colors["darkslateblue"]          = 0x483d8b;
		colors["darkslategray"]          = 0x2f4f4f;
		colors["darkslategrey"]          = 0x2f4f4f;
		colors["darkturquoise"]          = 0x00ced1;
		colors["darkviolet"]             = 0x9400d3;
		colors["deeppink"]               = 0xff1493;
		colors["deepskyblue"]            = 0x00bfff;
		colors["dimgray"]                = 0x696969;
		colors["dimgrey"]                = 0x696969;
		colors["dodgerblue"]             = 0x1e90ff;
		colors["firebrick"]              = 0xb22222;
		colors["floralwhite"]            = 0xfffaf0;
		colors["forestgreen"]            = 0x228b22;
		colors["fuchsia"]                = 0xff00ff;
		colors["gainsboro"]              = 0xdcdcdc;
		colors["ghostwhite"]             = 0xf8f8ff;
		colors["gold"]                   = 0xffd700;
		colors["goldenrod"]              = 0xdaa520;
		colors["gray"]                   = 0x808080;
		colors["green"]                  = 0x008000;
		colors["greenyellow"]            = 0xadff2f;
		colors["grey"]                   = 0x808080;
		colors["honeydew"]               = 0xf0fff0;
		colors["hotpink"]                = 0xff69b4;
		colors["indianred"]              = 0xcd5c5c;
		colors["indigo"]                 = 0x4b0082;
		colors["ivory"]                  = 0xfffff0;
		colors["khaki"]                  = 0xf0e68c;
		colors["lavender"]               = 0xe6e6fa;
		colors["lavenderblush"]          = 0xfff0f5;
		colors["lawngreen"]              = 0x7cfc00;
		colors["lemonchiffon"]           = 0xfffacd;
		colors["lightblue"]              = 0xadd8e6;
		colors["lightcoral"]             = 0xf08080;
		colors["lightcyan"]              = 0xe0ffff;
		colors["lightgoldenrodyellow"]   = 0xfafad2;
		colors["lightgray"]              = 0xd3d3d3;
		colors["lightgreen"]             = 0x90ee90;
		colors["lightgrey"]              = 0xd3d3d3;
		colors["lightpink"]              = 0xffb6c1;
		colors["lightsalmon"]            = 0xffa07a;
		colors["lightseagreen"]          = 0x20b2aa;
		colors["lightskyblue"]           = 0x87cefa;
		colors["lightslategray"]         = 0x778899;
		colors["lightslategrey"]         = 0x778899;
		colors["lightsteelblue"]         = 0xb0c4de;
		colors["lightyellow"]            = 0xffffe0;
		colors["lime"]                   = 0x00ff00;
		colors["limegreen"]              = 0x32cd32;
		colors["linen"]                  = 0xfaf0e6;
		colors["magenta"]                = 0xff00ff;
		colors["maroon"]                 = 0x800000;
		colors["mediumaquamarine"]       = 0x66cdaa;
		colors["mediumblue"]             = 0x0000cd;
		colors["mediumorchid"]           = 0xba55d3;
		colors["mediumpurple"]           = 0x9370db;
		colors["mediumseagreen"]         = 0x3cb371;
		colors["mediumslateblue"]        = 0x7b68ee;
		colors["mediumspringgreen"]      = 0x00fa9a;
		colors["mediumturquoise"]        = 0x48d1cc;
		colors["mediumvioletred"]        = 0xc71585;
		colors["midnightblue"]           = 0x191970;
		colors["mintcream"]              = 0xf5fffa;
		colors["mistyrose"]              = 0xffe4e1;
		colors["moccasin"]               = 0xffe4b5;
		colors["navajowhite"]            = 0xffdead;
		colors["navy"]                   = 0x000080;
		colors["oldlace"]                = 0xfdf5e6;
		colors["olive"]                  = 0x808000;
		colors["olivedrab"]              = 0x6b8e23;
		colors["orange"]                 = 0xffa500;
		colors["orangered"]              = 0xff4500;
		colors["orchid"]                 = 0xda70d6;
		colors["palegoldenrod"]          = 0xeee8aa;
		colors["palegreen"]              = 0x98fb98;
		colors["paleturquoise"]          = 0xafeeee;
		colors["palevioletred"]          = 0xdb7093;
		colors["papayawhip"]             = 0xffefd5;
		colors["peachpuff"]              = 0xffdab9;
		colors["peru"]                   = 0xcd853f;
		colors["pink"]                   = 0xffc0cb;
		colors["plum"]                   = 0xdda0dd;
		colors["powderblue"]             = 0xb0e0e6;
		colors["purple"]                 = 0x800080;
		colors["red"]                    = 0xff0000;
		colors["rosybrown"]              = 0xbc8f8f;
		colors["royalblue"]              = 0x4169e1;
		colors["saddlebrown"]            = 0x8b4513;
		colors["salmon"]                 = 0xfa8072;
		colors["sandybrown"]             = 0xf4a460;
		colors["seagreen"]               = 0x2e8b57;
		colors["seashell"]               = 0xfff5ee;
		colors["sienna"]                 = 0xa0522d;
		colors["silver"]                 = 0xc0c0c0;
		colors["skyblue"]                = 0x87ceeb;
		colors["slateblue"]              = 0x6a5acd;
		colors["slategray"]              = 0x708090;
		colors["slategrey"]              = 0x708090;
		colors["snow"]                   = 0xfffafa;
		colors["springgreen"]            = 0x00ff7f;
		colors["steelblue"]              = 0x4682b4;
		colors["tan"]                    = 0xd2b48c;
		colors["teal"]                   = 0x008080;
		colors["thistle"]                = 0xd8bfd8;
		colors["tomato"]                 = 0xff6347;
		colors["turquoise"]              = 0x40e0d0;
		colors["violet"]                 = 0xee82ee;
		colors["wheat"]                  = 0xf5deb3;
		colors["white"]                  = 0xffffff;
		colors["whitesmoke"]             = 0xf5f5f5;
		colors["yellow"]                 = 0xffff00;
		colors["yellowgreen"]            = 0x9acd32;
	}
	
	u32 rgbcolor = 0;
	
	if (color_string.length() > 0)
	{
		const char* c=color_string.c_str();
		
		if (*c >= '0' && *c <= '9')
		{
			scaling = atof(c);
			return;
		}
		else if (*c == '#')
		{
			++c;
	
			while (*c)
			{
				u32 digit = 0;
				if (*c >= '0' && *c <= '9')
					digit = *c - '0';
				else if (*c >= 'a' && *c <= 'f')
					digit = (*c - 'a') + 10;
				else if (*c >= 'A' && *c <= 'F')
					digit = (*c - 'A') + 10;
					
				rgbcolor = (rgbcolor<<4) | digit;
				++c;
			}
			
			if (color_string.length() <= 5)
			{
				rgbcolor = ((rgbcolor&0xF000)<<16) | ((rgbcolor&0xF000)<<12) | 
					((rgbcolor&0xF00)<<12) | ((rgbcolor&0xF00)<<8) | 
					((rgbcolor&0x0F0)<<8) | ((rgbcolor&0x0F0)<<4) | 
					((rgbcolor&0x00F)<<4) | (rgbcolor&0x00F);
					
				if (color_string.length() <= 3)
					rgbcolor |= 0xFF000000;
			}
			else if (color_string.length() <= 6)
					rgbcolor |= 0xFF000000;
		}
		else
		{	
			std::map<const std::string, u32>::const_iterator found_color;
			found_color = colors.find(color_string);
			
			if (found_color != colors.end())
				rgbcolor = 0xFF000000 | found_color->second;
		}
	}

	color.color = rgbcolor;
}
} // end namespace scene
} // end namespace irr

