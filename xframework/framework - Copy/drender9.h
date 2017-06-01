#ifndef DRENDER9_H
#define DRENDER9_H

#define DX92DFVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)
#include <map>
#include <d3d9.h>
#include <d3dx9.h>

class DX9Vertex2D {
public:
	float         x, y, z, rhw;
	unsigned long argb;
};

#define DX93DFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE)
#define RT_LEFT DT_LEFT
#define RT_RIGHT DT_RIGHT
#define RT_TOP  DT_TOP
#define RT_NOCLIP DT_NOCLIP
#define RT_CENTER DT_CENTER

class DX9Vertex3D {
public:
	float         x, y, z;
	unsigned long argb;
};

class DX9Texture {
public:
	IDirect3DTexture9 *texture;
};

class DX9Font {
public:
	ID3DXFont *font;
	int        h;
};

class Sphere {
public:
	float x, y, z, r;
	_Color c;
};

struct MeshGeometry3D {
	_Vector<DX9Vertex3D> positions;
	_Vector<short>       indices;
};

class IcoSphereCreator {
public:
	MeshGeometry3D *Create(int recursionLevel);
	IcoSphereCreator(void);
	~IcoSphereCreator(void);

	MeshGeometry3D         geometry;
	int                    index, recLevel;
	std::map<__int64, int> middlePointIndexCache;

	int addVertex(D3DXVECTOR3 p);
	int getMiddlePoint(int p1, int p2);
};

class DX9Renderer {
public:
	DX9Renderer();

	void SetDevice(IDirect3DDevice9 *d);
	bool Initialize();
	void PreReset();
	void PostReset();
	void Release();

	void Begin();
	void End();
	void Flush();
	void SetColor(_Color c);

	DX9Texture *CreateBlankTexture(int w, int h);
	DX9Texture *CreateTextureFromMemory(void *buffer, size_t len);
	DX9Texture *CreateTextureFromFile(char *file);
	void        RenderTexture(DX9Texture *t, int x, int y);
	void        RenderTexture(DX9Texture *t, int x, int y, RECT *srcRect);
	// TODO: texture writing/modification funcz, see oshgui
	void        ReleaseTexture(DX9Texture *t);

	DX9Font *CreateNewFont(char *fontName, int size, unsigned int flags);
	void     ReleaseFont(DX9Font *f);
	void     RenderText(DX9Font *f, int x, int y, char *text);
	void     RenderText(DX9Font *f, int x, int y, char *text, _Color c);
	void     RenderCenteredText(DX9Font *f, int x, int y, char *text, _Color c);
	void     RenderTextf(DX9Font *f, int x, int y, char *format, ...);
	void     RenderTextf(DX9Font *f, int x, int y, _Color c, char *format, ...);
	void     RenderTextEx(DX9Font *f, RECT *r, char *text, unsigned int flags); //FW1_* flags
	void     RenderTextExf(DX9Font *f, RECT *r, unsigned int flags, char *format, ...);
	void     RenderTextEx(DX9Font *f, RECT *r, char *text, unsigned int flags, _Color c); //FW1_* flags
	void     CalcTextRect(DX9Font *f, char *text, RECT *r);

	void       Calculate2DCirclePoints(int numSides, int radius, DX9Vertex2D *v);
	void       Render2DCircle(int vCount, DX9Vertex2D *v, int x, int y);
	void       Render2DCircle(int vCount, DX9Vertex2D *v, int x, int y, _Color c);
	void       Render2DCircleEdges(int vCount, DX9Vertex2D *v, int x, int y);
	void       Render2DCircleEdges(int vCount, DX9Vertex2D *v, int x, int y, _Color c);
	void       Render2DTriangle(int x1, int y1, int x2, int y2, int x3, int y3);
	void       Render2DTriangle(int x1, int y1, int x2, int y2, int x3, int y3, _Color c);
	void       Render2DRect(int x, int y, int w, int h);
	void       Render2DRect(int x, int y, int w, int h, _Color c);
	void       Render2DRectEdges(int x, int y, int w, int h, int t);
	void       Render2DRectEdges(int x, int y, int w, int h, int t, _Color c);
	void       Render2DOutlinedRect(int x, int y, int w, int h, int t, _Color fillColor, _Color outlineColor);
	void       Render2DGradientRect(int dir, _Color start, _Color end, int x, int y, int w, int h);
	void       Render2DLine(int x1, int y1, int x2, int y2, int t);

	//void       Render3DLine(FVector3 *start, FVector3 *end);
	void       Render3DCube(float x, float y, float w, float h);
	LPD3DXMESH Create3DSphere(int slices);
	void       Render3DSphere(float x, float y, float z, float r);
	void       Render3DSphere(float x, float y, float z, float r, _Color c);

	void       AddVertex(int x, int y);
	void       AddVertex(int x, int y, _Color c);

	void         RenderIndexedTriangle(DX9Vertex2D *v, int vCount, short *i, int iCount);
	DX9Vertex2D *ReserveVertices(size_t num);
	short       *ReserveIndices(size_t num);
	// TODO: indexed/buffer/DOD interface to renderer, for very optimized
	// rendering

	IDirect3DDevice9       *device;
	IDirect3DVertexBuffer9 *vb;
	IDirect3DIndexBuffer9  *ib;
	ID3DXSprite            *s;
	ID3DXMesh              *sphereMesh;
	IcoSphereCreator        icoSphereCreator;
	_Vector<DX9Font>        fonts;
	_Vector<DX9Texture>     textures;
	_Vector<Sphere>         spheres;
	int                     sW, sH;
	_Color                  currentColor;
	int                     numVertices, numIndices, numLines;
	DX9Vertex2D             vertices[MAX_VERTICES], lines[MAX_LINES];
	short                   indices[MAX_INDICES];
};

extern DX9Renderer render9;

#endif
