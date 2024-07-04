#ifndef _3DRAYCASTER_
#define _3DRAYCASTER_

#include <iostream>
#include <cmath>

#include "ConsoleGraphics.hpp"
#define INCREASE_MOUSE_ACCURACY
#include "ConsoleUtilities.h"

#include "RayCasterThread.hpp"
#include "Movement.hpp"

class RayCaster
{
	uint32 state; //0 = good, non zero = error
	cg::Image texture;

	FPVector2D fov;
	const float speed = 2.5f;
	FPVector3D rotation; //z rotation doesn't work (roll)
	std::vector<std::vector<std::vector<bool>>> map;
	FPVector3D pos;
	const float rayMax = 7.5f, rayStep = 0.05f;
	float elapsedTime = 0.f;
	
	cg::ConsoleGraphics* graphics;
	std::deque<RCThreadData> rcThreadData;
	std::deque<HANDLE> threads;
	bool running = true;
	protected:

	public:
		RayCaster(int nArgs, char** args)
		{
			graphics = new cg::ConsoleGraphics(1920, 1080, true, 6, true);
			graphics->enableAlpha();

			state = 0;
			uint32 textureData[] = {0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0x7FFFFFFF};
			texture = cg::Image("File443.bmp"); //cg::Image(textureData, 2, 2, true);
			//texture.scale(128.f, cg::InterpolationMethod::Bisinusoidal);

			rotation = FPVector3D(0.f, 0.f, 0.f);
			pos = FPVector3D(3.5f, 0.5f, 1.5f);
			fov = FPVector2D(90.f, 60.f);

			map.resize(3);
			map[0].resize(9);
			map[0][0] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
			map[0][1] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
			map[0][2] = {1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1};
			map[0][3] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
			map[0][4] = {1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1};
			map[0][5] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1};
			map[0][6] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
			map[0][7] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1};
			map[0][8] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
			map[1].resize(9);
			map[1][0] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
			map[1][1] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
			map[1][2] = {1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1};
			map[1][3] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1};
			map[1][4] = {1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1};
			map[1][5] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1};
			map[1][6] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
			map[1][7] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1};
			map[1][8] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
			map[2].resize(9);
			map[2][0] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
			map[2][1] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
			map[2][2] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
			map[2][3] = {1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1};
			map[2][4] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
			map[2][5] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
			map[2][6] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
			map[2][7] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
			map[2][8] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

			uint32 threadsToCreate = 4;
			for (uint32 i = 0, posY = 0; i < threadsToCreate; i++)
			{
				RCThreadData rctd;
				rctd.startY = posY;

				uint32 h;
				if (i == threadsToCreate - 1){h = graphics->getHeight() - posY;
				} else h = graphics->getHeight() / threadsToCreate;
				posY += h;
				rctd.height = h;

				rctd.graphics = graphics;
				rctd.fov = &fov;
				rctd.map = &map;
				FPVector3D mapBounds(map[0].size(), map.size(), map[0][0].size());
				rctd.mapBounds = mapBounds;
				rctd.pos = &pos;
				rctd.rot = &rotation;
				rctd.rayMax = rayMax;
				rctd.rayStep = rayStep;
				rctd.texture = texture;

				std::string drawNextStr = "DrawNext" + std::to_string(i);
				std::string hasDrawnStr = "HasDrawn" + std::to_string(i);
				rctd.drawNext = CreateEvent(NULL, true, false, drawNextStr.c_str());
				rctd.hasDrawn = CreateEvent(NULL, true, false, hasDrawnStr.c_str());

				rctd.isRunning = &this->running;

				rcThreadData.push_back(rctd);
			}

			for (auto& td : rcThreadData)
			{
				threads.push_back(CreateThread(NULL, 0, RCThread, reinterpret_cast<void*>(&td), 0, NULL));
			}
		}
		~RayCaster()
		{
			delete graphics;
		}

		bool isRunning(void){return state == 0;
		}

		void update(float dT)
		{
			getCursorPos();
			static int32 mouseX = cu::cursor.x, mouseY = cu::cursor.y;
			float rotW = 10.f * (mouseX - cu::cursor.x) * dT;
			float rotH = 10.f * (mouseY - cu::cursor.y) * dT;
			rotation.x += rotW;
			rotation.y += rotH;
			if (rotation.y > 45.f){rotation.y = 45.f;
			} else if (rotation.y < -45.f){rotation.y = -45.f;
			}

			if (GetAsyncKeyState(VK_ESCAPE)){state = 1;
			}

			float spd = 2.5f;
			updateMovement(pos, rotation, spd, dT);

			//if (GetAsyncKeyState('Q'))
			//{
			//	rotation.x -= 50.f * dT;
			//}
			//else if (GetAsyncKeyState('W'))
			//{
			//	rotation.x += 50.f * dT;
			//}
			if (rotation.x < 0.f)
			{
				rotation.x += 360.f;
			}
			rotation.x = fmod(rotation.x, 360.f);

			//pos.y = 0.5f * sinf(degToRad(elapsedTime * 50.f)) + 1.f;
			std::string out = "(" + std::to_string(rotation.x) + ")[" + std::to_string(pos.x) + ", " + std::to_string(pos.y);
			out += ", " + std::to_string(pos.z) + "] FPS:"+std::to_string(1/dT);
			graphics->setTitle(out);
			mouseX = cu::cursor.x;
			mouseY = cu::cursor.y;

			elapsedTime += dT;

			return;
		}

		void draw(void)
		{
			if (state != 0){return;
			}

			graphics->clear();

			for (auto& d : rcThreadData)
			{
				ResetEvent(d.hasDrawn);
				SetEvent(d.drawNext);
			}
			for (auto& d : rcThreadData)
			{
				WaitForSingleObject(d.hasDrawn, 30);
			}

			cg::Image mmap(map[0].size(), map[0][0].size(), cg::BGR(0, 255, 0));
			for (uint32 i = 0; i < map[0].size(); i++)
			{
				for (uint32 j = 0; j < map[0][0].size(); j++)
				{
					if (map[0][i][j])
					{
						mmap.setPixel(i, j, 0x00FFFFFF);
					}
				}
			}
			mmap.setPixel(pos.x, pos.z, cg::BGR(255, 0, 0));
			mmap.scale(4.f);
			mmap.setPos(graphics->getWidth() - mmap.getWidth(), graphics->getHeight() - mmap.getHeight());
			graphics->draw(mmap);

			//graphics->draw(texture);
			graphics->display();
			return;
		}

		uint32 close(void)
		{
			//Allow threads to close normally
			this->running = false;
			for (auto& e : rcThreadData)
			{
				SetEvent(e.drawNext);
			}

			for (HANDLE& t : threads)
			{
				if (WaitForSingleObject(t, 500) == WAIT_TIMEOUT)
				{
					std::cout << "Thread has timed out, terminating." << std::endl;
					TerminateThread(t, 2);
				}
				CloseHandle(t);
			}

			for (auto& e : rcThreadData)
			{
				CloseHandle(e.drawNext);
				CloseHandle(e.hasDrawn);
			}

			return state;
		}
};

#endif // !_3DRAYCASTER_

