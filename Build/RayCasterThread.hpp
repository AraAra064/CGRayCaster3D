#ifndef RAYCASTER_THREAD
#define RAYCASTER_THREAD

//#include <thread>
#include <mutex>
//<windows.h>
//"ConsoleGraphics"

struct RCThreadData
{

	FPVector3D* pos;
	FPVector3D* rot;
	FPVector2D* fov;
	cg::Image texture; //each thread gets its own copy
	cg::ConsoleGraphics* graphics;
	uint32 startY, height;
	std::vector<std::vector<std::vector<bool>>>* map;
	float rayMax, rayStep;
	FPVector3D mapBounds;
	HANDLE drawNext, hasDrawn;
	bool* isRunning;

	RCThreadData()
	{
		pos = nullptr;
		rot = nullptr;
		fov = nullptr;
		texture = cg::Image(256, 256, 0x00FFFFFFFF);
		graphics = nullptr;
		startY = 0;
		height = 0;
		rayMax = 1.f;
		rayStep = 0.1f;
		drawNext = NULL;
		hasDrawn = NULL;
		map = nullptr;
		isRunning = nullptr;
	}
	~RCThreadData()
	{
		pos = nullptr;
		rot = nullptr;
		fov = nullptr;
		graphics = nullptr;
		map = nullptr;
		isRunning = nullptr;
	}
};

/*//float d1 = fabs(ray.x - round(ray.x));
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
break;*/

DWORD WINAPI RCThread(void* data)
{
	RCThreadData* threadData = reinterpret_cast<RCThreadData*>(data);
	FPVector3D* pos = threadData->pos;
	FPVector3D* rot = threadData->rot;
	FPVector2D* fov = threadData->fov;
	cg::ConsoleGraphics* graphics = threadData->graphics;
	auto map = threadData->map;
	FPVector3D mapBounds = threadData->mapBounds;
	float rayMax = threadData->rayMax;
	float rayStep = threadData->rayStep;

	//Set up vars
	FPVector3D dRot(fov->x / graphics->getWidth(), fov->y / graphics->getHeight(), 0.f);
	FPVector3D ray, rayDir, rayRot; //absolute and relative rotations

	while (*threadData->isRunning)
	{
		for (uint32 y = 0; y < threadData->height; y++)
		{
			for (uint32 x = 0; x < graphics->getWidth(); x++)
			{
				//Degrees
				rayRot = FPVector3D((dRot.x * x) - (fov->x * 0.5f), (dRot.y * (y + threadData->startY)) - (fov->y * 0.5f), 0.f);
				//Radians
				rayDir.fromAngle(-degToRad(rot->x + rayRot.x), -degToRad(rot->y + rayRot.y));
				ray = *pos;

				for (float rayDist = 0.f; rayDist < rayMax; rayDist += rayStep)
				{
					ray.x += rayStep * rayDir.x;
					ray.y += rayStep * rayDir.y;
					ray.z += rayStep * rayDir.z;

					bool isNegative = (ray.x < 0.f || ray.y < 0.f || ray.z < 0.f);
					bool inBounds = (ray.x < mapBounds.x && ray.y < mapBounds.y && ray.z < mapBounds.z);

					if (isNegative || (inBounds && (*map)[(int)ray.y][(int)ray.x][(int)ray.z]))
					{
						//Fix distortion correction and fix texture sampling
						rayDist *= std::cosf(degToRad(rayRot.x)) * std::cosf(degToRad(rayRot.y));
						float texX, texY;
						auto im = cg::InterpolationMethod::Bisinusoidal;
						auto em = cg::ExtrapolationMethod::Repeat;

						if (isNegative) //Floor
						{
							texX = ray.x - std::floor(ray.x);
							texY = ray.z - std::floor(ray.z);
						}
						else
						{
							float d1 = std::fabs(ray.x - std::round(ray.x));
							float d2 = std::fabs(ray.z - std::round(ray.z));
							float d3 = std::fabs(ray.y - std::round(ray.y));

							texY = ray.y;

							if (d1 < d2 && d2 < d3) //Intersection at X boundary, use Z and X
							{
								texX = ray.z;
							}
							else if (d2 < d1 && d2 < d3) //Intersection at Z boundary, use X and X
							{
								texX = ray.x;
							}
							else
							{
								texX = ray.z;
								texY = ray.x;
							}
						}

						auto p = threadData->texture.samplePixel(texX, texY, im, em);
						float alpha = 1.f - (rayDist / rayMax);
						graphics->drawPixel(x, y + threadData->startY, p.first, alpha * 255);

						break;
					}
				}
			}
		}

		SetEvent(threadData->hasDrawn);
		WaitForSingleObject(threadData->drawNext, INFINITE);
		ResetEvent(threadData->drawNext);
	}

	ExitThread(0);
}

#endif // !RAYCASTER_THREAD

