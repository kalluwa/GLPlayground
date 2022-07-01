#ifndef _DEBUG_VIEWER_H_
#define _DEBUG_VIEWER_H_

class GameApp;
class Shader;

class DebugViewer
{
	GameApp* app = nullptr;

public:
	DebugViewer(GameApp* app);

	void drawTex(unsigned int texid,int x,int y,int width,int height);

	Shader* shader = nullptr;
};
#endif