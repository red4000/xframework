#ifndef DRENDER_H
#define DRENDER_H

#define SAFE_LOST(x) if (x != NULL) { x->OnLostDevice(); x = NULL; }
#define SAFE_RESET(x) if (x != NULL) { x->OnResetDevice(); x = NULL; }
#define SAFE_RELEASE(x) if (x != NULL) { x->Release(); x = NULL; }

#define MAX_VERTICES 2048
#define MAX_INDICES  (2048 * 2)
#define MAX_LINES    128
#include "_vector.h"

#define FL_BOLD    0x1
#define FL_ITALIC  0x2
#define FL_OUTLINE 0x4

enum { // gradient directions
	DIR_UPDOWN,
	DIR_LEFTRIGHT,
	DIR_LEFTDOWN
};

typedef unsigned long _Color;

/*
   class BaseTexture {};
   class BaseFont {};

   class BaseRenderer {
   public:
		bool Initialize();
		void PreReset();
		void PostReset();
		void Release();

		void Begin();
		void End();
		void Flush();
		void SetColor(_Color c);

		BaseTexture *CreateBlankTexture(int w, int h);
		BaseTexture *CreateTextureFromMemory(void *buffer, size_t len);
		BaseTexture *CreateTextureFromFile(char *file);
		void RenderTexture(BaseTexture *t, int x, int y, int w, int h);
		void ReleaseTexture(BaseTexture *t);

		BaseFont *CreateNewFont(char *fontName, int size, unsigned int flags);
		void ReleaseFont(BaseFont *f);
		void RenderText(BaseFont *f, int x, int y, char *text);
		void RenderText(BaseFont *f, int x, int y, char *text, _Color c);
		void RenderTextEx(BaseFont *f, RECT *r, char *text, unsigned int flags); //DT_* flags, like DT_LEFT
		void RenderTextEx(BaseFont *f, RECT *r, char *text, unsigned int flags, _Color c); //DT_* flags, like DT_LEFT
		void CalcTextRect(BaseFont *f, char *text, RECT *r);

		void Render2DTriangle(int x1, int y1, int x2, int y2, int x3, int y3);
		void Render2DTriangle(int x1, int y1, int x2, int y2, int x3, int y3, _Color c);
		void Render2DRect(int x, int y, int w, int h);
		void Render2DRect(int x, int y, int w, int h, _Color c);
		void Render2DGradientRect(int dir, _Color start, _Color end, int x, int y, int w, int h);
		void Render2DLine(int x1, int y1, int x2, int y2, int t);

   private:
		DX9Renderer dx9render;
		DX10Renderer dx10render;
   };
 */

#define USE_D3D9
//#define USE_D3D10
//#define USE_D3D11

#include <d3d9.h>
#include <d3dx9.h>

#ifdef USE_D3D9
#include "drender9.h"
#define Texture  DX9Texture
#define Font     DX9Font
#define Renderer DX9Renderer
#define Vertex   DX9Vertex2D
#define render   render9
#define INIT     InitDX9
#define CLEANUP  CleanupDX9
#define RENDER   RenderDX9
#endif
#ifdef USE_D3D10
#include "drender10.h"
#define Texture  DX10Texture
#define Font     DX10Font
#define Renderer DX10Renderer
#define Vertex   DX10Vertex3D
#define render   render10
#define INIT     InitDX10
#define CLEANUP  CleanupDX10
#define RENDER   RenderDX10
#endif
#ifdef USE_D3D11
#include "drender11.h"
#define Texture  DX11Texture
#define Font     DX11Font
#define Renderer DX11Renderer
#define Vertex   DX11Vertex3D
#define render   render11
#define INIT     InitDX11
#define CLEANUP  CleanupDX11
#define RENDER   RenderDX11
#endif
#endif
