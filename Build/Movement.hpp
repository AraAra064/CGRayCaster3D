#ifndef RAYCASTER_MOVEMENT
#define RAYCASTER_MOVEMENT

void updateMovement(FPVector3D& pos, FPVector3D& rot, float spd, float dT)
{
	FPVector3D dir(0.f, 0.f, 0.f);

	if (GetAsyncKeyState(VK_UP))
	{
		dir.fromAngle(degToRad(-rot.x), 0.f);
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		dir.fromAngle(degToRad(-rot.x + 180.f), 0.f);
	}
	if (GetAsyncKeyState(VK_LEFT))
	{
		dir.fromAngle(degToRad(-rot.x + 90.f), 0.f);
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		dir.fromAngle(degToRad(-rot.x + 270.f), 0.f);
	}

	pos.x += dir.x * spd * dT;
	pos.y += dir.y * spd * dT;
	pos.z += dir.z * spd * dT;

	return;
}

#endif // !RAYCASTER_MOVEMENT


