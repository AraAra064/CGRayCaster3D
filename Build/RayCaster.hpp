#ifndef _3DRAYCASTER_
#define _3DRAYCASTER_

#include <iostream>
#include <cmath>

#include "ConsoleGraphics.hpp"
#define INCREASE_MOUSE_ACCURACY
#include "ConsoleUtilities.h"

//#include "RayCasterThread.hpp"

class RayCaster
{
	uint32 state; //0 = good, non zero = error
	cg::Image texture;

	const float fovW = 90.f, fovH = 60.f;
	const float speed = 2.5f;
	FPVector3D rotation; //z rotation doesn't work
	std::vector<std::vector<std::vector<bool>>> map;
	FPVector3D pos;
	const float rayMax = 7.5f;

	cg::ConsoleGraphics* graphics;

	protected:

	public:
		RayCaster(int nArgs, char** args)
		{
			graphics = new cg::ConsoleGraphics(1920, 1080, true, 8, true);
			graphics->enableAlpha();

			state = 0;
			uint32 textureData[] = {0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0x7FFFFFFF};
			texture = cg::Image("File443.bmp"); //cg::Image(textureData, 2, 2, true);
			//texture.scale(128.f, cg::InterpolationMethod::Bisinusoidal);

			rotation = FPVector3D(0.f, 0.f, 0.f);
			pos = FPVector3D(3.5f, 0.5f, 1.5f);

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
			map[2][3] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
			map[2][4] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
			map[2][5] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
			map[2][6] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
			map[2][7] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
			map[2][8] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
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
			float movDist = 2.5f;
			if (GetAsyncKeyState(VK_UP))
			{
				FPVector3D dir;
				dir.fromAngle(degToRad(-rotation.x), 0.f);
				pos.x += dir.x * movDist * dT;
				pos.y += dir.y * movDist * dT;
				pos.z += dir.z * movDist * dT;
			}
			if (GetAsyncKeyState(VK_DOWN))
			{
				FPVector3D dir;
				dir.fromAngle(degToRad(-rotation.x + 180.f), 0.f);
				pos.x += dir.x * movDist * dT;
				pos.y += dir.y * movDist * dT;
				pos.z += dir.z * movDist * dT;
			}
			if (GetAsyncKeyState(VK_LEFT))
			{
				FPVector3D dir;
				dir.fromAngle(degToRad(-rotation.x + 90.f), 0.f);
				pos.x += dir.x * movDist * dT;
				pos.y += dir.y * movDist * dT;
				pos.z += dir.z * movDist * dT;
			}
			if (GetAsyncKeyState(VK_RIGHT))
			{
				FPVector3D dir;
				dir.fromAngle(degToRad(-rotation.x + 270.f), 0.f);
				pos.x += dir.x * movDist * dT;
				pos.y += dir.y * movDist * dT;
				pos.z += dir.z * movDist * dT;

			}

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

			//posY = sinf(degToRad(rotation)) - 1.f;
			std::string out = "(" + std::to_string(rotation.x) + ")[" + std::to_string(pos.x) + ", " + std::to_string(pos.y);
			out += ", " + std::to_string(pos.z) + "] FPS:"+std::to_string(1/dT);
			graphics->setTitle(out);
			mouseX = cu::cursor.x;
			mouseY = cu::cursor.y;
			return;
		}

		void draw(void)
		{
			if (state != 0){return;
			}
			graphics->clear();

			FPVector3D dir, ray;
			float rotWStep = fovW / graphics->getWidth();
			float rotHStep = fovH / graphics->getHeight();
			for (uint32 y = 0; y < graphics->getHeight(); y++)
			{
				for (uint32 x = 0; x < graphics->getWidth(); x++)
				{
					FPVector3D rayRot(-(fovW / 2) + (rotWStep * x), -(fovH / 2) + (rotHStep * y), 0.f);

					dir.fromAngle(degToRad(-(rotation.x + rayRot.x)), degToRad(-(rotation.y + rayRot.y)));
					ray = pos;

					float rayStep = 0.05f;

					for (float rayDist = 0.f; rayDist < rayMax; rayDist += rayStep)
					{
						ray.x += rayStep * dir.x;
						ray.y += rayStep * dir.y;
						ray.z += rayStep * dir.z;
						//rayStep *= 2.f;

						bool negative = (ray.x < 0.f || ray.y < 0.f || ray.z < 0.f);
						bool inBounds = (ray.x < map[0].size() && ray.y < map.size() && ray.z < map[0][0].size());
						if (negative || (inBounds && map[trunc(ray.y)][trunc(ray.x)][trunc(ray.z)]))
						{
							//rayDist *= cosf(degToRad(-rayRot.x)) * cosf(degToRad(-rayRot.y));
							float a = 1.f - (rayDist / rayMax);
							uint8 _a = a * 255;
							graphics->drawPixel(x, y, cg::BGR(_a, _a, _a), 255);

							float d1 = fabs(ray.x - round(ray.x));
							float d2 = fabs(ray.z - round(ray.z));

							float texX, texY = ray.y;

							if (d1 < d2) //Intersection at X boundary, use Z and X
							{
								texX = ray.z;
							}
							else //Intersection at Z boundary, use X and X
							{
								texX = ray.x;
							}

							if (negative)
							{
								texX = ray.x;
								texY = ray.z;
								auto im = cg::InterpolationMethod::Bisinusoidal;
								auto em = cg::ExtrapolationMethod::Repeat;
								graphics->drawPixel(x, y, texture.samplePixel(texX, texY, im, em).first, _a);
							}
							else
							{
								auto im = cg::InterpolationMethod::Bisinusoidal;
								auto em = cg::ExtrapolationMethod::Repeat;
								graphics->drawPixel(x, y, texture.samplePixel(texX, texY, im, em).first, _a);
							}

							//float d1 = fabs(ray.x - round(ray.x));
							//float d2 = fabs(ray.z - round(ray.z));
							//float d3 = fabs(ray.y - round(ray.y));
							//
							//float texX, texY = ray.y;
							//uint32 t;
							//
							//if (d1 < d2 && d2 < d3) //Intersection at X boundary, use Z and X
							//{
							//	texX = ray.z;
							//}
							//else if (d2 < d1 && d2 < d3) //Intersection at Z boundary, use X and X
							//{
							//	texX = ray.x;
							//}
							//else //Intersection at Y boundary, use Z as X and X as Y
							//{
							//	texX = ray.z;
							//	texY = ray.x;
							//}
							//
							//auto im = cg::InterpolationMethod::Bisinusoidal;
							//auto em = cg::ExtrapolationMethod::Repeat;
							//graphics->drawPixel(x, y, texture.samplePixel(texX, texY, im, em).first, _a);
							break;
						}
					}
				}
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
			return state;
		}
};

#endif // !_3DRAYCASTER_

