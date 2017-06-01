#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <map>
#include <algorithm>
#include "drender.h"
#include "drender9.h"
#include "vmath.h"

DX9Renderer render9;

DX9Renderer::DX9Renderer() {
	for (size_t i = 0; i < MAX_VERTICES; i++) {
		vertices[i].z   = 0.0f;
		vertices[i].rhw = 1.0f;
	}

	for (size_t i = 0; i < MAX_LINES; i++) {
		lines[i].z   = 0.0f;
		lines[i].rhw = 1.0f;
	}

	numVertices = numIndices = numLines = 0;
}

void DX9Renderer::SetDevice(IDirect3DDevice9 *d) {
	device = d;
}

bool DX9Renderer::Initialize() {
	LPDIRECT3DSURFACE9 backBuffer;
	D3DSURFACE_DESC    desc;
	device->GetRenderTarget(0, &backBuffer);
	backBuffer->GetDesc(&desc);
	sW = desc.Width;
	sH = desc.Height;

	if (SUCCEEDED(D3DXCreateSprite(device, &s))) {
		icoSphereCreator.Create(3);
		sphereMesh = Create3DSphere(32);
		if (SUCCEEDED(device->CreateVertexBuffer(MAX_VERTICES * sizeof(DX9Vertex2D), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &vb, NULL))) {
			if (SUCCEEDED(device->CreateIndexBuffer(MAX_INDICES * sizeof(short), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &ib, NULL))) {
				return true;
			}
		}
	}

	return false;
}

void DX9Renderer::PreReset() {
	SAFE_LOST(s);
}

void DX9Renderer::PostReset() {
	SAFE_RESET(s);
}

void DX9Renderer::Release() {
	for (auto it = textures.begin(); it != textures.end(); it++) {
		SAFE_RELEASE(it->texture);
	}

	for (auto it = fonts.begin(); it != fonts.end(); it++) {
		SAFE_RELEASE(it->font);
	}

	SAFE_RELEASE(s);
	SAFE_RELEASE(vb);
	SAFE_RELEASE(ib);
}

void DX9Renderer::Begin() {
	this->s->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE | D3DXSPRITE_DO_NOT_ADDREF_TEXTURE);
}

void DX9Renderer::End() {
	Flush();

	s->End();

	size_t numSpheres = spheres.size();
	if (numSpheres) {
		DWORD oFillmode, oZenable, oStencilEnable;
		device->GetRenderState(D3DRS_FILLMODE, &oFillmode);
		device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		device->GetRenderState(D3DRS_ZENABLE, &oZenable);
		device->SetRenderState(D3DRS_ZENABLE, FALSE);
		device->GetRenderState(D3DRS_STENCILENABLE, &oStencilEnable);
		device->SetRenderState(D3DRS_STENCILENABLE, FALSE);
		IDirect3DPixelShader9 *ps;
		device->GetPixelShader(&ps);
		IDirect3DVertexShader9 *vs;
		device->GetVertexShader(&vs);
		device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
		device->SetStreamSource(0, vb, 0, sizeof(DX9Vertex3D));
		device->SetIndices(ib);

		for (size_t i = 0; i < numSpheres; i++) {
			Sphere    *sphere = &spheres[i];

			//D3DXMATRIX worldMat;
			//D3DXMatrixIdentity(&worldMat);
			//D3DXMatrixTranslation(&worldMat, sphere->x, sphere->y, sphere->z);
			//worldMat._11 = worldMat._22 = worldMat._33 = sphere->r;
			//device->SetTransform(D3DTS_WORLD, &worldMat);

			void *ptr;
			vb->Lock(0, icoSphereCreator.index * sizeof(DX9Vertex3D), &ptr, D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK);
			DX9Vertex3D *v = (DX9Vertex3D*)ptr;
			for (auto it = icoSphereCreator.geometry.positions.begin(); it != icoSphereCreator.geometry.positions.end(); it++) {
				v->x    = sphere->x + it->x * sphere->r;
				v->y    = sphere->y + it->y * sphere->r;
				v->z    = sphere->z + it->z * sphere->r;
				v->argb = sphere->c;
				v++;
			}
			//memcpy(ptr, icoSphereCreator.geometry.positions.base, icoSphereCreator.index * sizeof(DX9Vertex3D));
			vb->Unlock();
			ib->Lock(0, icoSphereCreator.geometry.indices.size() * sizeof(short), &ptr, D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK);
			memcpy(ptr, icoSphereCreator.geometry.indices.base, icoSphereCreator.geometry.indices.size() * sizeof(short));
			ib->Unlock();
			device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, icoSphereCreator.index, 0, icoSphereCreator.geometry.indices.size() / 3);
			//sphereMesh->DrawSubset(0);
		}

		spheres.clear();
		device->SetRenderState(D3DRS_STENCILENABLE, oZenable);
		device->SetRenderState(D3DRS_ZENABLE, oZenable);
		device->SetRenderState(D3DRS_FILLMODE, oFillmode);
	}
}

void DX9Renderer::Flush() {
	if (numVertices > 0) {
		device->SetTexture(0, NULL);
		void *ptr;
		vb->Lock(0, numVertices * sizeof(DX9Vertex2D), &ptr, D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK);
		memcpy(ptr, vertices, numVertices * sizeof(DX9Vertex2D));
		vb->Unlock();
		ib->Lock(0, numIndices * sizeof(short), &ptr, D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK);
		memcpy(ptr, indices, numIndices * sizeof(short));
		ib->Unlock();
		device->SetStreamSource(0, vb, 0, sizeof(DX9Vertex2D));
		device->SetIndices(ib);
		device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
		device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, numVertices, 0, numIndices / 3);
		numVertices = numIndices = 0;
		if (numLines > 0) {
			device->DrawPrimitiveUP(D3DPT_LINELIST, numLines / 2, lines, sizeof(DX9Vertex2D));
			numLines = 0;
		}
	} else {
		if (numLines > 0) {
			device->SetTexture(0, NULL);
			device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
			device->DrawPrimitiveUP(D3DPT_LINELIST, numLines / 2, lines, sizeof(DX9Vertex2D));
			numLines = 0;
		}
	}
}

void DX9Renderer::SetColor(_Color c) {
	currentColor = c;
}

DX9Texture *DX9Renderer::CreateBlankTexture(int w, int h) {
	DX9Texture     t;
	D3DLOCKED_RECT d3dlr;
	WORD          *pDst16 = NULL;

	if (FAILED(device->CreateTexture(w, h, 1, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, &t.texture, NULL))) {
		return NULL;
	}

	WORD colour16 = ((WORD)((currentColor >> 28) & 0xF) << 12) | (WORD)(((currentColor >> 20) & 0xF) << 8) | (WORD)(((currentColor >> 12) & 0xF) << 4) | (WORD)(((currentColor >> 4) & 0xF) << 0);

	t.texture->LockRect(0, &d3dlr, 0, 0);
	pDst16 = (WORD*)d3dlr.pBits;

	for (int xy = 0; xy < 8 * 8; xy++) {
		(*pDst16++) = colour16;
	}

	t.texture->UnlockRect(0);

	return S_OK;
}

DX9Texture *DX9Renderer::CreateTextureFromMemory(void *buffer, size_t len) {
	DX9Texture *texture = textures.push_new();
	texture->texture = NULL;
	D3DXCreateTextureFromFileInMemory(device, buffer, len, &texture->texture);
	return texture;
}

DX9Texture *DX9Renderer::CreateTextureFromFile(char *file) {
	DX9Texture *texture = textures.push_new();
	texture->texture = NULL;
	D3DXCreateTextureFromFileA(device, file, &texture->texture);
	return texture;
}

void DX9Renderer::RenderTexture(DX9Texture *t, int x, int y) {
	D3DXVECTOR3 pos;
	pos.x = x;
	pos.y = y;
	pos.z = 0.0f;
	s->Draw(t->texture, NULL, NULL, &pos, D3DCOLOR_ARGB(255, 255, 255, 255));
}

void DX9Renderer::RenderTexture(DX9Texture *t, int x, int y, RECT *srcRect) {
	D3DXVECTOR3 pos;
	pos.x = x;
	pos.y = y;
	pos.z = 0.0f;
	s->Draw(t->texture, srcRect, NULL, &pos, D3DCOLOR_ARGB(255, 255, 255, 255));
}

void DX9Renderer::ReleaseTexture(DX9Texture *t) {
	t->texture->Release();
	t->texture = NULL;
}

DX9Font *DX9Renderer::CreateNewFont(char *fontName, int size, unsigned int flags) {
	DX9Font f;
	f.font = NULL;
	D3DXCreateFontA(device, size, 0, (flags & FL_BOLD) ? 800 : 0, 0, flags & FL_ITALIC, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, fontName, &f.font);

	if (f.font != NULL) {
		RECT rect = { 0, 0, 1024, 480 };
		f.font->DrawTextA(s, "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890abcdefghijklmnopqrstuvwxyz", -1, &rect, DT_CALCRECT, NULL);
		f.h = rect.bottom;
		fonts.push_back(f);
		return fonts.end() - 1;
	}

	return NULL;
}

//Font *Renderer::CreateFontFromMemory
//Font *Renderer::CreateFontFromFile

void DX9Renderer::ReleaseFont(DX9Font *f) {
	SAFE_RELEASE(f->font);
}

void DX9Renderer::RenderText(DX9Font *f, int x, int y, char *text) {
	RECT clip = { x, y, 0, 0 };
	f->font->DrawTextA(s, text, -1, &clip, DT_LEFT | DT_TOP | DT_NOCLIP, currentColor);
}

void DX9Renderer::RenderText(DX9Font *f, int x, int y, char *text, _Color c) {
	RECT clip = { x, y, 0, 0 };
	f->font->DrawTextA(s, text, -1, &clip, DT_LEFT | DT_TOP | DT_NOCLIP, c);
}

void DX9Renderer::RenderCenteredText(DX9Font *f, int x, int y, char *text, _Color c) {
	RECT clip = { x, y, 0, 0 };
	f->font->DrawTextA(s, text, -1, &clip, DT_CENTER | DT_TOP | DT_NOCLIP, c);
}

void DX9Renderer::RenderTextf(DX9Font *f, int x, int y, char *format, ...) {
	va_list va_alist;
	char buf[128];

	va_start(va_alist, format);
	_vsnprintf_s(buf, sizeof(buf), format, va_alist);
	va_end(va_alist);
	RenderText(f, x, y, buf);
}

void DX9Renderer::RenderTextf(DX9Font *f, int x, int y, _Color c, char *format, ...) {
	va_list va_alist;
	char buf[128];

	va_start(va_alist, format);
	_vsnprintf_s(buf, sizeof(buf), format, va_alist);
	va_end(va_alist);
	RenderText(f, x, y, buf, c);

}

void DX9Renderer::RenderTextEx(DX9Font *f, RECT *r, char *text, unsigned int flags) {
	f->font->DrawTextA(s, text, -1, r, flags, currentColor);
}

void DX9Renderer::RenderTextEx(DX9Font *f, RECT *r, char *text, unsigned int flags, _Color c) {
	f->font->DrawTextA(s, text, -1, r, flags, c);
}

void DX9Renderer::RenderTextExf(DX9Font *f, RECT *r, unsigned int flags, char *format, ...) {
	va_list va_alist;
	char buf[128];

	va_start(va_alist, format);
	_vsnprintf_s(buf, sizeof(buf), format, va_alist);
	va_end(va_alist);
	RenderTextEx(f, r, buf, flags);
}

void DX9Renderer::CalcTextRect(DX9Font *f, char *text, RECT *r) {
	memset(r, 0, sizeof(RECT));
	f->font->DrawTextA(NULL, text, -1, r, DT_CALCRECT, 0);
}

void DX9Renderer::Calculate2DCirclePoints(int numSides, int radius, DX9Vertex2D *v) {
	double step = M_PI * 2.0 / numSides;
	double x    = 0.0;

	for (size_t i = 0; i < numSides; i++) {
		v[i].x = ((double)radius) * cos(x);
		v[i].y = ((double)radius) * sin(x);
		x     += step;
	}
}

void DX9Renderer::Render2DCircle(int vCount, DX9Vertex2D *v, int x, int y) {
	int          baseVertex = numVertices;
	DX9Vertex2D *sv         = ReserveVertices(vCount + 1);

	for (size_t i = 0; i < vCount; i++) {
		sv[i].x    = v[i].x + (float)x;
		sv[i].y    = v[i].y + (float)y;
		sv[i].z    = 0.f;
		sv[i].argb = currentColor;
	}

	sv[vCount].x    = (float)x;
	sv[vCount].y    = (float)y;
	sv[vCount].z    = 0.f;
	sv[vCount].argb = currentColor;

	short *si = ReserveIndices((vCount + 1) * 3);

	for (size_t i = 0; i < vCount; i++) {
		size_t _i = i * 3;
		si[_i]     = baseVertex + vCount;
		si[_i + 1] = baseVertex + i;
		si[_i + 2] = baseVertex + i + 1;
	}

	si[((vCount - 1) * 3) + 2] = baseVertex;
}

void DX9Renderer::Render2DCircle(int vCount, DX9Vertex2D *v, int x, int y, _Color c) {
	currentColor = c;
	Render2DCircle(vCount, v, x, y);
}

void DX9Renderer::Render2DCircleEdges(int vCount, DX9Vertex2D *v, int x, int y) { // eww
	int          baseVertex = numLines;
	DX9Vertex2D *sv         = &lines[numLines];
	numLines += vCount * 2;

	for (size_t i = 0; i < vCount; i++) {
		size_t si = i * 2;
		sv[si].x        = v[i].x + (float)x;
		sv[si].y        = v[i].y + (float)y;
		sv[si].z        = 0.f;
		sv[si].argb     = currentColor;
		sv[si + 1].x    = v[i + 1].x + (float)x;
		sv[si + 1].y    = v[i + 1].y + (float)y;
		sv[si + 1].z    = 0.f;
		sv[si + 1].argb = currentColor;
	}

	sv[((vCount - 1) * 2) + 1].x = v[0].x + (float)x;
	sv[((vCount - 1) * 2) + 1].y = v[0].y + (float)y;
}

void DX9Renderer::Render2DCircleEdges(int vCount, DX9Vertex2D *v, int x, int y, _Color c) {
	currentColor = c;
	Render2DCircleEdges(vCount, v, x, y);
}

void DX9Renderer::Render2DTriangle(int x1, int y1, int x2, int y2, int x3, int y3) {
	AddVertex(x1, y1);
	AddVertex(x2, y2);
	AddVertex(x3, y3);
}

void DX9Renderer::Render2DTriangle(int x1, int y1, int x2, int y2, int x3, int y3, _Color c) {
	currentColor = c;
	Render2DTriangle(x1, y1, x2, y2, x3, y3);
}

void DX9Renderer::Render2DRect(int x, int y, int w, int h) {
	DX9Vertex2D v[4] = { { x, y, 0, 1, currentColor }, { x + w, y, 0, 1, currentColor }, { x, y + h, 0, 1, currentColor }, { x + w, y + h, 0, 1, currentColor } };
	short       i[6] = { numVertices, numVertices + 1, numVertices + 2, numVertices + 1, numVertices + 2, numVertices + 3 };
	RenderIndexedTriangle(v, 4, i, 6);
}

void DX9Renderer::Render2DRect(int x, int y, int w, int h, _Color c) {
	SetColor(c);
	Render2DRect(x, y, w, h);
}

void DX9Renderer::Render2DRectEdges(int x, int y, int w, int h, int t) {
	x -= t;
	y -= t;
	w += t * 2.0f;
	h += t * 2.0f;

	Render2DRect(x, y, w, t);

	y += t;

	Render2DRect(x, y, t, h - (2.0f * t));

	y += (h - (2.0f * t));

	Render2DRect(x, y, w, t);

	y -= (h - (2.0f * t));
	x += (w - t);

	Render2DRect(x, y, t, h - (2.0f * t));
}

void DX9Renderer::Render2DRectEdges(int x, int y, int w, int h, int t, _Color c) {
	SetColor(c);
	Render2DRectEdges(x, y, w, h, t);
}

void DX9Renderer::Render2DOutlinedRect(int x, int y, int w, int h, int t, _Color fillColor, _Color outlineColor) {
	int t2 = t * 2;
	Render2DRect(x - t, y - t, w + t2, h + t2, outlineColor);
	Render2DRect(x, y, w, h, fillColor);
}

void DX9Renderer::Render2DGradientRect(int dir, _Color start, _Color end, int x, int y, int w, int h) {
	if (dir == DIR_UPDOWN) {
		DX9Vertex2D v[4] = { { x, y, 0, 1, start }, { x + w, y, 0, 1, start }, { x, y + h, 0, 1, end }, { x + w, y + h, 0, 1, end } };
		short       i[6] = { numVertices, numVertices + 1, numVertices + 2, numVertices + 1, numVertices + 2, numVertices + 3 };
		RenderIndexedTriangle(v, 4, i, 6);
	} else if (dir == DIR_LEFTRIGHT) {
		DX9Vertex2D v[4] = { { x, y, 0, 1, start }, { x + w, y, 0, 1, end }, { x, y + h, 0, 1, start }, { x + w, y + h, 0, 1, end } };
		short       i[6] = { numVertices, numVertices + 1, numVertices + 2, numVertices + 1, numVertices + 2, numVertices + 3 };
		RenderIndexedTriangle(v, 4, i, 6);
	} else if (dir == DIR_LEFTDOWN) { // ugly
		DX9Vertex2D v[3] = { { x, y, 0, 1, start }, { x + w, y, 0, 1, end }, { x, y + h, 0, 1, end } };
		short       i[3] = { numVertices, numVertices + 1, numVertices + 2 };
		RenderIndexedTriangle(v, 3, i, 3);
	}
}

void DX9Renderer::Render2DLine(int x1, int y1, int x2, int y2, int t) {
	/* // TODO: render 2d lines using triangles
	int dx = x1 - x2,
		dy = y1 - y2;

	double length = sqrt((double)((dx * dx) + (dy * dy))),
		   angle  = RAD2DEG(atan(((double)(y2 - y1)) / ((double)(x2 - x1))));
	angle = DEG2RAD(NormalizeAngle180(angle - 90.0));
	double cosang = cos(angle), sinang = sin(angle);
	
	float _x, _y, _r, _b;
	if (x1 > x2) {
		_x = x2;
		_r = x1;
	} else {
		_x = x1;
		_r = x2;
	}
	if (y1 > y2) {
		_y = y2;
		_b = y1;
	} else {
		_y = y1;
		_b = y2;
	}
	float x, y, r, b;
	x = _x * cosang + _y * sinang;
	y = _y * cosang + _x * sinang;
	r = _r * cosang + _b * sinang;
	b = _b * cosang + _r * sinang;

	DX9Vertex2D v[4] = { { x, y, 0, 1, currentColor }, { r, y, 0, 1, currentColor }, { x, b, 0, 1, currentColor }, { r, b, 0, 1, currentColor } };
	short       i[6] = { numVertices, numVertices + 1, numVertices + 2, numVertices + 1, numVertices + 2, numVertices + 3 };
	RenderIndexedTriangle(v, 4, i, 6);
	
	v[i].x = ((double)radius) * cos(x);
	v[i].y = ((double)radius) * sin(x);
	*/

	lines[numLines].x      = x1;
	lines[numLines].y      = y1;
	lines[numLines].z      = 0.f;
	lines[numLines++].argb = currentColor;
	lines[numLines].x      = x2;
	lines[numLines].y      = y2;
	lines[numLines].z      = 0.f;
	lines[numLines++].argb = currentColor;
}

void DX9Renderer::AddVertex(int x, int y) {
	AddVertex(x, y, currentColor);
}

void DX9Renderer::AddVertex(int x, int y, _Color c) {
	indices[numIndices++]        = numVertices;
	vertices[numVertices].x      = (float)x;
	vertices[numVertices].y      = (float)y;
	vertices[numVertices++].argb = c;
}

void DX9Renderer::RenderIndexedTriangle(DX9Vertex2D *v, int vCount, short *i, int iCount) {
	memcpy((void*)&vertices[numVertices], (void*)v, sizeof(DX9Vertex2D) * vCount);
	numVertices += vCount;
	memcpy((void*)&indices[numIndices], (void*)i, sizeof(short) * iCount);
	numIndices += iCount;
}

DX9Vertex2D *DX9Renderer::ReserveVertices(size_t num) {
	DX9Vertex2D *res = &vertices[numVertices];
	numVertices += num;
	return res;
}

short *DX9Renderer::ReserveIndices(size_t num) {
	short *res = &indices[numIndices];
	numIndices += num;
	return res;
}

void DX9Renderer::Render3DCube(float x, float y, float w, float h) {
	//DX9Vertex3D v[3] = {};
	//device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, v, sizeof(DX9Vertex3D));
}

LPD3DXMESH DX9Renderer::Create3DSphere(int slices) { // dis unnessecary, create 1 sphere and cache of spheredraws and use THE MATRIX
	LPD3DXMESH   tempSphere;
	LPD3DXBUFFER adj;

	if (D3DXCreateSphere(device, 1, slices, slices, &tempSphere, &adj) == D3D_OK) {
		LPD3DXMESH sphere;
		tempSphere->CloneMeshFVF(0, tempSphere->GetFVF(), device, &sphere);
		tempSphere->Release();
		if (sphere->OptimizeInplace(D3DXMESHOPT_ATTRSORT, (DWORD*)adj->GetBufferPointer(), NULL, NULL, NULL) == D3D_OK) {
			return sphere;
		}
		sphere->Release();
	}

	return NULL;
}

void DX9Renderer::Render3DSphere(float x, float y, float z, float r) {
	Sphere *sphere = spheres.push_new();
	sphere->x = x;
	sphere->y = y;
	sphere->z = z;
	sphere->r = r;
	sphere->c = currentColor;
}

void DX9Renderer::Render3DSphere(float x, float y, float z, float r, _Color c) {
	currentColor = c;
	Render3DSphere(x, y, z, r);
}

IcoSphereCreator::IcoSphereCreator(void) {
	recLevel = 0;
}

IcoSphereCreator::~IcoSphereCreator(void) {
}

int IcoSphereCreator::addVertex(D3DXVECTOR3 p) {
	double       length = sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
	DX9Vertex3D *v      = geometry.positions.push_new();
	v->x    = p.x / length;
	v->y    = p.y / length;
	v->z    = p.z / length;
	v->argb = D3DCOLOR_ARGB(255, 255, 255, 255);
	return index++;
}

int IcoSphereCreator::getMiddlePoint(int p1, int p2) {
	bool    firstIsSmaller = p1 < p2;
	__int64 smallerIndex   = firstIsSmaller ? p1 : p2;
	__int64 greaterIndex   = firstIsSmaller ? p2 : p1;
	__int64 key            = (smallerIndex << 32) + greaterIndex;

	if (middlePointIndexCache.find(key) != middlePointIndexCache.end()) {
		return middlePointIndexCache[key];
	}

	D3DXVECTOR3 point1 = D3DXVECTOR3(geometry.positions[p1].x, geometry.positions[p1].y, geometry.positions[p1].z);
	D3DXVECTOR3 point2 = D3DXVECTOR3(geometry.positions[p2].x, geometry.positions[p2].y, geometry.positions[p2].z);
	D3DXVECTOR3 middle = D3DXVECTOR3((point1.x + point2.x) / 2.0, (point1.y + point2.y) / 2.0, (point1.z + point2.z) / 2.0);

	int         i = addVertex(middle);

	middlePointIndexCache[key] = i;
	return i;
}

MeshGeometry3D *IcoSphereCreator::Create(int recursionLevel) {
	if (recLevel == recursionLevel) {
		return &geometry;
	}

	geometry.positions.clear();
	geometry.indices.clear();
	index = 0;

	float t = (1.0 + sqrt(5.0f)) / 2.0;
	addVertex(D3DXVECTOR3(-1, t, 0));
	addVertex(D3DXVECTOR3(1, t, 0));
	addVertex(D3DXVECTOR3(-1, -t, 0));
	addVertex(D3DXVECTOR3(1, -t, 0));
	addVertex(D3DXVECTOR3(0, -1, t));
	addVertex(D3DXVECTOR3(0, 1, t));
	addVertex(D3DXVECTOR3(0, -1, -t));
	addVertex(D3DXVECTOR3(0, 1, -t));
	addVertex(D3DXVECTOR3(t, 0, -1));
	addVertex(D3DXVECTOR3(t, 0, 1));
	addVertex(D3DXVECTOR3(-t, 0, -1));
	addVertex(D3DXVECTOR3(-t, 0, 1));

	_Vector<D3DXVECTOR3> faces;
	faces.push_back(D3DXVECTOR3(0, 11, 5));
	faces.push_back(D3DXVECTOR3(0, 5, 1));
	faces.push_back(D3DXVECTOR3(0, 1, 7));
	faces.push_back(D3DXVECTOR3(0, 7, 10));
	faces.push_back(D3DXVECTOR3(0, 10, 11));

	faces.push_back(D3DXVECTOR3(1, 5, 9));
	faces.push_back(D3DXVECTOR3(5, 11, 4));
	faces.push_back(D3DXVECTOR3(11, 10, 2));
	faces.push_back(D3DXVECTOR3(10, 7, 6));
	faces.push_back(D3DXVECTOR3(7, 1, 8));

	faces.push_back(D3DXVECTOR3(3, 9, 4));
	faces.push_back(D3DXVECTOR3(3, 4, 2));
	faces.push_back(D3DXVECTOR3(3, 2, 6));
	faces.push_back(D3DXVECTOR3(3, 6, 8));
	faces.push_back(D3DXVECTOR3(3, 8, 9));

	faces.push_back(D3DXVECTOR3(4, 9, 5));
	faces.push_back(D3DXVECTOR3(2, 4, 11));
	faces.push_back(D3DXVECTOR3(6, 2, 10));
	faces.push_back(D3DXVECTOR3(8, 6, 7));
	faces.push_back(D3DXVECTOR3(9, 8, 1));

	for (int i = 0; i < recursionLevel; i++) {
		_Vector<D3DXVECTOR3> faces2;
		size_t              numFaces = faces.size();
		faces2.reserve(numFaces * 4);

		for (int r = 0; r < faces.size(); r++) {
			D3DXVECTOR3 tri = faces[r];

			int         a = getMiddlePoint(tri.x, tri.y);
			int         b = getMiddlePoint(tri.y, tri.z);
			int         c = getMiddlePoint(tri.z, tri.x);

			faces2.push_back(D3DXVECTOR3(tri.x, a, c));
			faces2.push_back(D3DXVECTOR3(tri.y, b, a));
			faces2.push_back(D3DXVECTOR3(tri.z, c, b));
			faces2.push_back(D3DXVECTOR3(a, b, c));
		}

		faces = faces2;
	}

	for (int r = 0; r < faces.size(); r++) {
		D3DXVECTOR3 tri = faces[r];
		geometry.indices.push_back(tri.x);
		geometry.indices.push_back(tri.y);
		geometry.indices.push_back(tri.z);
	}

	return &geometry;
}
