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
	bool draw;
	bool*** map;
	float rayMax;

	RCThreadData()
	{
		startY = 0;
		height = 0;
		draw = false;
		map = nullptr;
	}
	~RCThreadData()
	{
		draw = false;
		graphics = nullptr;
		map = nullptr;
	}
};

/*FPVector3D dir, ray;
float rotW = rotation - (fovW / 2), rotWStep = fovW / graphics->getWidth();
float rotH = 0.f - (fovH / 2), rotHStep = fovH / graphics->getHeight();
for (uint32 y = 0; y < graphics->getHeight(); y++)
{
	for (uint32 x = 0; x < graphics->getWidth(); x++)
	{
		dir.fromAngle(degToRad(-rotW), degToRad(-rotH));
		ray = pos;
		//Offset ray distance to correct distortion

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
				//rayDist *= cosf(degToRad(-rotW)) * cosf(degToRad(-rotH));
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

		rotW += rotWStep;
	}
	rotW = rotation - (fovW / 2);
	rotH += rotHStep;
}*/

DWORD WINAPI RCThread(void* data)
{
	RCThreadData* threadData = reinterpret_cast<RCThreadData*>(data);
	FPVector3D* pos = threadData->pos;
	FPVector3D* rot = threadData->rot;
	FPVector2D* fov = threadData->fov;
	cg::ConsoleGraphics* graphics = threadData->graphics;
	const bool*** map = threadData->map;

	//Set up vars
	FPVector3D dRot(fov->x / graphics->getWidth(), fov->y / graphics->getHeight(), 0.f);
	FPVector3D ray, rayDir, rayRot; //absolute and relative rotations

	//loop

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

				if (map[(int)ray.y][(int)ray.x][(int)ray.z])
				{
					break;
				}
			}

			graphics->setPixel(x, y + threadData->startY, p);
		}
	}

	//loop

	ExitThread(0);
}

#endif // !RAYCASTER_THREAD

